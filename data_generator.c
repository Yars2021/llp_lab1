//
// Created by yars on 23.11.22.
//

#include "data_generator.h"

char *generateRandomString(size_t min_len, size_t max_len)
{
    size_t len = (random() % (max_len - min_len + 1)) + min_len;
    char *string = (char*) malloc(len + 1);
    string[len] = '\0';
    for (size_t i = 0; i < len; i++) string[i] = (char)(random() % 59 + 64);
    return string;
}

char *generateRandomIntString(size_t min_len, size_t max_len)
{
    size_t len = (random() % (max_len - min_len + 1)) + min_len;
    char *string = (char*) malloc(len + 1);
    string[len] = '\0';
    for (size_t i = 0; i < len; i++) string[i] = (char)(random() % 10 + '0');
    if (random() % 2) {
        string[0] = '-';
        while (string[1] == '0') string[1] = (char)(random() % 10 + '0');
    }
    while (string[0] == '0') string[0] = (char)(random() % 10 + '0');
    return string;
}

char *generateRandomFloatString(size_t min_len, size_t max_len)
{
    size_t len = (random() % (max_len - min_len + 1)) + min_len, pt = 0;
    char *string = (char*) malloc(len + 1);
    string[len] = '\0';
    for (size_t i = 0; i < len; i++) string[i] = (char)(random() % 10 + '0');
    if (random() % 2) {
        string[0] = '-';
        while (string[1] == '0') string[1] = (char)(random() % 10 + '0');
        while (pt < 2) pt = random() % len;
    } else {
        while (string[0] == '0') string[0] = (char) (random() % 10 + '0');
        while (pt < 1) pt = random() % (len - 1);
    }
    string[pt] = '.';
    return string;
}

Field *generateField()
{
    return createField(generateRandomString(5, 15), random() % 4);
}

TableSchema *generateSchema(size_t min_len, size_t max_len)
{
    size_t len = (random() % (max_len - min_len + 1)) + min_len;
    Field **fields = (Field**) malloc(sizeof(Field*) * len);
    for (size_t i = 0; i < len; i++) fields[i] = generateField();
    return createTableSchema(fields, len, random() % len);
}

TableRecord *generateRecord(TableSchema *tableSchema)
{
    char **dataCells = (char**) malloc(sizeof(char*) * tableSchema->number_of_fields);
    for (size_t i = 0; i < tableSchema->number_of_fields; i++) {
        switch (tableSchema->fields[i]->fieldType) {
            case INTEGER:
                dataCells[i] = generateRandomIntString(2, 15);
                break;
            case FLOAT:
                dataCells[i] = generateRandomFloatString(5, 25);
                break;
            case BOOLEAN:
                if (random() % 2) dataCells[i] = createDataCell("False");
                else dataCells[i] = createDataCell("True");
                break;
            default:
                dataCells[i] = generateRandomString(10, 50);
                break;
        }
    }
    return createTableRecord(tableSchema->number_of_fields, dataCells);
}

void generateTable(char *filename, char *table_name, size_t min_cols, size_t max_cols, size_t min_length, size_t max_length)
{
    TableSchema *tableSchema = generateSchema(min_cols, max_cols);
    Table *table = createTable(tableSchema, table_name);
    size_t len = (random() % (max_length - min_length + 1)) + min_length;
    for (size_t i = 0; i < len; i++) insertTableRecord(table, generateRecord(tableSchema));
    addTableHeader(filename, table);
    destroyTable(table);
}

void generateDatabase(char *filename, size_t num_of_tables, char **table_names, size_t min_length, size_t max_length)
{
    freeDatabaseFile(filename);
    createDatabasePage(filename, "Randomly Generated Database");
    srandom(time(NULL));

    for (size_t i = 0; i < num_of_tables; i++) generateTable(filename, table_names[i], 3, 6, min_length, max_length);
}