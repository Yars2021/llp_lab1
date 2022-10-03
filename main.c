#include <stdio.h>
#include "db_internals.h"

int main() {
    Field **fields = (Field**) malloc(sizeof(Field*) * 3);

    fields[0] = createField("string", STRING);
    fields[1] = createField("randomCol", FLOAT);
    fields[2] = createField("idCol", INTEGER);

    TableSchema *tableSchema = createTableSchema(fields, 3, 2);

    Table *table = createTable(tableSchema, "TestTable");
    DataCell **dataCells = (DataCell**) malloc(sizeof(DataCell*) * table->tableSchema->number_of_fields);

    dataCells[0] = createDataCell(tableSchema->fields[0], "VAL-1");
    dataCells[1] = createDataCell(tableSchema->fields[1], "VAL-2");
    dataCells[2] = createDataCell(tableSchema->fields[2], "VAL_ID");

    TableRecord *tableRecord1 = createTableRecord(table->tableSchema->number_of_fields, dataCells);

    insertTableRecord(table, tableRecord1);

    char *res = transformTableToJSON(table);

    printf("%s\n", res);

    free(res);

    destroyTable(table);

    return 0;
}
