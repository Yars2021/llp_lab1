#include <stdio.h>
#include <time.h>
#include "db_file_manager.h"
#include "data_generator.h"

#define TARGET_FILE "/home/yars/CLionProjects/llp_lab1_c/.database"
#define NUM_OF_TABLES 15
#define MIN_TABLE_LEN 10
#define MAX_TABLE_LEN 10

void performanceTestMode(int argc, char **argv)
{
    char **table_names = (char**) malloc(sizeof(char**) * NUM_OF_TABLES);
    table_names[0] = createDataCell("Table0");
    table_names[1] = createDataCell("Table1");
    table_names[2] = createDataCell("Table2");
    table_names[3] = createDataCell("Table3");
    table_names[4] = createDataCell("Table4");
    table_names[5] = createDataCell("Table5");
    table_names[6] = createDataCell("Table6");
    table_names[7] = createDataCell("Table7");
    table_names[8] = createDataCell("Table8");
    table_names[9] = createDataCell("Table9");
    table_names[10] = createDataCell("Table10");
    table_names[11] = createDataCell("Table11");
    table_names[12] = createDataCell("Table12");
    table_names[13] = createDataCell("Table13");
    table_names[14] = createDataCell("Table14");

    if (argc <= 1) {
        printf("No argument provided, use -h to see the manual.\n");
    } else {
        if (argv[1][0] != '-') printf("Invalid argument format, use -h to see the manual.\n");
        else {
            srandom(time(NULL));
            clock_t start = clock();
            int print_time = 1;
            switch (argv[1][1]) {
                case 'h': {
                    printf("User manual:\n");
                    printf("WARNING: all tests require a generated database. Use -g to fill it with random data.\n");
                    printf("-h : prints out the key manual for the program.\n");
                    printf("-g : generates a new set of data.\n");
                    printf("-i : performs the insertion test. Inserts 50 random records into every table.\n");
                    printf("-s : performs the selection test. Prints first 50 records of a random table and demonstrates the usage of SearchFilters.\n");
                    printf("-d : performs the deletion test. Deletes a random table.\n");
                    printf("-u : performs the update test. Updates the records with ID between 0 and 10 in a random table.\n");
                    printf("-r : performs the removal test. Removes the records with ID between 5 and 25 from a random table.\n");
                    printf("-j : performs the join test. Joins 2 random tables on their IDs.\n");
                    print_time = 0;
                    break;
                }
                case 'g': {
                    printf("Generating a new dataset. This might take some time...\n");
                    generateDatabase(TARGET_FILE, NUM_OF_TABLES, table_names, MIN_TABLE_LEN, MAX_TABLE_LEN);
                    break;
                }
                case 'i': {
                    clock_t insertion_time = start;
                    printf("Performing the insertion test...\n");
                    for (size_t i = 0; i < NUM_OF_TABLES; i++) {
                        printf("Inserting into \"%s\"...\n", table_names[i]);
                        uint32_t maxID = findAndGetMaxID(TARGET_FILE, table_names[i]);

                        TableSchema *tableSchema = getSchema(TARGET_FILE, table_names[i]);
                        Table *table = createTable(tableSchema, table_names[i]);
                        size_t id_counter = maxID + 1;
                        for (size_t j = 0; j < 50; j++) insertTableRecord(table, generateRecord(tableSchema, &id_counter));
                        insertTableRecords(TARGET_FILE, table);
                        destroyTable(table);
                        findAndUpdateMaxID(TARGET_FILE, table_names[i], id_counter - 1);

                        printf("The insertion took %lf seconds\n\n", (double)(clock() - insertion_time) / CLOCKS_PER_SEC);
                        insertion_time = clock();
                    }
                    break;
                }
                case 's': {
                    size_t table_index = random() % NUM_OF_TABLES;
                    printf("Performing the selection test on \"%s\"...\n\n", table_names[table_index]);
                    TableSchema *tableSchema = getSchema(TARGET_FILE, table_names[table_index]);
                    size_t key_col = tableSchema->key_column_index;
                    destroyTableSchema(tableSchema);
                    uint64_t max_id = 50;
                    SearchFilter *id_constraint = createSearchFilter(INTEGER, NULL, &max_id);
                    bindFilter(id_constraint, key_col);
                    printTable(TARGET_FILE, table_names[table_index], 1, &id_constraint);
                    destroySearchFilter(id_constraint);
                    break;
                }
                case 'd': {
                    size_t table_index = random() % NUM_OF_TABLES;
                    printf("Performing the deletion test on \"%s\"...\n\n", table_names[table_index]);
                    deleteTable(TARGET_FILE, table_names[table_index]);
                    break;
                }
                case 'u': {
                    size_t table_index = random() % NUM_OF_TABLES;
                    printf("Performing the update test on \"%s\"...\n\n", table_names[table_index]);
                    TableSchema *tableSchema = getSchema(TARGET_FILE, table_names[table_index]);
                    size_t key_col = tableSchema->key_column_index, len = tableSchema->number_of_fields;
                    destroyTableSchema(tableSchema);
                    uint64_t min_id = 0, max_id = 10;
                    SearchFilter *id_constraint = createSearchFilter(INTEGER, &min_id, &max_id);
                    bindFilter(id_constraint, key_col);

                    char **newCells = (char**) malloc(sizeof(char*) * len);
                    for (size_t i = 0; i < len; i++) newCells[i] = createDataCell("REPLACED");
                    TableRecord *newValue = createTableRecord(len, newCells);

                    updateRows(TARGET_FILE, table_names[table_index], newValue, 1, &id_constraint);
                    destroyTableRecord(newValue);
                    destroySearchFilter(id_constraint);
                    break;
                }
                case 'r': {
                    size_t table_index = random() % NUM_OF_TABLES;
                    printf("Performing the removal test on \"%s\"...\n\n", table_names[table_index]);
                    TableSchema *tableSchema = getSchema(TARGET_FILE, table_names[table_index]);
                    size_t key_col = tableSchema->key_column_index;
                    destroyTableSchema(tableSchema);
                    uint64_t min_id = 5, max_id = 25;
                    SearchFilter *id_constraint = createSearchFilter(INTEGER, &min_id, &max_id);
                    bindFilter(id_constraint, key_col);
                    deleteRows(TARGET_FILE, table_names[table_index], 1, &id_constraint);
                    destroySearchFilter(id_constraint);
                    break;
                }
                case 'j': {
                    size_t l_table_index = random() % NUM_OF_TABLES, r_table_index = random() % NUM_OF_TABLES;
                    printf("Performing the join test on \"%s\" and \"%s\"...\n\n", table_names[l_table_index], table_names[r_table_index]);
                    innerJoinSelect(TARGET_FILE, table_names[l_table_index], table_names[r_table_index], 0, 0, 0, NULL, 0, NULL);
                    break;
                }
                default:
                    print_time = 0;
                    printf("Invalid argument, use -h to see the manual.\n");
                    break;
            }
            if (print_time) printf("The task took %lf seconds.\n", (double)(clock() - start) / CLOCKS_PER_SEC);
        }
    }

    for (size_t i = 0; i < NUM_OF_TABLES; i++) free(table_names[i]);
    free(table_names);
}

int main(int argc, char **argv)
{
    performanceTestMode(argc, argv);
    return 0;
}