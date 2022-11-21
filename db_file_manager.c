//
// Created by yars on 17.10.22.
//

#include "db_file_manager.h"

size_t min_size(size_t a, size_t b)
{
    return a < b ? a : b;
}

void freePage(DataPage *dataPage)
{
    if (!dataPage) return;
    dataPage->header.flags = 0;
    dataPage->header.next_related_page = dataPage->header.page_index;
    dataPage->header.data_size = 0;
    updatePageStatus(dataPage, PAGE_STATUS_INACTIVE);
    memset(dataPage->header.metadata,'\0',PAGE_METADATA_SIZE);
    memset(dataPage->page_data,'\0',PAGE_DATA_SIZE);
}

void freePageThread(const char *filename, size_t page_index)
{
    if (!filename) return;
    for (uint64_t current = page_index, last = current + 1; current != last;) {
        last = current;
        DataPage *dataPage = (DataPage*) malloc(sizeof(DataPage));
        readDataPage(filename, dataPage, current);
        current = dataPage->header.next_related_page;
        freePage(dataPage);
        writeDataPage(filename, dataPage);
        free(dataPage);
    }
}

void freeDatabaseFile(const char *filename)
{
    FILE *file = fopen(filename, "w");
    fclose(file);
}

void createDatabasePage(const char *filename, const char *db_name)
{
    if (strlen(db_name) >= PAGE_E_HEADER_MD_SIZE) return;
    DataPage *dbHeaderPage = (DataPage*) malloc(sizeof(DataPage));
    freeDatabaseFile(filename);
    freePage(dbHeaderPage);
    updatePageType(dbHeaderPage, PAGE_TYPE_DATABASE_HEADER);
    updatePageStatus(dbHeaderPage, PAGE_STATUS_ACTIVE);
    dbHeaderPage->header.page_index = 0;
    dbHeaderPage->header.next_related_page = dbHeaderPage->header.page_index;
    dbHeaderPage->header.data_size = 0;
    updateNumberOfPages(dbHeaderPage, 1);
    updateNumberOfTables(dbHeaderPage, 0);
    updateHeaderPageMetadata(dbHeaderPage, db_name);
    writeDataPage(filename, dbHeaderPage);
    free(dbHeaderPage);
}

void expandDBFile(const char *filename)
{
    if (!filename) return;

    DataPage *dbHeaderPage = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(filename, dbHeaderPage, PAGE_DB_ROOT_INDEX);
    int32_t index = getNumberOfPages(dbHeaderPage);
    if (index != PAGE_CORRUPT_EXITCODE) updateNumberOfPages(dbHeaderPage, index + 1);
    writeDataPage(filename, dbHeaderPage);
    free(dbHeaderPage);

    if (index != PAGE_CORRUPT_EXITCODE) {
        DataPage *newPage = (DataPage*) malloc(sizeof(DataPage));
        freePage(newPage);
        newPage->header.page_index = index;
        newPage->header.next_related_page = newPage->header.page_index;
        writeDataPage(filename, newPage);
        free(newPage);
    }
}

size_t expandPageThread(const char *filename, size_t page_index)
{
    if (!filename) return PAGE_SEARCH_FAILED;

    uint64_t current = page_index;
    for (uint64_t last = current + 1; current != last;) {
        last = current;
        DataPage *dataPage = (DataPage*) malloc(sizeof(DataPage));
        readDataPage(filename, dataPage, current);
        current = dataPage->header.next_related_page;
        free(dataPage);
    }

    size_t thread_tail_index = current, new_page_index = findFreePageOrExpand(filename, PAGE_DB_ROOT_INDEX);
    u_int8_t flags;

    if (new_page_index == PAGE_SEARCH_FAILED) return PAGE_SEARCH_FAILED;

    DataPage *dataPage = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(filename, dataPage, thread_tail_index);
    dataPage->header.page_index = thread_tail_index;
    dataPage->header.next_related_page = new_page_index;
    flags = dataPage->header.flags;
    writeDataPage(filename, dataPage);
    free(dataPage);

    DataPage *newDataPage = (DataPage*) malloc(sizeof(DataPage));
    freePage(newDataPage);
    newDataPage->header.flags = flags;
    newDataPage->header.page_index = new_page_index;
    newDataPage->header.next_related_page = newDataPage->header.page_index;
    newDataPage->header.data_size = 0;
    writeDataPage(filename, newDataPage);
    free(newDataPage);

    return new_page_index;
}

