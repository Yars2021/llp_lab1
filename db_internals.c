//
// Created by yars on 30.09.22.
//
#include "db_internals.h"

char *uint_to_str(unsigned int num)
{
    size_t length = 0;

    char *res = (char*) malloc(32);

    do {
        res[length] = num % 10 + '0';

        length++;
        num /= 10;
    } while (num > 0);

    char *rev_res = (char*) malloc(length + 1);

    rev_res[length] = '\0';

    for (size_t i = 0; i < length; i++) {
        rev_res[i] = res[length - i - 1];
    }

    free(res);

    return rev_res;
}

Field *createField(char *field_name, FieldType fieldType)
{
    Field *field = (Field*) malloc(sizeof(Field));

    field->field_name = field_name;
    field->fieldType = fieldType;

    return field;
}

char *transformFieldToJSON(Field *field)
{
    /// {"F_NAME":"field->name","F_TYPE":"field->type"}

    if (!field) {
        return "";
    } else {
        size_t index = 0;

        char *line = (char*) malloc(strlen("{'':'','':''}") + strlen(JSON_FIELD_NAME) + strlen(JSON_FIELD_TYPE) + strlen(field->field_name) + 2);

        line[index] = '{';
        line[index + 1] = '"';

        index += 2;

        for (size_t i = 0; i < strlen(JSON_FIELD_NAME); i++, index++) {
            line[index] = JSON_FIELD_NAME[i];
        }

        line[index] = '"';
        line[index + 1] = ':';
        line[index + 2] = '"';

        index += 3;

        for (size_t i = 0; i < strlen(field->field_name); i++, index++) {
            line[index] = field->field_name[i];
        }

        line[index] = '"';
        line[index + 1] = ',';
        line[index + 2] = '"';

        index += 3;

        for (size_t i = 0; i < strlen(JSON_FIELD_TYPE); i++, index++) {
            line[index] = JSON_FIELD_TYPE[i];
        }

        line[index] = '"';
        line[index + 1] = ':';
        line[index + 2] = '"';

        switch (field->fieldType) {
            case INTEGER:
                line[index + 3] = 'I';
                break;

            case FLOAT:
                line[index + 3] = 'F';
                break;

            case STRING:
                line[index + 3] = 'S';
                break;

            case BOOLEAN:
                line[index + 3] = 'B';
                break;
        }

        line[index + 4] = '"';
        line[index + 5] = '}';
        line[index + 6] = '\0';

        return line;
    }
}

void destroyField(Field *field)
{
    if (field) {
        field->field_name = 0;
        field->fieldType = INTEGER;

        free(field);
    }
}


TableSchema *createTableSchema(Field **fields, size_t number_of_fields, size_t key_column_index)
{
    TableSchema *tableSchema = (TableSchema*) malloc(sizeof(TableSchema));

    tableSchema->number_of_fields = number_of_fields;
    tableSchema->key_column_index = key_column_index;
    tableSchema->fields = fields;

    return tableSchema;
}

void updateTableSchemaColumnName(TableSchema *tableSchema, size_t column_index, char *field_name)
{
    if (tableSchema && column_index < tableSchema->number_of_fields) {
        if (tableSchema->fields[column_index]) {
            tableSchema->fields[column_index]->field_name = field_name;
        }
    }
}

void updateTableSchemaColumnType(TableSchema *tableSchema, size_t column_index, FieldType fieldType)
{
    if (tableSchema && column_index < tableSchema->number_of_fields) {
        if (tableSchema->fields[column_index]) {
            tableSchema->fields[column_index]->fieldType = fieldType;
        }
    }
}

