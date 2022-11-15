#include <stdio.h>
#include "db_file_manager.h"

#define PATH "/home/yars/CLionProjects/llp_lab1_c/.database"

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
    createDatabasePage(PATH, "TestDB");
    return 0;
}