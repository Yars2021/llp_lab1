//
// Created by yars on 23.11.22.
//

#ifndef LLP_LAB1_C_DATA_GENERATOR_H
#define LLP_LAB1_C_DATA_GENERATOR_H

#include <time.h>
#include "db_internals.h"
#include "db_file_manager.h"

/// Creates a random string.
char *generateRandomString(size_t min_len, size_t max_len);

/// Creates a random string, which can be parsed into a signed int value.
char *generateRandomIntString(size_t min_len, size_t max_len);

/// Creates a random string, which can be parsed into a signed float value.
char *generateRandomFloatString(size_t min_len, size_t max_len);

/// Creates a random field.
Field *generateField();

/// Creates a random schema of random fields.
TableSchema *generateSchema(size_t min_len, size_t max_len);

/// Creates a random table row.
TableRecord *generateRecord(TableSchema *tableSchema);

/// Creates a random table.
void generateTable(char *filename, char *table_name, size_t min_cols, size_t max_cols, size_t min_length, size_t max_length);

/// Creates a random database.
void generateDatabase(char *filename, size_t num_of_tables, char **table_names, size_t min_length, size_t max_length);

#endif //LLP_LAB1_C_DATA_GENERATOR_H
