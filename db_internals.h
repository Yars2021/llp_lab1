//
// Created by yars on 30.09.22.
//

#ifndef LLP_LAB1_C_DB_INTERNALS_H
#define LLP_LAB1_C_DB_INTERNALS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define JSON_FIELD_NAME "F_NAME"
#define JSON_FIELD_TYPE "F_TYPE"
#define JSON_SCHEMA_KEY_I "KEY_COL_I"
#define JSON_SCHEMA_NUM_OF_FIELDS "NUM_OF_FIELDS"
#define JSON_SCHEMA_FIELDS "FIELDS"
#define JSON_RECORD "RECORD"
#define JSON_TABLE_NAME "TABLE_NAME"
#define JSON_TABLE_SIZE "TABLE_SIZE"
#define JSON_TABLE_SCHEMA "TABLE_SCHEMA"
#define JSON_TABLE_DATA "TABLE_DATA"

char *uint_to_str(unsigned int num);

/// Types of fields in a table
typedef enum FieldType
{
    INTEGER,
    FLOAT,
    STRING,
    BOOLEAN
} FieldType;


/// Table field
typedef struct Field
{
    char *field_name;
    FieldType fieldType;
} Field;

/// Creates a new instance of a field
Field *createField(char *field_name, FieldType fieldType);

/// Returns a valid JSON string containing the field data
char *transformFieldToJSON(Field *field);

/// Destroys the field instance
void destroyField(Field *field);


/// Describes the table structure (column types and names as an array of Field pointers and the index of the ID column)
typedef struct TableSchema
{
    size_t key_column_index;
    size_t number_of_fields;
    Field **fields;
} TableSchema;

/// Creates a new instance of a schema
TableSchema *createTableSchema(Field **fields, size_t number_of_fields, size_t key_column_index);

/// Returns a valid JSON string containing the schema data
char *transformTableSchemaToJSON(TableSchema *tableSchema);

/// Destroys the schema instance
void destroyTableSchema(TableSchema *tableSchema);


/// Creates a table record, which is an array of string pointers.
/// Each record is linked to the next and previous ones, which helps to iterate through them
typedef struct TableRecord
{
    size_t length;
    struct TableRecord *next_record;
    struct TableRecord *prev_record;
    char **dataCells;
} TableRecord;

/// Creates a data cell from a string
char *createDataCell(const char *value);

/// Creates an instance of a table record (next is NULL by default)
TableRecord *createTableRecord(size_t length, char **dataCells);

/// Returns a valid JSON string containing the record data
char *transformTableRecordToJSON(TableRecord *tableRecord);

/// Creates a table record from a JSON line
TableRecord *parseTableRecordJSON(const char *line, size_t pos, TableSchema *tableSchema, size_t *new_index);

/// destroys the instance of table record
void destroyTableRecord(TableRecord *tableRecord);


/// Table that is described by a schema and has a name.
/// Points to all the first of the records inside it (2-way linked list)
typedef struct Table
{
    char *table_name;
    TableSchema *tableSchema;
    size_t length;
    TableRecord *firstTableRecord;
} Table;

/// Creates a new table
Table *createTable(TableSchema *tableSchema, char *table_name);

/// Adds a new line to the table
void insertTableRecord(Table *table, TableRecord *tableRecord);

/// Returns a valid JSON string containing the table
char *transformTableToJSON(Table *table);

/// Creates a table header from a JSON line
Table *parseTableHeaderJSON(const char *line, size_t pos, size_t *new_index);

/// Destroys the table instance
void destroyTable(Table *table);

#endif //LLP_LAB1_C_DB_INTERNALS_H