char *transformTableSchemaToJSON(TableSchema *tableSchema)
{
    /// {"KEY_COL_I":"","":"NUM_OF_FIELDS":"","FIELDS":[]}

    if (!tableSchema) {
        return "";
    } else {
        size_t index = 0, field_lengths = 0;

        char *num_of_fields = uint_to_str(tableSchema->number_of_fields);
        char *schema_key_i = uint_to_str(tableSchema->key_column_index);

        for (size_t i = 0; i < tableSchema->number_of_fields; i++) {
            char *field_line = transformFieldToJSON(tableSchema->fields[i]);

            field_lengths += strlen(field_line);

            free(field_line);
        }

        char *line = (char*) malloc(strlen("{'':'','':'','':[]}") + strlen(JSON_SCHEMA_KEY_I) + strlen(JSON_SCHEMA_NUM_OF_FIELDS) + strlen(JSON_SCHEMA_FIELDS) +
                                    field_lengths + tableSchema->number_of_fields + strlen(schema_key_i) + strlen(num_of_fields));

        line[index] = '{';
        line[index + 1] = '"';

        index += 2;

        for (size_t i = 0; i < strlen(JSON_SCHEMA_KEY_I); i++, index++) {
            line[index] = JSON_SCHEMA_KEY_I[i];
        }

        line[index] = '"';
        line[index + 1] = ':';
        line[index + 2] = '"';

        index += 3;

        for (size_t i = 0; i < strlen(schema_key_i); i++, index++) {
            line[index] = schema_key_i[i];
        }

        line[index] = '"';
        line[index + 1] = ',';
        line[index + 2] = '"';

        index += 3;

        for (size_t i = 0; i < strlen(JSON_SCHEMA_NUM_OF_FIELDS); i++, index++) {
            line[index] = JSON_SCHEMA_NUM_OF_FIELDS[i];
        }

        line[index] = '"';
        line[index + 1] = ':';
        line[index + 2] = '"';

        index += 3;

        for (size_t i = 0; i < strlen(num_of_fields); i++, index++) {
            line[index] = num_of_fields[i];
        }

        line[index] = '"';
        line[index + 1] = ',';
        line[index + 2] = '"';

        index += 3;

        for (size_t i = 0; i < strlen(JSON_SCHEMA_FIELDS); i++, index++) {
            line[index] = JSON_SCHEMA_FIELDS[i];
        }

        line[index] = '"';
        line[index + 1] = ':';
        line[index + 2] = '[';

        index += 3;

        for (size_t i = 0; i < tableSchema->number_of_fields; i++) {
            char *field = transformFieldToJSON(tableSchema->fields[i]);

            for (size_t j = 0; j < strlen(field); j++, index++) {
                line[index] = field[j];
            }

            if (i < tableSchema->number_of_fields - 1) {
                line[index++] = ',';
            }

            free(field);
        }

        line[index] = ']';
        line[index + 1] = '}';
        line[index + 2] = '\0';

        free(num_of_fields);
        free(schema_key_i);

        return line;
    }
}

void destroyTableSchema(TableSchema *tableSchema)
{
    if (tableSchema) {
        if (tableSchema->fields) {
            for (size_t i = 0; i < tableSchema->number_of_fields; i++) {
                if (tableSchema->fields[i]) {
                    destroyField(tableSchema->fields[i]);
                }
            }

            free(tableSchema->fields);
        }

        tableSchema->fields = NULL;
        tableSchema->number_of_fields = 0;
        tableSchema->key_column_index = 0;

        free(tableSchema);
    }
}


DataCell *createDataCell(Field *field, char *value)
{
    DataCell *dataCell = (DataCell*) malloc(sizeof(DataCell));

    dataCell->field = field;
    dataCell->value = value;

    return dataCell;
}

void destroyDataCell(DataCell *dataCell)
{
    if (dataCell) {
        dataCell->field = NULL;
        dataCell->value = 0;

        free(dataCell);
    }
}


TableRecord *createTableRecord(size_t length, DataCell **dataCells)
{
    TableRecord *tableRecord = (TableRecord*) malloc(sizeof(TableRecord));

    tableRecord->length = length;
    tableRecord->next_record = NULL;
    tableRecord->prev_record = NULL;
    tableRecord->dataCells = dataCells;

    return tableRecord;
}

void linkToTheNextTableRecord(TableRecord *tableRecord)
{
    if (tableRecord) {
        tableRecord->next_record = tableRecord;
    }
}

void linkToThePrevTableRecord(TableRecord *tableRecord)
{
    if (tableRecord) {
        tableRecord->prev_record = tableRecord;
    }
}

TableRecord *getNextTableRecord(TableRecord *tableRecord)
{
    if (tableRecord) {
        return tableRecord->next_record;
    } else {
        return NULL;
    }
}

