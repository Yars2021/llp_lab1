//
// Created by yars on 22.11.22.
//

#include "db_interface.h"

SearchFilter *createSearchFilter(Field *field, void *lower_threshold, void *upper_threshold)
{
    SearchFilter *searchFilter = (SearchFilter*) malloc(sizeof(SearchFilter));
    searchFilter->field = field;
    searchFilter->lower_threshold = lower_threshold;
    searchFilter->upper_threshold = upper_threshold;
    return searchFilter;
}

void bindFilter(SearchFilter *searchFilter, size_t column)
{
    if (!searchFilter) return;
    searchFilter->field_index = column;
}

int cmpInteger(char *a, char *b)
{

}

int cmpFloat(char *a, char *b)
{

}

int cmpBoolean(char *a, char *b)
{
    if (strcmp(a, b) == 0) return 0;
    if (strcmp(a, "True") == 0 && strcmp(b, "False") == 0) return 1;
    return -1;
}

int applyFilter(SearchFilter *searchFilter, const char *dataCell)
{
    if (!searchFilter || !searchFilter->field || !dataCell) return FILTER_NULL_POINTER;
    switch (searchFilter->field->fieldType) {
        case INTEGER:
            // ToDo Add sign checks
            break;
        case FLOAT:
            // ToDo Add sign checks
            break;
        case BOOLEAN:
            // ToDo False < True
            break;
        default: {
            if (!searchFilter->lower_threshold) {
                if (!searchFilter->upper_threshold) return FILTER_ACCEPT;
                else {
                    if (strcmp(dataCell, (char*) searchFilter->upper_threshold) > 0) return FILTER_REJECT;
                    else return FILTER_ACCEPT;
                }
            } else {
                if (!searchFilter->upper_threshold) {
                    if (strcmp(dataCell, (char*) searchFilter->lower_threshold) < 0) return FILTER_REJECT;
                    else return FILTER_ACCEPT;
                } else {
                    if (strcmp(dataCell, (char*) searchFilter->lower_threshold) < 0 || strcmp(dataCell, (char*) searchFilter->upper_threshold) > 0)
                        return FILTER_REJECT;
                    else return FILTER_ACCEPT;
                }
            }
        }
    }

    return FILTER_INCOMPATIBLE;
}

int applyAll(TableRecord *tableRecord, size_t num_of_filters, SearchFilter **filters)
{
    if (!tableRecord) return FILTER_NULL_POINTER;
    if (num_of_filters == 0 || !filters) return FILTER_ACCEPT;
    if (num_of_filters > tableRecord->length) return FILTER_INCOMPATIBLE;

    for (size_t i = 0; i < num_of_filters; i++) {
        if (filters[i]->field_index > tableRecord->length) return FILTER_INCOMPATIBLE;
        int exitcode = applyFilter(filters[i], tableRecord->dataCells[filters[i]->field_index]);
        if (exitcode == FILTER_ACCEPT) continue;
        else return exitcode;
    }

    return FILTER_ACCEPT;
}

void destroySearchFilter(SearchFilter *searchFilter)
{
    if (!searchFilter) return;
    free(searchFilter->lower_threshold);
    free(searchFilter->upper_threshold);
    free(searchFilter);
}
