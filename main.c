#include <stdio.h>
#include "db_file_manager.h"

#define TARGET_FILE "/home/yars/CLionProjects/llp_lab1_c/.database"

/// Creates 7 tables across 2 pages. Puts some random \0s between some of them then searches for all of them and for the "no" table, which does not exist.
void tableSearchTest()
{
    freeDatabaseFile(TARGET_FILE);
    createDatabasePage(TARGET_FILE, "Test DB");
    DataPage *dataPage = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(TARGET_FILE, dataPage, PAGE_DB_ROOT_INDEX);
    updateNumberOfPages(dataPage, 1);
    updateNumberOfTables(dataPage, 7);
    appendData(dataPage, "\0");
    appendData(dataPage, "\0");
    appendData(dataPage, "\0");
    appendData(dataPage, "{\"T_NAME\":\"table\",\"T_LINK\":\"1\"}");
    appendData(dataPage, "{\"T_NAME\":\"four\",\"T_LINK\":\"4\"}");
    appendData(dataPage, "\0");
    appendData(dataPage, "\0");
    appendData(dataPage, "{\"T_NAME\":\"seven with spaces\",\"T_LINK\":\"7\"}");
    appendData(dataPage, "{\"T_NAME\":\"LARGE_NUMBER\",\"T_LINK\":\"349\"}");
    writeDataPage(TARGET_FILE, dataPage);
    free(dataPage);

    expandPageThread(TARGET_FILE, PAGE_DB_ROOT_INDEX);
    dataPage = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(TARGET_FILE, dataPage, PAGE_DB_ROOT_INDEX + 1);
    updateNumberOfPages(dataPage, 1);
    appendData(dataPage, "{\"T_NAME\":\"123\",\"T_LINK\":\"11244\"}");
    appendData(dataPage, "\0");
    appendData(dataPage, "{\"T_NAME\":\"gggggg\",\"T_LINK\":\"42\"}");
    appendData(dataPage, "\0");
    appendData(dataPage, "{\"T_NAME\":\"     f\",\"T_LINK\":\"38\"}");
    writeDataPage(TARGET_FILE, dataPage);
    free(dataPage);

    size_t exitcode;
    exitcode = findTable(TARGET_FILE, "table");
    printf("%zd\n\n", exitcode);

    exitcode = findTable(TARGET_FILE, "four");
    printf("%zd\n\n", exitcode);

    exitcode = findTable(TARGET_FILE, "seven with spaces");
    printf("%zd\n\n", exitcode);

    exitcode = findTable(TARGET_FILE, "LARGE_NUMBER");
    printf("%zd\n\n", exitcode);

    exitcode = findTable(TARGET_FILE, "no");
    printf("%zd\n\n", exitcode);

    exitcode = findTable(TARGET_FILE, "123");
    printf("%zd\n\n", exitcode);

    exitcode = findTable(TARGET_FILE, "gggggg");
    printf("%zd\n\n", exitcode);

    exitcode = findTable(TARGET_FILE, "     f");
    printf("%zd\n\n", exitcode);
}

/// Creates a table named Table1 with 250 records in it and then adds 100 more after creating another table. If del is true, drops Table1 in the end.
void tableCreationTest(int del)
{
    freeDatabaseFile(TARGET_FILE);
    createDatabasePage(TARGET_FILE, "Test DB");

    Field **fields = (Field**) malloc(sizeof(Field*) * 3);

    fields[0] = createField("F1", INTEGER);
    fields[1] = createField("Field2", INTEGER);
    fields[2] = createField("Data Field 3", INTEGER);

    TableSchema *tableSchema = createTableSchema(fields, 3, 0);

    Table *table = createTable(tableSchema, "Table1");

    for (size_t i = 0; i < 250; i++) {
        char **rec = (char**) malloc(sizeof(char*) * 3);
        rec[0] = createDataCell("First");
        rec[1] = createDataCell("Second");
        rec[2] = createDataCell("Third");
        insertTableRecord(table, createTableRecord(3, rec));
    }

    addTableHeader(TARGET_FILE, table);

    DataPage *dataPage = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(TARGET_FILE, dataPage, findTable(TARGET_FILE, "Table1"));
    printf("%zd\n", getTableLength(dataPage));
    free(dataPage);

    table = createTable(tableSchema, "Table2");

    for (size_t i = 0; i < 160; i++) {
        char **rec = (char**) malloc(sizeof(char*) * 3);
        rec[0] = createDataCell("1");
        rec[1] = createDataCell("2");
        rec[2] = createDataCell("3");
        insertTableRecord(table, createTableRecord(3, rec));
    }

    addTableHeader(TARGET_FILE, table);

    destroyTable(table);

    dataPage = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(TARGET_FILE, dataPage, findTable(TARGET_FILE, "Table2"));
    printf("%zd\n", getTableLength(dataPage));
    free(dataPage);

    table = createTable(tableSchema, "Table1");

    for (size_t i = 0; i < 100; i++) {
        char **rec = (char**) malloc(sizeof(char*) * 3);
        rec[0] = createDataCell("ADDED1");
        rec[1] = createDataCell("ADDED2");
        rec[2] = createDataCell("ADDED3");
        insertTableRecord(table, createTableRecord(3, rec));
    }

    insertTableRecords(TARGET_FILE, table);

    destroyTable(table);

    dataPage = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(TARGET_FILE, dataPage, findTable(TARGET_FILE, "Table1"));
    printf("%zd\n", getTableLength(dataPage));
    free(dataPage);

    if (del) deleteTable(TARGET_FILE, "Table1");
}

int main()
{
    tableCreationTest(0);
    return 0;
}