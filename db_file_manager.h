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
#define PAGE_METADATA_SIZE 235

#define PAGE_CORRUPT_EXITCODE       (-1)
#define PAGE_TYPE_TABLE_DATA        0b00000000
#define PAGE_TYPE_TABLE_HEADER      0b00000001
#define PAGE_TYPE_DATABASE_HEADER   0b00000010
#define PAGE_STATUS_ACTIVE          0b00000000
#define PAGE_STATUS_INACTIVE        0b10000000

/*
 * DataPage flags:
 *
 * Counting from lowest to highest bits:
 * 0 and 1 bits - page type
 * 7 bit - page status
 *
 *
 * Page types (M - the field is stored in the metadata):
 *
 * Flags    | Name               | Description
 * ---------------------------------------------------------------------------
 * 00       | TableDataPage      | Table data page, contains records.
 * ---------------------------------------------------------------------------
 * 01       | TableHeaderPage    | Table header page, contains table name (M),
 *          |                    | schema, length and data page indexes.
 * ---------------------------------------------------------------------------
 * 10       | DatabaseHeaderPage | Database header page, contains db name (M),
 *          |                    | number of tables and first page
 *          |                    | indexes for every table.
 */

/// Header of the DataPage. Contains flags (1 byte), page_index (8 bytes), next_related_page (8 bytes),
/// data_size (2 bytes) and metadata (235 bytes).
typedef struct {
    u_int8_t flags;
    u_int64_t page_index;
    u_int64_t next_related_page;
    u_int16_t data_size;
    char metadata[PAGE_METADATA_SIZE];
} DataPageHeader;

/// DataPage. Its size is 4096 bytes, 256 first of which store the header data.
typedef struct {
    DataPageHeader header;
    char page_data[PAGE_SIZE - PAGE_HEADER_SIZE];
} DataPage;

/// Extracts a DataPage by its number from a file and puts it into a struct.
void readDataPage(const char *filename, DataPage *dataPage, size_t page_number);

/// Writes a DataPage struct into a file.
void writeDataPage(const char *filename, DataPage *dataPage);

/// Updates the metadata field value in the DataPages header.
void updatePageMetadata(DataPage *dataPage, const char *metadata);

/// Updates the data of the DataPage.
void updatePageData(DataPage *dataPage, const char *data);

/// Returns a char pointer to the DataPages metadata.
char *getPageMetadata(DataPage *dataPage);

/// Returns a char pointer to the DataPages data.
char *getPageData(DataPage *dataPage);

/// Returns the type of the DataPage or PAGE_CORRUPT_EXITCODE.
int getPageType(DataPage *dataPage);

/// Returns the status of the DataPage or PAGE_CORRUPT_EXITCODE.
int getPageStatus(DataPage *dataPage);

/// Updates the DataPages type.
void updatePageType(DataPage *dataPage, u_int8_t new_type);

/// Updates the DataPages status.
void updatePageStatus(DataPage *dataPage, u_int8_t new_status);

#endif //LLP_LAB1_C_DB_FILE_MANAGER_H