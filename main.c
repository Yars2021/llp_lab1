#include <stdio.h>
#include "db_file_manager.h"

/*
 * {"PATH":"/home/yars/ClionProjects/llp_lab1_c/.database","NAME":"test_db","SIZE":"1","NODES":[{"NODE_NAME":"TestTable","LINE":"1"}]}
{"TABLE_NAME":"TestTable","TABLE_SIZE":"3","TABLE_SCHEMA":{"KEY_COL_I":"2","NUM_OF_FIELDS":"3","FIELDS":[{"F_NAME":"string","F_TYPE":"S"},{"F_NAME":"randomCol","F_TYPE":"F"},{"F_NAME":"idCol","F_TYPE":"I"}]},"TABLE_DATA":[{"RECORD":["VAL-1","VAL-2","VAL_ID"]},{"RECORD":["VAL-5","VAL-452","VAL_ID"]},{"RECORD":["VAL-5435","VAL-452","VAL_ID"]}]}
 */

int main() {

   // Table *table = parseTableJSON("{\"TABLE_NAME\":\"TestTable\",\"TABLE_SIZE\":\"3\",\"TABLE_SCHEMA\":{\"KEY_COL_I\":\"2\",\"NUM_OF_FIELDS\":\"3\",\"FIELDS\":[{\"F_NAME\":\"string\",\"F_TYPE\":\"S\"},{\"F_NAME\":\"randomCol\",\"F_TYPE\":\"F\"},{\"F_NAME\":\"idCol\",\"F_TYPE\":\"I\"}]},\"TABLE_DATA\":[{\"RECORD\":[\"VAL-5435\",\"VAL-452\",\"VAL_ID\"]},{\"RECORD\":[\"VAL-5\",\"VAL-452\",\"VAL_ID\"]},{\"RECORD\":[\"VAL-1\",\"VAL-2\",\"VAL_ID\"]}]}");
   // printf("%s", transformTableToJSON(table));

   // destroyTable(table);

    DBFileHeader *dbFileHeader = createDBFileHeader("/home/yars/CLionProjects/llp_lab1_c/.database");

    printf("%s\n", dbFileHeader->target_file_path);
    printf("%s\n", dbFileHeader->db_name);
    printf("%d\n", dbFileHeader->number_of_nodes);

    persistDatabase(dbFileHeader);

    free(dbFileHeader);

    return 0;
}
