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
    memset(dataPage->header.metadata,'\0',PAGE_METADATA_SIZE + 1);
    memset(dataPage->page_data,'\0',PAGE_DATA_SIZE + 1);
}

void freePageThread(char *filename, size_t page_index)
{
    uint64_t current = page_index, last = current + 1;
    while (current != last) {
        last = current;
        DataPage *dataPage = (DataPage *) malloc(sizeof(DataPage));
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
    dbHeaderPage->header.page_index = 0;
    dbHeaderPage->header.next_related_page = 0;
    updatePageType(dbHeaderPage, PAGE_TYPE_DATABASE_HEADER);
    updatePageStatus(dbHeaderPage, PAGE_STATUS_ACTIVE);
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
        newPage->header.flags = 0;
        newPage->header.page_index = index;
        newPage->header.next_related_page = newPage->header.page_index;
        newPage->header.data_size = 0;
        updatePageStatus(newPage, PAGE_STATUS_INACTIVE);
        writeDataPage(filename, newPage);
        free(newPage);
    }
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
    memset(dataPage->header.metadata,'\0',PAGE_METADATA_SIZE + 1);
    memcpy(dataPage->header.metadata, metadata, min_size(strlen(metadata), PAGE_METADATA_SIZE));
}

void updatePageData(DataPage *dataPage, const char *data)
{
    if (!dataPage || !data) return;
    memset(dataPage->page_data,'\0',PAGE_DATA_SIZE + 1);
    memcpy(dataPage->page_data, data, min_size(strlen(data), PAGE_DATA_SIZE));
    dataPage->header.data_size = min_size(strlen(data), PAGE_DATA_SIZE);
}

void updateHeaderPageMetadata(DataPage *dataPage, const char *metadata)
{
    if (!dataPage || !metadata) return;
    memset(dataPage->header.metadata + sizeof(u_int32_t) * 2, '\0', PAGE_E_HEADER_MD_SIZE + 1);
    memcpy(dataPage->header.metadata + sizeof(u_int32_t) * 2, metadata, min_size(strlen(metadata), PAGE_E_HEADER_MD_SIZE));
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
    if (dataPage->header.data_size + 1 + strlen(line) > PAGE_DATA_SIZE) return;
    if (dataPage->header.data_size == 0) updatePageData(dataPage, line);
    else {
        memcpy(getPageData(dataPage) + dataPage->header.data_size + 1, line, strlen(line) + 1);
        dataPage->header.data_size += (strlen(line) + 1);
    }
}

void addTableHeader(const char *filename, Table *table)
{
    if (!filename || !table || !table->tableSchema) return;
    size_t page_index = findFreePageOrExpand(filename, PAGE_DB_ROOT_INDEX);
    if (page_index == PAGE_SEARCH_FAILED) return;

    DataPage *foundPage = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(filename, foundPage, page_index);
    updateTableLength(foundPage, table->length);
    updateHeaderPageMetadata(foundPage, table->table_name);
    updatePageType(foundPage, PAGE_TYPE_TABLE_HEADER);
    updatePageStatus(foundPage, PAGE_STATUS_ACTIVE);

    char *serialized_schema = transformTableSchemaToJSON(table->tableSchema);
    if (strlen(serialized_schema) <= PAGE_DATA_SIZE) updatePageData(foundPage, serialized_schema);
    free(serialized_schema);

    writeDataPage(filename, foundPage);
    free(foundPage);

    DataPage *dbHeaderPage = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(filename, dbHeaderPage, PAGE_DB_ROOT_INDEX);
    // ToDo add TableLink insertion
    // ToDo add Page Thread writing
    free(dbHeaderPage);
}

size_t findTablePage(const char *filename, const char *table_name)
{
    // ToDo Rewrite using the info from the root page
    if (!filename || !table_name) return PAGE_SEARCH_FAILED;
    DataPage *dbHeaderPage = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(filename, dbHeaderPage, PAGE_DB_ROOT_INDEX);
    size_t max_index = getNumberOfPages(dbHeaderPage), found = 0;
    free(dbHeaderPage);

    for (size_t current = PAGE_DB_ROOT_INDEX; current < max_index && !found; current++) {
        DataPage *foundPage = (DataPage*) malloc(sizeof(DataPage));
        readDataPage(filename, foundPage, current);

        if (getPageType(foundPage) == PAGE_TYPE_TABLE_HEADER && getPageStatus(foundPage) == PAGE_STATUS_ACTIVE)
            if (strcmp(getHeaderPageMetadata(foundPage), table_name) == 0)
                found = current;

        free(foundPage);
    }

    if (!found) return SEARCH_TABLE_NOT_FOUND;

    return found;
}