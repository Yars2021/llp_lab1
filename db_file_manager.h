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
#define PAGE_METADATA_SIZE 189

/*
 * Each PageHeader contains flags (1 byte), pointer to the next page (64 bytes), data size (2 bytes) and metadata (189 bytes).
 *
 * The page can be 1 of 2 types depending on flags:
 *      1) DataPage (00000000).
 *      It contains TableRecords in its data part.
 *      There is a pointer to the next page of this table (64 bytes) and the number of records it contains (2 bytes).
 *      The rest is the name of the table (123 bytes).
 *
 *      2) MetadataPage (00000001).
 *      It contains TableSchemas, lengths of the tables and pointers to their DataPages.
 *      There is a pointer to the next metadata page (64 bytes) and the number of table headers it contains (2 bytes).
 *      The rest is a buffer, which can be used to store some additional information (123 bytes).
 */

/// Page header, contains flags, pointer to the next page, size of its data and the rest is metadata.
typedef struct {
    char flags[1];
    char next_addr[64];
    char data_size[2];
    char metadata[PAGE_METADATA_SIZE];
} PageHeader;

/// PageHeader extension. Contains additional information such as next data page pointer,
/// number of TableRecords on this page and table name in metadata field.
typedef struct {
    char next_d_page[64];
    char number_of_records[2];
    char table_name[PAGE_METADATA_SIZE - 66];
} DataPageHeader;

/// PageHeader extension. Contains additional information such as next metadata page pointer,
/// number of TableSchemas and table pointers on this page and additional information in metadata field.
typedef struct {
    char next_m_page[64];
    char number_of_schemas[2];
    char buffer[PAGE_METADATA_SIZE - 66];
} MetadataPageHeader;

/// Data page, consists of a header (256 bytes) and data (2840 bytes).
/// The data is described in the header.
typedef struct {
    PageHeader *pageHeader;
    char data[PAGE_SIZE - PAGE_HEADER_SIZE];
} DataPage;


/// Creates an instance of a PageHeader.
PageHeader *createPageHeader();

/// Destroys the PageHeader.
void destroyPageHeader(DataPageHeader *pageHeader);


/// Creates an instance of a DataPageHeader.
DataPageHeader *createDataPageHeader();

/// Destroys the DataPageHeader.
void destroyDataPageHeader(PageHeader *dataPageHeader);


/// Creates an instance of a MetadataPageHeader.
MetadataPageHeader *createMetadataPageHeader();

/// Destroys the MetadataPageHeader.
void destroyMetadataPageHeader(MetadataPageHeader *metadataPageHeader);


/// Creates an empty DataPage struct.
DataPage *createDataPage();

/// Clears an existing DataPage struct.
void clearDataPage(DataPage *dataPage);

/// Reads the raw data and maps it into the DataPage struct.
void mapDataPage(DataPage *dataPage, char raw_data[PAGE_SIZE]);

/// Returns DataPages header.
PageHeader *getPageHeader(DataPage *dataPage);

/// Interprets the PageHeader as a DataPageHeader, returning a pointer to new a struct.
DataPageHeader *getDataPageHeader(PageHeader *pageHeader);

/// Interprets the PageHeader as a MetadataPageHeader, returning a pointer to new a struct.
MetadataPageHeader *getMetadataPageHeader(PageHeader *pageHeader);

/// Updates the DataPages header.
void replaceDataPageHeader(DataPage *dataPage, DataPageHeader *dataPageHeader);

/// Updates the DataPages header.
void replaceMetadataPageHeader(DataPage *dataPage, MetadataPageHeader *metadataPageHeader);

/// Returns the serialized version of the DataPage.
char *serialize(DataPage *dataPage);

/// Destroys the DataPage struct.
void destroyDataPage(DataPage *dataPage);

#endif //LLP_LAB1_C_DB_FILE_MANAGER_H