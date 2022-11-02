//
// Created by yars on 30.09.22.
//
#include "db_internals.h"

char *uint_to_str(unsigned int num)
{
    size_t length = 0;
    char res[32];

    do {
        res[length] = num % 10 + '0';
        length++;
        num /= 10;
    } while (num > 0);

    char *rev_res = (char*) malloc(length + 1);

    rev_res[length] = '\0';

    for (size_t i = 0; i < length; i++) rev_res[i] = res[length - i - 1];

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

    if (!field) return "";

    char type, *line = (char*) malloc(strlen("{'':'','':''}") + strlen(JSON_FIELD_NAME) + strlen(JSON_FIELD_TYPE) + strlen(field->field_name) + 2);

    switch (field->fieldType) {
        case INTEGER:
            type = 'I';
            break;
        case FLOAT:
            type = 'F';
            break;
        case BOOLEAN:
            type = 'B';
            break;
        default:
            type = 'S';
            break;
    }

    sprintf(line, "{\"%s\":\"%s\",\"%s\":\"%c\"}", JSON_FIELD_NAME, field->field_name, JSON_FIELD_TYPE, type);

    return line;
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

char *transformTableSchemaToJSON(TableSchema *tableSchema)
{
    /// {"KEY_COL_I":"","":"NUM_OF_FIELDS":"","FIELDS":[]}

    if (!tableSchema) return "";

    size_t field_lengths = 0, offset = 0;

    for (size_t i = 0; i < tableSchema->number_of_fields; i++) {
        char *field_line = transformFieldToJSON(tableSchema->fields[i]);
        field_lengths += strlen(field_line);
        free(field_line);
    }

    char *fields = (char*) malloc(field_lengths + tableSchema->number_of_fields);

    for (size_t i = 0; i < tableSchema->number_of_fields; i++) {
        char *field = transformFieldToJSON(tableSchema->fields[i]);
        memcpy(fields + offset, field, strlen(field));
        offset += (strlen(field) + 1);
        fields[offset - 1] = ',';
        free(field);
    }

    fields[field_lengths + tableSchema->number_of_fields - 1] = '\0';

    char *line = (char*) malloc(strlen("{'':'','':'','':[]}") + strlen(JSON_SCHEMA_KEY_I) + strlen(JSON_SCHEMA_NUM_OF_FIELDS) + strlen(JSON_SCHEMA_FIELDS) +
            field_lengths + tableSchema->number_of_fields + 64);

    sprintf(line, "{\"%s\":\"%zd\",\"%s\":\"%zd\",\"%s\":[%s]}", JSON_SCHEMA_KEY_I, tableSchema->key_column_index, JSON_SCHEMA_NUM_OF_FIELDS, tableSchema->number_of_fields,
            JSON_SCHEMA_FIELDS, fields);

    free(fields);

    return line;
}

void destroyTableSchema(TableSchema *tableSchema)
{
    if (tableSchema) {
        if (tableSchema->fields) {
            for (size_t i = 0; i < tableSchema->number_of_fields; i++)
                if (tableSchema->fields[i]) destroyField(tableSchema->fields[i]);

            free(tableSchema->fields);
        }

        tableSchema->fields = NULL;
        tableSchema->number_of_fields = 0;
        tableSchema->key_column_index = 0;

        free(tableSchema);
    }
}


char *createDataCell(const char *value)
{
    if (!value) return NULL;

    char *cell = (char*) malloc(strlen(value) + 1);

    memcpy(cell, value, strlen(value));
    cell[strlen(value)] = '\0';

    return cell;
}

TableRecord *createTableRecord(size_t length, char **dataCells)
{
    TableRecord *tableRecord = (TableRecord*) malloc(sizeof(TableRecord));

    tableRecord->length = length;
    tableRecord->next_record = NULL;
    tableRecord->prev_record = NULL;
    tableRecord->dataCells = dataCells;

    return tableRecord;
}

char *transformTableRecordToJSON(TableRecord *tableRecord)
{
    /// {"RECORD":[]}

    if (!tableRecord) return "";

    size_t cell_values_length = 0, offset = 0;
    for (size_t i = 0; i < tableRecord->length; i++) cell_values_length += strlen(tableRecord->dataCells[i]);

    char *record_data = (char*) malloc(tableRecord->length * 3 + cell_values_length);

    for (size_t i = 0; i < tableRecord->length; i++) {
        record_data[offset++] = '"';
        memcpy(record_data + offset, tableRecord->dataCells[i], strlen(tableRecord->dataCells[i]));
        offset += (strlen(tableRecord->dataCells[i]) + 1);
        record_data[offset - 1] = '"';
        record_data[offset++] = ',';
    }

    record_data[tableRecord->length * 3 + cell_values_length - 1] = '\0';

    char *line = (char*) malloc(strlen("{'':[]}") + strlen(JSON_RECORD) + tableRecord->length * 3 + cell_values_length);

    sprintf(line, "{\"%s\":[%s]}", JSON_RECORD, record_data);

    free(record_data);

    return line;
}

void destroyTableRecord(TableRecord *tableRecord)
{
    if (tableRecord) {
        if (tableRecord->dataCells) {
            for (size_t i = 0; i < tableRecord->length; i++)
                if (tableRecord->dataCells[i])
                    free(tableRecord->dataCells[i]);

            free(tableRecord->dataCells);
        }

        tableRecord->dataCells = NULL;

        if (!tableRecord->prev_record) {
            if (tableRecord->next_record) tableRecord->next_record->prev_record = NULL;
        } else {
            if (!tableRecord->next_record) tableRecord->prev_record->next_record = NULL;
            else {
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

void insertTableRecord(Table *table, TableRecord *tableRecord)
{
    if (!table || !tableRecord) return;

    table->length++;

    if (!table->firstTableRecord) table->firstTableRecord = tableRecord;
    else {
        tableRecord->next_record = table->firstTableRecord;
        table->firstTableRecord->prev_record = tableRecord;
        table->firstTableRecord = tableRecord;
    }
}

char *transformTableToJSON(Table *table)    // ToDo Rewrite using memcpy
{
    /// {"TABLE_NAME":"","TABLE_SIZE":"","TABLE_SCHEMA":,"TABLE_DATA":[]}

    if (!table) return "";

    size_t data_length = 0;
    TableRecord *currentRecord = table->firstTableRecord;

    for (size_t i = 0; i < table->length; i++) {
        char *tableRecord = transformTableRecordToJSON(currentRecord);
        data_length += strlen(tableRecord);
        currentRecord = currentRecord->next_record;
        free(tableRecord);
    }

    char *tableSchema = transformTableSchemaToJSON(table->tableSchema);

    char *line = (char*) malloc(strlen("{'':'','':'','':,'':[]}") + strlen(JSON_FIELD_NAME) + strlen(JSON_TABLE_SIZE) + strlen(JSON_TABLE_SCHEMA) +
            strlen(JSON_TABLE_DATA) + strlen(table->table_name) + strlen(tableSchema) + data_length + table->length + 64);

    free(tableSchema);

    return line;
}

TableRecord *parseTableRecordJSON(const char *line, size_t pos, TableSchema *tableSchema, size_t *new_index) {
    if (!line || !tableSchema) {
        return NULL;
    } else {
        char *cell_value;
        char **dataCells;

        size_t begin_index, index = pos;

        index += 2;
        index += strlen(JSON_RECORD);
        index += 2;

        if (tableSchema->number_of_fields > 0) {
            dataCells = (char**) malloc(sizeof(char*) * tableSchema->number_of_fields);

            for (size_t i = 0; i < tableSchema->number_of_fields; i++) {
                index += 2;
                begin_index = index;

                while (line[index] != '"') index++;

                cell_value = malloc(index - begin_index + 1);
                for (size_t j = begin_index; j < index; j++) cell_value[j - begin_index] = line[j];
                cell_value[index - begin_index] = '\0';
                dataCells[i] = cell_value;

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

        for (size_t i = begin_index; i < index; i++) table_name[i - begin_index] = line[i];

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

            for (size_t j = begin_index; j < index; j++) field_names[i][j - begin_index] = line[j];

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

void destroyTable(Table *table) // ToDo Rewrite
{
    if (table) {

    }
}