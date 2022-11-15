//
// Created by yars on 17.10.22.
//

#include "db_file_manager.h"

size_t min_size(size_t a, size_t b)
{
    return a < b ? a : b;
}

void clearPage(DataPage *dataPage)
{
    if (!dataPage) return;
    dataPage->header.flags = 0;
    dataPage->header.next_related_page = dataPage->header.page_index;
    dataPage->header.data_size = 0;
    memset(dataPage->header.metadata,'\0',PAGE_METADATA_SIZE + 1);
    memset(dataPage->page_data,'\0',PAGE_DATA_SIZE + 1);
}

void clearPageThread(char *filename, size_t page_index)
{
    uint64_t current = page_index, last = current + 1;
    while (current != last) {
        last = current;
        DataPage *dataPage = (DataPage *) malloc(sizeof(DataPage));
        readDataPage(filename, dataPage, current);
        current = dataPage->header.next_related_page;
        clearPage(dataPage);
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
    readDataPage(filename, dbHeaderPage, 0);
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
        writeDataPage(filename, newPage);
        free(newPage);
    }
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