size_t findFreePageOrExpand(const char *filename, size_t starting_point)
{
    if (!filename) return PAGE_SEARCH_FAILED;

    DataPage *dbHeaderPage = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(filename, dbHeaderPage, PAGE_DB_ROOT_INDEX);
    size_t max_index = getNumberOfPages(dbHeaderPage), found = 0;
    free(dbHeaderPage);

    for (size_t current = starting_point; current < max_index && !found; current++) {
        DataPage *dataPage = (DataPage *) malloc(sizeof(DataPage));
        readDataPage(filename, dataPage, current);
        if (getPageStatus(dataPage) == PAGE_STATUS_INACTIVE) found = current;
        free(dataPage);
    }

    if (found) return found;

    expandDBFile(filename);
    return max_index;
}

void readDataPage(const char *filename, DataPage *dataPage, size_t page_index)
{
    if (!filename || !dataPage) return;

    FILE *file = fopen(filename, "rb+");

    if (!file) return;

    fseek(file, PAGE_SIZE * page_index, SEEK_SET);
    fread(dataPage, sizeof(DataPage), 1, file);
    fclose(file);
}

void writeDataPage(const char *filename, DataPage *dataPage)
{
    if (!filename || !dataPage) return;

    FILE *file = fopen(filename, "rb+");

    if (!file) return;

    fseek(file, PAGE_SIZE * dataPage->header.page_index, SEEK_SET);
    fwrite(dataPage, sizeof(DataPage), 1, file);
    fclose(file);
}

void updatePageMetadata(DataPage *dataPage, const char *metadata)
{
    if (!dataPage || !metadata) return;
    memset(dataPage->header.metadata,'\0',PAGE_METADATA_SIZE);
    memcpy(dataPage->header.metadata, metadata, min_size(strlen(metadata), PAGE_METADATA_SIZE - 1));
}

void updatePageData(DataPage *dataPage, const char *data)
{
    if (!dataPage || !data) return;
    memset(dataPage->page_data,'\0',PAGE_DATA_SIZE);
    memcpy(dataPage->page_data, data, min_size(strlen(data), PAGE_DATA_SIZE - 1));
    dataPage->header.data_size = min_size(strlen(data) + 1, PAGE_DATA_SIZE);
}

void updateHeaderPageMetadata(DataPage *dataPage, const char *metadata)
{
    if (!dataPage || !metadata) return;
    memset(dataPage->header.metadata + sizeof(u_int32_t) * 2, '\0', PAGE_E_HEADER_MD_SIZE);
    memcpy(dataPage->header.metadata + sizeof(u_int32_t) * 2, metadata, min_size(strlen(metadata), PAGE_E_HEADER_MD_SIZE - 1));
}

void updateNumberOfPages(DataPage *dataPage, u_int32_t num)
{
    if (!dataPage) return;
    *(u_int32_t*)(dataPage->header.metadata) = num;
}

void updateNumberOfTables(DataPage *dataPage, u_int32_t num)
{
    if (!dataPage) return;
    *(u_int32_t*)(dataPage->header.metadata + sizeof(u_int32_t)) = num;
}

void updateTableLength(DataPage *dataPage, u_int64_t num)
{
    if (!dataPage) return;
    *(u_int64_t*)(dataPage->header.metadata) = num;
}

char *getPageMetadata(DataPage *dataPage)
{
    if (!dataPage) return NULL;
    return &*(dataPage->header.metadata);
}

char *getPageData(DataPage *dataPage)
{
    if (!dataPage) return NULL;
    return &*(dataPage->page_data);
}

