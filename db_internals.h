//
// Created by yars on 30.09.22.
//

#ifndef LLP_LAB1_C_DB_INTERNALS_H
#define LLP_LAB1_C_DB_INTERNALS_H

#include <stdlib.h>
#include <string.h>

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

/// Updates a column name
void updateTableSchemaColumnName(TableSchema *tableSchema, size_t column_index, char *field_name);

/// Updates a column type
void updateTableSchemaColumnType(TableSchema *tableSchema, size_t column_index, FieldType fieldType);

/// Returns a valid JSON string containing the schema data
char *transformTableSchemaToJSON(TableSchema *tableSchema);

/// Destroys the schema instance
void destroyTableSchema(TableSchema *tableSchema);


/// Describes a cell (field and it's value)
typedef struct DataCell
{
    Field *field;
    char *value;
} DataCell;

/// Creates a new data cell
DataCell *createDataCell(Field *field, char *value);

/// Destroys the data cell instance
void destroyDataCell(DataCell *dataCell);


/// Creates a table record, which is an array of DataCell pointers.
/// Each record is linked to the next and previous ones, which helps to iterate through them
typedef struct TableRecord
{
    size_t length;
    struct TableRecord *next_record;
    struct TableRecord *prev_record;
    DataCell **dataCells;
} TableRecord;

/// Creates an instance of a table record (next is NULL by default)
TableRecord *createTableRecord(size_t length, DataCell **dataCells);

/// Links the record to the next one
void linkToTheNextTableRecord(TableRecord *tableRecord);

/// Links the record to the previous one
void linkToThePrevTableRecord(TableRecord *tableRecord);

/// Returns the next record
TableRecord *getNextTableRecord(TableRecord *tableRecord);

/// Returns the previous record
TableRecord *getPrevTableRecord(TableRecord *tableRecord);

/// Returns an array of data cells, which are contained in the record
DataCell **getDataCells(TableRecord *tableRecord);

/// Updates a cell in the record
void updateTableRecord(TableRecord *tableRecord, size_t cell_index, DataCell *dataCell);

/// Returns a valid JSON string containing the record data
char *transformTableRecordToJSON(TableRecord *tableRecord);

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

/// Updates the table name
void updateTableName(Table *table, char *new_table_name);

/// Adds a new line to the table
void insertTableRecord(Table *table, TableRecord *tableRecord);

/// Returns the length of the table
size_t getTableLength(Table *table);

/// Returns the record list head
TableRecord *getFirstRecordFromTable(Table *table);

/// Returns a valid JSON string containing the table
char *transformTableToJSON(Table *table);

/// Creates a table from a JSON line
Table *parseTableJSON(const char *line);

/// Destroys the table instance
void destroyTable(Table *table);

#endif //LLP_LAB1_C_DB_INTERNALS_H
