//
// Created by yars on 22.11.22.
//

#ifndef LLP_LAB1_C_DB_INTERFACE_H
#define LLP_LAB1_C_DB_INTERFACE_H

#include "db_internals.h"

#define FILTER_REJECT 0
#define FILTER_ACCEPT 1
#define FILTER_INCOMPATIBLE (-1)
#define FILTER_NULL_POINTER (-2)

typedef struct {
    Field *field;
    size_t field_index;
    void *lower_threshold;
    void *upper_threshold;
} SearchFilter;

/// Creates a new instance of a SearchFilter.
SearchFilter *createSearchFilter(Field *field, void *lower_threshold, void *upper_threshold);

/// Binds the filter to the column by its index.
void bindFilter(SearchFilter *searchFilter, size_t column);

/// Applies teh filter to the data cell. If the type is incompatible, returns FILTER_INCOMPATIBLE.
int applyFilter(SearchFilter *searchFilter, const char *dataCell);

/// Filters a table record.
int applyAll(TableRecord *tableRecord, size_t num_of_filters, SearchFilter **filters);

/// Destroys the SearchFilter.
void destroySearchFilter(SearchFilter *searchFilter);

#endif //LLP_LAB1_C_DB_INTERFACE_H
