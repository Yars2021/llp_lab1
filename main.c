#include <stdio.h>
#include "db_internals.h"


int main() {

    Table *table = parseTableJSON("{\"TABLE_NAME\":\"TestTable\",\"TABLE_SIZE\":\"3\",\"TABLE_SCHEMA\":{\"KEY_COL_I\":\"2\",\"NUM_OF_FIELDS\":\"3\",\"FIELDS\":[{\"F_NAME\":\"string\",\"F_TYPE\":\"S\"},{\"F_NAME\":\"randomCol\",\"F_TYPE\":\"F\"},{\"F_NAME\":\"idCol\",\"F_TYPE\":\"I\"}]},\"TABLE_DATA\":[{\"RECORD\":[\"VAL-5435\",\"VAL-452\",\"VAL_ID\"]},{\"RECORD\":[\"VAL-5\",\"VAL-452\",\"VAL_ID\"]},{\"RECORD\":[\"VAL-1\",\"VAL-2\",\"VAL_ID\"]}]}");
    printf("%s", transformTableToJSON(table));

    destroyTable(table);

    return 0;
}