TableRecord *getPrevTableRecord(TableRecord *tableRecord)
{
    if (tableRecord) {
        return tableRecord->prev_record;
    } else {
        return NULL;
    }
}

DataCell **getDataCells(TableRecord *tableRecord)
{
    if (tableRecord) {
        return tableRecord->dataCells;
    } else {
        return NULL;
    }
}

void updateTableRecord(TableRecord *tableRecord, size_t cell_index, DataCell *dataCell)
{
    if (tableRecord && cell_index < tableRecord->length) {
        if (tableRecord->dataCells) {
            tableRecord->dataCells[cell_index] = dataCell;
        }
    }
}

char *transformTableRecordToJSON(TableRecord *tableRecord)
{
    /// {"RECORD":[]}

    if (!tableRecord) {
        return "";
    } else {
        size_t index = 0, cell_values_length = 0;

        for (size_t i = 0; i < tableRecord->length; i++) {
            cell_values_length += strlen(tableRecord->dataCells[i]->value);
        }

        char *line = (char*) malloc(strlen("{'':[]}") + strlen(JSON_RECORD) + cell_values_length + tableRecord->length);

        line[index] = '{';
        line[index + 1] = '"';

        index += 2;

        for (size_t i = 0; i < strlen(JSON_RECORD); i++, index++) {
            line[index] = JSON_RECORD[i];
        }

        line[index] = '"';
        line[index + 1] = ':';
        line[index + 2] = '[';

        index += 3;

        for (size_t i = 0; i < tableRecord->length; i++) {
            line[index++] = '"';

            for (size_t j = 0; j < strlen(tableRecord->dataCells[i]->value); j++, index++) {
                line[index] = tableRecord->dataCells[i]->value[j];
            }

            line[index++] = '"';

            if (i < tableRecord->length - 1) {
                line[index++] = ',';
            }
        }

        line[index] = ']';
        line[index + 1] = '}';
        line[index + 2] = '\0';

        return line;
    }
}

void destroyTableRecord(TableRecord *tableRecord)
{
    if (tableRecord) {
        if (tableRecord->dataCells) {
            for (size_t i = 0; i < tableRecord->length; i++) {
                if (tableRecord->dataCells[i]) {
                    destroyDataCell(tableRecord->dataCells[i]);
                }
            }

            free(tableRecord->dataCells);
        }

        tableRecord->dataCells = NULL;

        if (!tableRecord->prev_record) {
            if (tableRecord->next_record) {
                tableRecord->next_record->prev_record = NULL;
            }
        } else {
            if (!tableRecord->next_record) {
                tableRecord->prev_record->next_record = NULL;
            } else {
                tableRecord->prev_record->next_record = tableRecord->next_record;
                tableRecord->next_record->prev_record = tableRecord->prev_record;
            }
        }

        free(tableRecord);
    }
}


Table *createTable(TableSchema *tableSchema, char *table_name)
{
    Table *table = (Table*) malloc(sizeof(Table));

    table->tableSchema = tableSchema;
    table->table_name = table_name;
    table->length = 0;
    table->firstTableRecord = NULL;

    return table;
}

void updateTableName(Table *table, char *new_table_name)
{
    if (table) {
        table->table_name = new_table_name;
    }
}

void insertTableRecord(Table *table, TableRecord *tableRecord)
{
    if (table && tableRecord) {
        table->length++;

        if (!table->firstTableRecord) {
            table->firstTableRecord = tableRecord;
        } else {
            tableRecord->next_record = table->firstTableRecord;
            table->firstTableRecord->prev_record = tableRecord;

            table->firstTableRecord = tableRecord;
        }
    }
}

size_t getTableLength(Table *table)
{
    if (table) {
        return table->length;
    } else {
        return 0;
    }
}

TableRecord *getFirstRecordFromTable(Table *table)
{
    if (table) {
        return table->firstTableRecord;
    } else {
        return NULL;
    }
}

