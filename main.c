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

int main()
{
    char *line = "{\"KEY_COL_I\":\"1\",\"NUM_OF_FIELDS\":\"3\",\"FIELDS\":[{\"F_NAME\":\"Field\",\"F_TYPE\":\"B\"},{\"F_NAME\":\"f\",\"F_TYPE\":\"S\"},{\"F_NAME\":\"int\",\"F_TYPE\":\"I\"}]}";
    size_t i;
    TableSchema *tableSchema = parseTableSchemaJSON(line, 0, &i);

    printf("%s\n%zd\n%zd\n", transformTableSchemaToJSON(tableSchema), i, strlen(transformTableSchemaToJSON(tableSchema)));

    destroyTableSchema(tableSchema);

    createDatabase("/home/yars/CLionProjects/llp_lab1_c/.database", "TestDB");
    return 0;
}