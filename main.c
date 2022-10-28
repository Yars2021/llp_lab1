#include <stdio.h>
#include "db_file_manager.h"

/*
 * {"PATH":"/home/yars/ClionProjects/llp_lab1_c/.database","NAME":"test_db","SIZE":"1","NODES":[{"NODE_NAME":"TestTable","LINE":"1"}]}
{"TABLE_NAME":"TestTable","TABLE_SIZE":"3","TABLE_SCHEMA":{"KEY_COL_I":"2","NUM_OF_FIELDS":"3","FIELDS":[{"F_NAME":"string","F_TYPE":"S"},{"F_NAME":"randomCol","F_TYPE":"F"},{"F_NAME":"idCol","F_TYPE":"I"}]},"TABLE_DATA":[{"RECORD":["VAL-1","VAL-2","VAL_ID"]},{"RECORD":["VAL-5","VAL-452","VAL_ID"]},{"RECORD":["VAL-5435","VAL-452","VAL_ID"]}]}
 */

// Parsing the whole table VS header + 1 record
int main() {
    char *line = "{\"TABLE_NAME\":\"TestTable\",\"TABLE_SIZE\":\"3\",\"TABLE_SCHEMA\":{\"KEY_COL_I\":\"2\",\"NUM_OF_FIELDS\":\"3\",\"FIELDS\":[{\"F_NAME\":\"string\",\"F_TYPE\":\"S\"},{\"F_NAME\":\"randomCol\",\"F_TYPE\":\"F\"},{\"F_NAME\":\"idCol\",\"F_TYPE\":\"I\"}]},\"TABLE_DATA\":[{\"RECORD\":[\"VAL-5435\",\"VAL-452\",\"VAL_ID\"]},{\"RECORD\":[\"VAL-5\",\"VAL-452\",\"VAL_ID\"]},{\"RECORD\":[\"VAL-1\",\"VAL-2\",\"VAL_ID\"]}]}";
    size_t records = 0;

    Table *table = parseTableJSON(line, 0);
    printf("%s\n", transformTableToJSON(table));

    destroyTable(table);

    table = parseTableHeaderJSON(line, 0, &records);
    table->length = 0;

    insertTableRecord(table, parseTableRecordJSON(line, records, table->tableSchema, &records));

    printf("%s\n", transformTableToJSON(table));

    destroyTable(table);

    return 0;
}
