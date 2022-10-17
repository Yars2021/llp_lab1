//
// Created by yars on 17.10.22.
//

#ifndef LLP_LAB1_C_DB_FILE_MANAGER_H
#define LLP_LAB1_C_DB_FILE_MANAGER_H

#include <string.h>
#include "db_internals.h"

/**
 *  File structure:
 *
 *      Header (path, db_name, num_of_tables, table_node_set)
 *
 *      Table1 JSON
 *      Table2 JSON
 *      ...
 *      TableN JSON
 *
 *  The set of names is always sorted, it means finding a table will take ~log(num_of_tables) time
 *  The set of names is a red-black tree
 */

/// A node in the table set, contains table name and it's line in the file.
/// It also knows about its parent and children, which allows to make a balanced tree of TableNodes
typedef struct DBFileTableNode {
    int node_color;
    char *table_name;
    unsigned int db_table_line_number;
    struct DBFileTableNode *parent;
    struct DBFileTableNode *left_child;
    struct DBFileTableNode *right_child;
} DBFileTableNode;

/// Creates an instance of a DB name tree node
DBFileTableNode *createDBFileTableNode(char *table_name, unsigned int db_table_line_number);

/// TableNode color setter
void setDBFileTableNodeColor(DBFileTableNode *dbFileTableNode, int color);

/// TableNode parent setter
void setDBFileTableNodeParent(DBFileTableNode *dbFileTableNode, DBFileTableNode *parent);

/// TableNode left child setter
void setDBFileTableNodeLeftChild(DBFileTableNode *dbFileTableNode, DBFileTableNode *child);

/// TableNode right child setter
void setDBFileTableNodeRightChild(DBFileTableNode *dbFileTableNode, DBFileTableNode *child);

/// TableNode name setter
void updateDBFileTableNodeName(DBFileTableNode *dbFileTableNode, char *table_name);

/// TableNode line setter
void updateDBFileTableNodeLineNumber(DBFileTableNode *dbFileTableNode, unsigned int db_table_line_number);

/// TableNode color getter
int getDBFileTableNodeColor(DBFileTableNode *dbFileTableNode);

/// TableNode name getter
char *getDBFileTableName(DBFileTableNode *dbFileTableNode);

/// TableNode line getter
unsigned int getDBFileTableLineNumber(DBFileTableNode *dbFileTableNode);

/// TableNode parent getter
DBFileTableNode *getDBFileTableParent(DBFileTableNode *dbFileTableNode);

/// TableNode left child getter
DBFileTableNode *getDBFileTableLeftChild(DBFileTableNode *dbFileTableNode);

/// TableNode right child getter
DBFileTableNode *getDBFileTableRightChild(DBFileTableNode *dbFileTableNode);

/// TableNode destructor
void destroyDBFileTableNode(DBFileTableNode *dbFileTableNode);


/// DB file header contains the name of the database, file path and a tree of the table names and links to their data
typedef struct DBFileHeader {
    char *target_file_path;
    char *db_name;
    unsigned int number_of_nodes;
    DBFileTableNode *rootNode;
} DBFileHeader;

/// Creates an instance of DBFileHeader
DBFileHeader *createDBFileHeader(char *target_file_path);

/// Updates the database name
void updateDBFileHeaderDBName(DBFileHeader *dbFileHeader, char *db_name);

/// Adds a new node to the tree
int addDBFileNodeToHeader(DBFileHeader *dbFileHeader, DBFileTableNode *dbFileTableNode);

/// Removes a node from the tree
int dropDBFileNodeFromHeader(DBFileHeader *dbFileHeader, DBFileTableNode *dbFileTableNode);

/// Tree root setter
int updateDBFileHeaderRootNode(DBFileHeader *dbFileHeader, DBFileTableNode *dbFileTableNode);

/// Destroys DBFileHeader instance
void destroyDBFileHeader(DBFileHeader *dbFileHeader);


/// Updates a table in the DB file (creates a new TableNode if the table is new)
int persistTable(DBFileHeader *dbFileHeader, Table *table);

/// Finds and returns a table by it's name. Returns null if such table is not present in the database
Table *selectTable(DBFileHeader *dbFileHeader, char *table_name);

/// Removes a table by it's name from the DB file
int removeTable(DBFileHeader *dbFileHeader, char *table_name);

#endif //LLP_LAB1_C_DB_FILE_MANAGER_H
