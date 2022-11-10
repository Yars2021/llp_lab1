#include <stdio.h>
#include "db_file_manager.h"

/*
 * {"PATH":"/home/yars/CLionProjects/llp_lab1_c/.database","NAME":"test_db","SIZE":"1","NODES":[{"NODE_NAME":"TestTable","LINE":"1"}]}
{"TABLE_NAME":"TestTable","TABLE_SIZE":"3","TABLE_SCHEMA":{"KEY_COL_I":"2","NUM_OF_FIELDS":"3","FIELDS":[{"F_NAME":"string","F_TYPE":"S"},{"F_NAME":"randomCol","F_TYPE":"F"},{"F_NAME":"idCol","F_TYPE":"I"}]},"TABLE_DATA":[{"RECORD":["VAL-1","VAL-2","VAL_ID"]},{"RECORD":["VAL-5","VAL-452","VAL_ID"]},{"RECORD":["VAL-5435","VAL-452","VAL_ID"]}]}
 */

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

void tableTest()
{

}

int main()
{
    Field **fields = (Field**) malloc(sizeof(Field*) * 3);

    fields[0] = createField("1", BOOLEAN);
    fields[1] = createField("2", STRING);
    fields[2] = createField("3", INTEGER);

    TableSchema *tableSchema = createTableSchema(fields, 3, 1);

    size_t i;
    TableRecord *tableRecord = parseTableRecordJSON("{\"RECORD\":[\"123\",\"22\",\"d\"]}", 0, &i, tableSchema);

    printf("%s\n", tableRecord->dataCells[0]);
    printf("%s\n", tableRecord->dataCells[1]);
    printf("%s\n", tableRecord->dataCells[2]);
    printf("%s\n", transformTableRecordToJSON(tableRecord));
    printf("%zd %zd\n", i, strlen(transformTableRecordToJSON(tableRecord)));

    destroyTableSchema(tableSchema);
    destroyTableRecord(tableRecord);

    Field *field = parseFieldJSON("{\"F_NAME\":\"test name\",\"F_TYPE\":\"F\"}", 0, &i);

    printf("%s\n%zd %zd\n", transformFieldToJSON(field), i, strlen(transformFieldToJSON(field)));

    destroyField(field);
    return 0;
}