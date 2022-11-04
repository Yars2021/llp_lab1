//
// Created by yars on 17.10.22.
//

#ifndef LLP_LAB1_C_DB_FILE_MANAGER_H
#define LLP_LAB1_C_DB_FILE_MANAGER_H

#include <stdio.h>
#include <string.h>
#include "db_internals.h"

#define PAGE_SIZE 4096
#define PAGE_HEADER_SIZE 256
#define PAGE_METADATA_SIZE 243

typedef struct {
    u_int8_t flags;
    u_int64_t page_number;
    u_int16_t data_size;
    char metadata[PAGE_METADATA_SIZE];
} DataPageHeader;

typedef struct {
    DataPageHeader header;
    char page_data[PAGE_SIZE - PAGE_HEADER_SIZE];
} DataPage;


void readDataPage(const char *filename, DataPage *dataPage, size_t page_number);

void writeDataPage(const char *filename, DataPage *dataPage);

void updatePageMetadata(DataPage *dataPage, const char *metadata);

void updatePageData(DataPage *dataPage, const char *data);

char *getPageMetadata(DataPage *dataPage);

char *getPageData(DataPage *dataPage);

#endif //LLP_LAB1_C_DB_FILE_MANAGER_H