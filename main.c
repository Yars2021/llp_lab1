#include <stdio.h>
#include "db_file_manager.h"

/*
 * {"PATH":"/home/yars/ClionProjects/llp_lab1_c/.database","NAME":"test_db","SIZE":"1","NODES":[{"NODE_NAME":"TestTable","LINE":"1"}]}
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
    Field **fields = (Field**) malloc(sizeof(Field*) * 3);

    fields[0] = createField("1", BOOLEAN);
    fields[1] = createField("2", STRING);
    fields[2] = createField("3", INTEGER);

    TableSchema *tableSchema = createTableSchema(fields, 3, 1);

    char **cells = (char**) malloc(sizeof(char*) * 3);

    cells[0] = createDataCell("1asddas");
    cells[1] = createDataCell("63raf-");
    cells[2] = createDataCell("gы");

    TableRecord *tableRecord = createTableRecord(3, cells);

    char **cells2 = (char**) malloc(sizeof(char*) * 3);

    cells[0] = createDataCell("asd");
    cells[1] = createDataCell("asd-");
    cells[2] = createDataCell("asd");

    TableRecord *tableRecord2 = createTableRecord(3, cells2);

    Table *table = createTable(tableSchema, "Test");
    insertTableRecord(table, tableRecord2);

    destroyTable(table);
}

int main() {
    char *line = "{\"TABLE_NAME\":\"TestTable\",\"TABLE_SIZE\":\"3\",\"TABLE_SCHEMA\":{\"KEY_COL_I\":\"2\",\"NUM_OF_FIELDS\":\"3\",\"FIELDS\":[{\"F_NAME\":\"string\",\"F_TYPE\":\"S\"},{\"F_NAME\":\"randomCol\",\"F_TYPE\":\"F\"},{\"F_NAME\":\"idCol\",\"F_TYPE\":\"I\"}]},\"TABLE_DATA\":[{\"RECORD\":[\"VAL-5435\",\"VAL-452\",\"VAL_ID\"]},{\"RECORD\":[\"VAL-5\",\"VAL-452\",\"VAL_ID\"]},{\"RECORD\":[\"VAL-1\",\"VAL-2\",\"VAL_ID\"]}]}";
    size_t records;

    tableTest();

    return 0;
}
