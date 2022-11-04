//
// Created by yars on 17.10.22.
//

#include "db_file_manager.h"

size_t min_size(size_t a, size_t b)
{
    return a < b ? a : b;
}

void readDataPage(const char *filename, DataPage *dataPage, size_t page_number)
{
    if (!filename || !dataPage) return;

    FILE *file = fopen(filename, "rb+");

    if (!file) return;

    fseek(file, PAGE_SIZE * page_number, SEEK_SET);
    fread(dataPage, sizeof(DataPage), 1, file);

    fclose(file);
}

void writeDataPage(const char *filename, DataPage *dataPage)
{
    if (!filename || !dataPage) return;

    FILE *file = fopen(filename, "rb+");

    if (!file) return;

    fseek(file, PAGE_SIZE * dataPage->header.page_number, SEEK_SET);
    fwrite(dataPage, sizeof(DataPage), 1, file);

    fclose(file);
}

void updatePageMetadata(DataPage *dataPage, const char *metadata)
{
    if (!dataPage || !metadata) return;
    memset(dataPage->header.metadata,'\0',PAGE_METADATA_SIZE - 1);
    memcpy(dataPage->header.metadata, metadata, min_size(strlen(metadata), PAGE_METADATA_SIZE - 1));
}

void updatePageData(DataPage *dataPage, const char *data)
{
    if (!dataPage || !data) return;
    memset(dataPage->page_data,'\0',PAGE_SIZE - PAGE_HEADER_SIZE - 1);
    memcpy(dataPage->page_data, data, min_size(strlen(data), PAGE_SIZE - PAGE_HEADER_SIZE - 1));
    dataPage->header.data_size = min_size(strlen(data), PAGE_SIZE - PAGE_HEADER_SIZE - 1) + 1;
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