char *transformTableToJSON(Table *table)
{
    if (!table) {
        return "";
    } else {
        size_t index = 0, data_length = 0;

        TableRecord *currentRecord = table->firstTableRecord;

        for (size_t i = 0; i < table->length; i++) {
            char *tableRecord = transformTableRecordToJSON(currentRecord);

            data_length += strlen(tableRecord);

            currentRecord = currentRecord->next_record;

            free(tableRecord);
        }

        char *num_of_records = uint_to_str(table->length);
        char *tableSchema = transformTableSchemaToJSON(table->tableSchema);

        char *line = malloc(strlen("{'':'','':'','':,'':[]}") + strlen(JSON_TABLE_NAME) + strlen(JSON_TABLE_SIZE) + strlen(JSON_TABLE_SCHEMA) +
                strlen(JSON_TABLE_DATA) + strlen(table->table_name) + strlen(num_of_records) + strlen(tableSchema) + data_length + table->length);

        line[index] = '{';
        line[index + 1] = '"';

        index += 2;

        for (size_t i = 0; i < strlen(JSON_TABLE_NAME); i++, index++) {
            line[index] = JSON_TABLE_NAME[i];
        }

        line[index] = '"';
        line[index + 1] = ':';
        line[index + 2] = '"';

        index += 3;

        for (size_t i = 0; i < strlen(table->table_name); i++, index++) {
            line[index] = table->table_name[i];
        }

        line[index] = '"';
        line[index + 1] = ',';
        line[index + 2] = '"';

        index += 3;

        for (size_t i = 0; i < strlen(JSON_TABLE_SIZE); i++, index++) {
            line[index] = JSON_TABLE_SIZE[i];
        }

        line[index] = '"';
        line[index + 1] = ':';
        line[index + 2] = '"';

        index += 3;

        for (size_t i = 0; i < strlen(num_of_records); i++, index++) {
            line[index] = num_of_records[i];
        }

        line[index] = '"';
        line[index + 1] = ',';
        line[index + 2] = '"';

        index += 3;

        for (size_t i = 0; i < strlen(JSON_TABLE_SCHEMA); i++, index++) {
            line[index] = JSON_TABLE_SCHEMA[i];
        }

        line[index] = '"';
        line[index + 1] = ':';

        index += 2;

        for (size_t i = 0; i < strlen(tableSchema); i++, index++) {
            line[index] = tableSchema[i];
        }

        line[index] = ',';
        line[index + 1] = '"';

        index += 2;

        for (size_t i = 0; i < strlen(JSON_TABLE_DATA); i++, index++) {
            line[index] = JSON_TABLE_DATA[i];
        }

        line[index] = '"';
        line[index + 1] = ':';
        line[index + 2] = '[';

        if (table->length == 0) {
            line[index + 3] = ']';
            line[index + 4] = '}';
            line[index + 5] = '\0';
        } else {
            index += 3;

            currentRecord = table->firstTableRecord;

            for (size_t i = 0; i < table->length; i++) {
                char *tableRecord = transformTableRecordToJSON(currentRecord);

                for (size_t j = 0; j < strlen(tableRecord); j++, index++) {
                    line[index] = tableRecord[j];
                }

                if (i < table->length - 1) {
                    line[index++] = ',';
                }

                currentRecord = currentRecord->next_record;

                free(tableRecord);
            }

            line[index] = ']';
            line[index + 1] = '}';
            line[index + 2] = '\0';
        }

        free(tableSchema);
        free(num_of_records);

        return line;
    }
}

TableRecord *parseTableRecordJSON(const char *line, size_t pos, TableSchema *tableSchema, size_t *new_index) {
    if (!line || !tableSchema) {
        return NULL;
    } else {
        char *cell_value;
        DataCell **dataCells;

        size_t begin_index, index = pos;

        index += 2;
        index += strlen(JSON_RECORD);
        index += 2;

        if (tableSchema->number_of_fields > 0) {
            dataCells = (DataCell**) malloc(sizeof(DataCell*) * tableSchema->number_of_fields);

            for (size_t i = 0; i < tableSchema->number_of_fields; i++) {
                index += 2;

                begin_index = index;

                while (line[index] != '"') index++;

                cell_value = malloc(index - begin_index + 1);

                for (size_t j = begin_index; j < index; j++) {
                    cell_value[j - begin_index] = line[j];
                }

                cell_value[index - begin_index] = '\0';

                dataCells[i] = createDataCell(tableSchema->fields[i], cell_value);

                index++;
            }

            *new_index = index;

            return createTableRecord(tableSchema->number_of_fields, dataCells);
        }

        return NULL;
    }
}