char *getHeaderPageMetadata(DataPage *dataPage)
{
    if (!dataPage) return NULL;
    return &*(dataPage->header.metadata + sizeof(u_int32_t) * 2);
}

int32_t getNumberOfPages(DataPage *dataPage)
{
    if (!dataPage) return PAGE_CORRUPT_EXITCODE;
    return *(int32_t*)(dataPage->header.metadata);
}

int32_t getNumberOfTables(DataPage *dataPage)
{
    if (!dataPage) return PAGE_CORRUPT_EXITCODE;
    return *(int32_t*)(dataPage->header.metadata + sizeof(u_int32_t));
}

int64_t getTableLength(DataPage *dataPage)
{
    if (!dataPage) return PAGE_CORRUPT_EXITCODE;
    return *(int64_t*)(dataPage->header.metadata);
}

int getPageType(DataPage *dataPage)
{
    if (!dataPage) return PAGE_CORRUPT_EXITCODE;
    return dataPage->header.flags & PAGE_TYPE_MASK;
}

int getPageStatus(DataPage *dataPage)
{
    if (!dataPage) return PAGE_CORRUPT_EXITCODE;
    return dataPage->header.flags & PAGE_STATUS_MASK;
}

void updatePageType(DataPage *dataPage, u_int8_t new_type)
{
    if (!dataPage) return;
    dataPage->header.flags &= ~PAGE_TYPE_MASK;
    dataPage->header.flags |= (new_type & PAGE_TYPE_MASK);
}

void updatePageStatus(DataPage *dataPage, u_int8_t new_status)
{
    if (!dataPage) return;
    dataPage->header.flags &= ~PAGE_STATUS_MASK;
    dataPage->header.flags |= (new_status & PAGE_STATUS_MASK);
}

void appendData(DataPage *dataPage, const char *line)
{
    if (!dataPage || !line) return;
    if (dataPage->header.data_size + strlen(line) + 1 > PAGE_DATA_SIZE) return;
    if (dataPage->header.data_size == 0) updatePageData(dataPage, line);
    else {
        memcpy(getPageData(dataPage) + dataPage->header.data_size, line, strlen(line) + 1);
        dataPage->header.data_size += (strlen(line) + 1);
    }
}

void appendDataOrExpandThread(const char *filename, size_t page_index, const char *line)
{
    if (!line || strlen(line) >= PAGE_DATA_SIZE) return;

    DataPage *dbHeader = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(filename, dbHeader, PAGE_DB_ROOT_INDEX);
    size_t max_index = getNumberOfPages(dbHeader);
    free(dbHeader);

    ssize_t found = SEARCH_PAGE_NOT_FOUND;
    size_t current = page_index;
    for (size_t last = current + 1; current < max_index && (found == SEARCH_PAGE_NOT_FOUND) && current != last;) {
        last = current;
        DataPage *dataPage = (DataPage*) malloc(sizeof(DataPage));
        readDataPage(filename, dataPage, current);
        if (dataPage->header.data_size + strlen(line) + 1 <= PAGE_DATA_SIZE) found = (ssize_t) current;
        current = dataPage->header.next_related_page;
        free(dataPage);
    }

    if (found != SEARCH_PAGE_NOT_FOUND) {
        DataPage *dataPage = (DataPage*) malloc(sizeof(DataPage));
        readDataPage(filename, dataPage, found);
        appendData(dataPage, line);
        writeDataPage(filename, dataPage);
        free(dataPage);
    } else {
        size_t new_page = findFreePageOrExpand(filename, PAGE_DB_ROOT_INDEX);
        int flags;
        if (new_page == PAGE_SEARCH_FAILED) return;

        DataPage *dataPage = (DataPage*) malloc(sizeof(DataPage));
        readDataPage(filename, dataPage, current);
        dataPage->header.next_related_page = new_page;
        flags = dataPage->header.flags;
        writeDataPage(filename, dataPage);
        free(dataPage);

        DataPage *newPage = (DataPage*) malloc(sizeof(DataPage));
        readDataPage(filename, newPage, new_page);
        newPage->header.flags = flags;
        newPage->header.page_index = new_page;
        newPage->header.next_related_page = newPage->header.page_index;
        writeDataPage(filename, newPage);
        free(newPage);
    }
}

