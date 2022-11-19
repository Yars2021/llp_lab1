#include <stdio.h>
#include "db_file_manager.h"

#define TARGET_FILE "/home/yars/CLionProjects/llp_lab1_c/.database"

void schemaTest()
{
    Field **fields = (Field**) malloc(sizeof(Field*) * 3);

    fields[0] = createField("1", BOOLEAN);
    fields[1] = createField("2", STRING);
    fields[2] = createField("3", INTEGER);

    TableSchema *tableSchema = createTableSchema(fields, 3, 1);

    printf("%s\n", transformTableSchemaToJSON(tableSchema));

    destroyTableSchema(tableSchema);
}

void recordTest()
{
    char **cells = (char**) malloc(sizeof(char*) * 3);

    cells[0] = createDataCell("1asddas");
    cells[1] = createDataCell("63raf-");
    cells[2] = createDataCell("gы");

    TableRecord *tableRecord = createTableRecord(3, cells);

    printf("%s\n", transformTableRecordToJSON(tableRecord));

    destroyTableRecord(tableRecord);
}

int main()
{
    freeDatabaseFile(TARGET_FILE);
    createDatabasePage(TARGET_FILE, "Test DB");
    DataPage *dataPage = (DataPage*) malloc(sizeof(DataPage));
    readDataPage(TARGET_FILE, dataPage, PAGE_DB_ROOT_INDEX);
    appendData(dataPage, "DATA DATA");
    writeDataPage(TARGET_FILE, dataPage);
    free(dataPage);

    #define len 3838
    char line[len];
    for (size_t i = 0; i < len; i++) line[i] = 'O';

    appendDataOrExpandThread(TARGET_FILE, PAGE_DB_ROOT_INDEX, line);
    appendDataOrExpandThread(TARGET_FILE, PAGE_DB_ROOT_INDEX, "SOMETHING relatively short");

    char line2[len];
    for (size_t i = 0; i < len; i++) line2[i] = 'L';

    appendDataOrExpandThread(TARGET_FILE, PAGE_DB_ROOT_INDEX, line2);
    appendDataOrExpandThread(TARGET_FILE, PAGE_DB_ROOT_INDEX, "SOMETHING relatively short AGAIN");
    return 0;
}