Table *parseTableHeaderJSON(const char *line, size_t pos, size_t *new_index) {
    if (!line) {
        return NULL;
    } else {
        char *table_name;
        size_t table_size = 0;
        size_t schema_key_col_i = 0;
        size_t schema_num_of_fields = 0;
        char **field_names;
        Field **fields;
        TableSchema *tableSchema;
        Table *table;

        size_t begin_index, index = pos + 2;

        index += strlen(JSON_TABLE_NAME);
        index += 3;

        begin_index = index;

        while (line[index] != '"') index++;

        table_name = malloc(index - begin_index + 1);

        for (size_t i = begin_index; i < index; i++) {
            table_name[i - begin_index] = line[i];
        }

        table_name[index - begin_index] = '\0';

        index += 3;
        index += strlen(JSON_TABLE_SIZE);
        index += 3;

        while (line[index] != '"') {
            table_size += line[index] - '0';
            table_size *= 10;
            index++;
        }

        table_size /= 10;

        index += 3;
        index += strlen(JSON_TABLE_SCHEMA);
        index += 4;
        index += strlen(JSON_SCHEMA_KEY_I);
        index += 3;

        while (line[index] != '"') {
            schema_key_col_i += line[index] - '0';
            schema_key_col_i *= 10;
            index++;
        }

        schema_key_col_i /= 10;

        index += 3;
        index += strlen(JSON_SCHEMA_NUM_OF_FIELDS);
        index += 3;

        while (line[index] != '"') {
            schema_num_of_fields += line[index] - '0';
            schema_num_of_fields *= 10;
            index++;
        }

        schema_num_of_fields /= 10;

        field_names = (char **) malloc(sizeof(char *) * schema_num_of_fields);
        fields = (Field **) malloc(sizeof(Field) * schema_num_of_fields);

        index += 3;
        index += strlen(JSON_SCHEMA_FIELDS);
        index += 2;

        for (size_t i = 0; i < schema_num_of_fields; i++) {
            index += 3;
            index += strlen(JSON_FIELD_NAME);
            index += 3;

            begin_index = index;

            while (line[index] != '"') index++;

            field_names[i] = malloc(index - begin_index + 1);

            for (size_t j = begin_index; j < index; j++) {
                field_names[i][j - begin_index] = line[j];
            }

            field_names[i][index - begin_index] = '\0';

            index += 3;
            index += strlen(JSON_FIELD_TYPE);
            index += 3;

            FieldType fieldType;

            switch (line[index]) {
                case 'I':
                    fieldType = INTEGER;
                    break;

                case 'F':
                    fieldType = FLOAT;
                    break;

                case 'S':
                    fieldType = STRING;
                    break;

                case 'B':
                    fieldType = BOOLEAN;
                    break;

                default:
                    fieldType = INTEGER;
                    break;
            }

            index += 3;

            fields[i] = createField(field_names[i], fieldType);
        }

        tableSchema = createTableSchema(fields, schema_num_of_fields, schema_key_col_i);
        table = createTable(tableSchema, table_name);
        table->length = table_size;

        index += 3;
        index += strlen(JSON_TABLE_DATA);
        index += 4;

        *new_index = index;

        return table;
    }
}

Table *parseTableJSON(const char *line, size_t pos)
{
    if (!line) {
        return NULL;
    } else {
        size_t index;

        Table *table = parseTableHeaderJSON(line, pos, &index);

        if (table->length > 0) {
            size_t *new_index = malloc(sizeof(size_t));

            for (size_t i = 0; i < table->length; i++) {
                insertTableRecord(table, parseTableRecordJSON(line, index, table->tableSchema, new_index));
                table->length--;

                index = *new_index;
                index += 3;
            }

            free(new_index);
        }

        return table;
    }
}

void destroyTable(Table *table)
{
    if (table) {
        destroyTableSchema(table->tableSchema);

        table->tableSchema = NULL;
        table->table_name = NULL;

        TableRecord *tableRecord = table->firstTableRecord;

        for (size_t i = 0; i < table->length; i++) {
            TableRecord *nextTableRecord = tableRecord->next_record;

            destroyTableRecord(tableRecord);

            tableRecord = nextTableRecord;
        }

        table->length = 0;

        free(table);
    }
}