size_t findTableOnPage(DataPage *dataPage, const char *table_name, size_t *checked) {
    if (!dataPage || !table_name) return PAGE_SEARCH_FAILED;

    size_t index = 0, parsed = 0, found = 0;
    while (dataPage->page_data[index] == '\0' && index < PAGE_DATA_SIZE) index++;

    while (!found && index < PAGE_DATA_SIZE) {
        TableLink *tableLink = parseTableLinkJSON(dataPage->page_data + index, 0, &parsed);
        index += parsed;
        if (tableLink != NULL && strcmp(table_name, tableLink->table_name) == 0) found = tableLink->link;
        destroyTableLink(tableLink);
        while (dataPage->page_data[index] == '\0' && index < PAGE_DATA_SIZE) index++;
        (*checked)++;
    }

    if (!found) return PAGE_SEARCH_FAILED;

    return found;
}

size_t findTable(const char *filename, const char *table_name)
{
    if (!filename || !table_name) return PAGE_SEARCH_FAILED;

    DataPage *dbHeader = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(filename, dbHeader, PAGE_DB_ROOT_INDEX);
    int32_t num_of_tables = getNumberOfTables(dbHeader);
    int32_t num_of_pages = getNumberOfPages(dbHeader);
    free(dbHeader);

    if (num_of_tables == PAGE_CORRUPT_EXITCODE || num_of_pages == PAGE_CORRUPT_EXITCODE) return PAGE_SEARCH_FAILED;

    size_t found = 0, iterated = 0;
    for (uint64_t current = PAGE_DB_ROOT_INDEX, last = current + 1; current != last && iterated < num_of_tables && !found;) {
        last = current;
        DataPage *dataPage = (DataPage*) malloc(sizeof(DataPage));
        readDataPage(filename, dataPage, current);
        current = dataPage->header.next_related_page;
        found = findTableOnPage(dataPage, table_name, &iterated);
        free(dataPage);
    }

    if (!found) return PAGE_SEARCH_FAILED;

    return found;
}

void addTableHeader(const char *filename, Table *table)
{
    if (!filename || !table || !table->tableSchema) return;
    size_t search_result = findTable(filename, table->table_name);
    if (search_result != SEARCH_TABLE_NOT_FOUND) return;

    size_t table_header = findFreePageOrExpand(filename, PAGE_DB_ROOT_INDEX);
    if (table_header == PAGE_SEARCH_FAILED) return;

    DataPage *dbHeader = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(filename, dbHeader, PAGE_DB_ROOT_INDEX);
    updateNumberOfTables(dbHeader, getNumberOfTables(dbHeader) + 1);
    writeDataPage(filename, dbHeader);
    free(dbHeader);

    TableLink *tableLink = createTableLink(table->table_name, table_header);
    char *table_link = transformTableLinkToJSON(tableLink);
    destroyTableLink(tableLink);
    appendDataOrExpandThread(filename, PAGE_DB_ROOT_INDEX, table_link);
    free(table_link);

    DataPage *tableHeader = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(filename, tableHeader, table_header);
    freePage(tableHeader);
    char *table_schema = transformTableSchemaToJSON(table->tableSchema);
    updatePageStatus(tableHeader, PAGE_STATUS_ACTIVE);
    updatePageType(tableHeader, PAGE_TYPE_TABLE_HEADER);
    updateTableLength(tableHeader, table->length);
    updateHeaderPageMetadata(tableHeader, table->table_name);
    updatePageData(tableHeader, table_schema);
    tableHeader->header.data_size = PAGE_DATA_SIZE;
    writeDataPage(filename, tableHeader);
    free(table_schema);
    free(tableHeader);

    size_t table_data = findFreePageOrExpand(filename, PAGE_DB_ROOT_INDEX);
    if (table_data == PAGE_SEARCH_FAILED) return;

    DataPage *tableData = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(filename, tableData, table_header);
    freePage(tableData);
    updatePageStatus(tableData, PAGE_STATUS_ACTIVE);
    updatePageType(tableData, PAGE_TYPE_TABLE_DATA);
    tableData->header.page_index = table_data;
    tableData->header.next_related_page = tableData->header.page_index;
    writeDataPage(filename, tableData);
    free(tableData);

    for (TableRecord *tableRecord = table->firstTableRecord; tableRecord != NULL; tableRecord = tableRecord->next_record) {
        char *table_record = transformTableRecordToJSON(tableRecord);
        appendDataOrExpandThread(filename, table_data, table_record);
        free(table_record);
    }

    tableHeader = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(filename, tableHeader, table_header);
    tableHeader->header.next_related_page = table_data;
    writeDataPage(filename, tableHeader);
    free(tableHeader);
}

void insertTableRecords(const char *filename, Table *table)
{
    if (!filename || !table || !table->tableSchema || table->length == 0) return;
    size_t search_result = findTable(filename, table->table_name);
    if (search_result == SEARCH_TABLE_NOT_FOUND) return;

    DataPage *tableHeader = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(filename, tableHeader, search_result);
    updateTableLength(tableHeader, getTableLength(tableHeader) + table->length);
    writeDataPage(filename, tableHeader);
    free(tableHeader);

    for (TableRecord *tableRecord = table->firstTableRecord; tableRecord != NULL; tableRecord = tableRecord->next_record) {
        char *table_record = transformTableRecordToJSON(tableRecord);
        appendDataOrExpandThread(filename, search_result, table_record);
        free(table_record);
    }
}

size_t findAndErase(DataPage *dataPage, const char *table_name, size_t *checked)
{
    if (!dataPage || !table_name) return PAGE_SEARCH_FAILED;

    size_t index = 0, parsed = 0, found = 0, stop = 0;
    while (dataPage->page_data[index] == '\0' && index < PAGE_DATA_SIZE) index++;

    while (!stop && index < PAGE_DATA_SIZE) {
        TableLink *tableLink = parseTableLinkJSON(dataPage->page_data + index, 0, &parsed);
        if (tableLink != NULL && strcmp(table_name, tableLink->table_name) == 0) {
            found = index;
            stop = 1;
        }
        index += parsed;
        destroyTableLink(tableLink);
        while (dataPage->page_data[index] == '\0' && index < PAGE_DATA_SIZE) index++;
        (*checked)++;
    }

    memset(dataPage->page_data + found, '\0', parsed);

    if (!found) return PAGE_SEARCH_FAILED;

    return found;
}

void deleteTable(const char *filename, const char *table_name)
{
    if (!filename || !table_name) return;
    size_t search_result = findTable(filename, table_name);
    if (search_result == SEARCH_TABLE_NOT_FOUND) return;
    freePageThread(filename, search_result);

    DataPage *dbHeader = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(filename, dbHeader, PAGE_DB_ROOT_INDEX);
    int32_t num_of_tables = getNumberOfTables(dbHeader);
    int32_t num_of_pages = getNumberOfPages(dbHeader);
    updateNumberOfTables(dbHeader, num_of_tables - 1);
    writeDataPage(filename, dbHeader);
    free(dbHeader);

    if (num_of_tables == PAGE_CORRUPT_EXITCODE || num_of_pages == PAGE_CORRUPT_EXITCODE) return;

    size_t found = 0, iterated = 0;
    for (uint64_t current = PAGE_DB_ROOT_INDEX, last = current + 1; current != last && iterated < num_of_tables && !found;) {
        last = current;
        DataPage *dataPage = (DataPage*) malloc(sizeof(DataPage));
        readDataPage(filename, dataPage, current);
        current = dataPage->header.next_related_page;
        found = findAndErase(dataPage, table_name, &iterated);
        writeDataPage(filename, dataPage);
        free(dataPage);
    }
}