//
// Created by yars on 17.10.22.
//

#include "db_file_manager.h"

DBFileTableNode *createDBFileTableNode(char *table_name, unsigned int db_table_line_number)
{
    DBFileTableNode *dbFileTableNode = (DBFileTableNode*) malloc(sizeof(DBFileTableNode));

    dbFileTableNode->table_name = table_name;
    dbFileTableNode->db_table_line_number = db_table_line_number;
    dbFileTableNode->node_color = RBT_CLR_RED;
    dbFileTableNode->parent = NULL;
    dbFileTableNode->left_child = dbFileTableNode->right_child = NULL;

    return dbFileTableNode;
}

void setDBFileTableNodeColor(DBFileTableNode *dbFileTableNode, int color)
{
    if (dbFileTableNode) {
        dbFileTableNode->node_color = color;
    }
}

void setDBFileTableNodeParent(DBFileTableNode *dbFileTableNode, DBFileTableNode *parent)
{
    if (dbFileTableNode) {
        dbFileTableNode->parent = parent;
    }
}

void setDBFileTableNodeLeftChild(DBFileTableNode *dbFileTableNode, DBFileTableNode *child)
{
    if (dbFileTableNode) {
        dbFileTableNode->left_child = child;
    }
}

void setDBFileTableNodeRightChild(DBFileTableNode *dbFileTableNode, DBFileTableNode *child)
{
    if (dbFileTableNode) {
        dbFileTableNode->right_child = child;
    }
}

void updateDBFileTableNodeName(DBFileTableNode *dbFileTableNode, char *table_name)
{
    if (dbFileTableNode) {
        dbFileTableNode->table_name = table_name;
    }
}

void updateDBFileTableNodeLineNumber(DBFileTableNode *dbFileTableNode, unsigned int db_table_line_number)
{
    if (dbFileTableNode) {
        dbFileTableNode->db_table_line_number = db_table_line_number;
    }
}

int getDBFileTableNodeColor(DBFileTableNode *dbFileTableNode)
{
    if (!dbFileTableNode) {
        return 0;
    } else {
        return dbFileTableNode->node_color;
    }
}

char *getDBFileTableName(DBFileTableNode *dbFileTableNode)
{
    if (!dbFileTableNode) {
        return NULL;
    } else {
        return dbFileTableNode->table_name;
    }
}

unsigned int getDBFileTableLineNumber(DBFileTableNode *dbFileTableNode)
{
    if (!dbFileTableNode) {
        return 0;
    } else {
        return dbFileTableNode->db_table_line_number;
    }
}

DBFileTableNode *getDBFileTableParent(DBFileTableNode *dbFileTableNode)
{
    if (!dbFileTableNode) {
        return NULL;
    } else {
        return dbFileTableNode->parent;
    }
}

DBFileTableNode *getDBFileTableLeftChild(DBFileTableNode *dbFileTableNode)
{
    if (!dbFileTableNode) {
        return NULL;
    } else {
        return dbFileTableNode->left_child;
    }
}

DBFileTableNode *getDBFileTableRightChild(DBFileTableNode *dbFileTableNode)
{
    if (!dbFileTableNode) {
        return NULL;
    } else {
        return dbFileTableNode->right_child;
    }
}


DBFileHeader *createDBFileHeader(char *target_file_path)
{
    FILE *file = fopen(target_file_path, "r");

    if (!file) return NULL;  // Invalid path (file does not exist)

    DBFileHeader *dbFileHeader = (DBFileHeader*) malloc(sizeof(DBFileHeader));
    dbFileHeader->target_file_path = target_file_path;

    char *line = NULL;
    size_t len = 0;
    ssize_t exitcode = getline(&line, &len, file);

    if (exitcode == -1) {                   // File is empty, creating an empty header
        dbFileHeader->db_name = "";
        dbFileHeader->number_of_nodes = 0;
        dbFileHeader->rootNode = NULL;
    } else {                                // Parsing the database header line into a struct
        char *db_name;
        size_t db_size = 0;

        size_t begin_index, index = 2;

        index += strlen(JSON_DB_PATH);
        index += 3;
        index += strlen(target_file_path);
        index += 3;
        index += strlen(JSON_DB_NAME);
        index += 3;

        begin_index = index;
        while (line[index] != '"') index++;

        db_name = malloc(index - begin_index + 1);
        for (size_t i = begin_index; i < index; i++) {
            db_name[i - begin_index] = line[i];
        }

        db_name[index - begin_index] = '\0';

        dbFileHeader->db_name = db_name;

        index += 3;
        index += strlen(JSON_DB_SIZE);
        index += 3;

        while (line[index] != '"') {
            db_size += line[index] - '0';
            db_size *= 10;
            index++;
        }

        db_size /= 10;

        dbFileHeader->number_of_nodes = db_size;

        // ToDo Add the node insertion after implementing the RBTree
    }

    fclose(file);

    return dbFileHeader;
}

void updateDBFileHeaderDBName(DBFileHeader *dbFileHeader, char *db_name)
{
    if (dbFileHeader) {
        dbFileHeader->db_name = db_name;
    }
}

int addDBFileNodeToHeader(DBFileHeader *dbFileHeader, DBFileTableNode *dbFileTableNode)
{

}

int dropDBFileNodeFromHeader(DBFileHeader *dbFileHeader, DBFileTableNode *dbFileTableNode)
{

}

int updateDBFileHeaderRootNode(DBFileHeader *dbFileHeader, DBFileTableNode *dbFileTableNode)
{

}

char *serializeHeader(DBFileHeader *dbFileHeader)
{
    if (!dbFileHeader) {
        return "";
    } else {
        size_t combined_node_length = 100;  // ToDo Calculate it's actual value
        char *line = (char*) malloc(strlen("{'PATH':'','NAME':'','SIZE':'','NODES':[]}") + strlen(dbFileHeader->target_file_path) +
                                            strlen(dbFileHeader->db_name) + strlen(uint_to_str(dbFileHeader->number_of_nodes)) + combined_node_length + 1);

        char *db_size = uint_to_str(dbFileHeader->number_of_nodes);
        size_t index = 0;

        line[index] = '{';
        line[index + 1] = '"';

        index += 2;

        for (size_t i = 0; i < strlen(JSON_DB_PATH); i++, index++) {
            line[index] = JSON_DB_PATH[i];
        }

        line[index] = '"';
        line[index + 1] = ':';
        line[index + 2] = '"';

        index += 3;

        for (size_t i = 0; i < strlen(dbFileHeader->target_file_path); i++, index++) {
            line[index] = dbFileHeader->target_file_path[i];
        }

        line[index] = '"';
        line[index + 1] = ',';
        line[index + 2] = '"';

        index += 3;

        for (size_t i = 0; i < strlen(JSON_DB_NAME); i++, index++) {
            line[index] = JSON_DB_NAME[i];
        }

        line[index] = '"';
        line[index + 1] = ':';
        line[index + 2] = '"';

        index += 3;

        for (size_t i = 0; i < strlen(dbFileHeader->db_name); i++, index++) {
            line[index] = dbFileHeader->db_name[i];
        }

        line[index] = '"';
        line[index + 1] = ',';
        line[index + 2] = '"';

        index += 3;

        for (size_t i = 0; i < strlen(JSON_DB_SIZE); i++, index++) {
            line[index] = JSON_DB_SIZE[i];
        }

        line[index] = '"';
        line[index + 1] = ':';
        line[index + 2] = '"';

        index += 3;

        for (size_t i = 0; i < strlen(db_size); i++, index++) {
            line[index] = db_size[i];
        }

        line[index] = '"';
        line[index + 1] = ',';
        line[index + 2] = '"';

        index += 3;

        for (size_t i = 0; i < strlen(JSON_DB_NODES); i++, index++) {
            line[index] = JSON_DB_NODES[i];
        }

        line[index] = '"';
        line[index + 1] = ':';
        line[index + 2] = '[';

        //ToDo Add node serialization

        line[index + 3] = ']';
        line[index + 4] = '}';

        free(db_size);

        return line;
    }
}

void persistDatabase(DBFileHeader *dbFileHeader)
{
    if (dbFileHeader) {
        FILE *file = fopen(dbFileHeader->target_file_path, "w");

        if (!file) return;  // Invalid path (file could not be created)

        fseek(file, 0, SEEK_SET);
        fprintf(file, "%s", serializeHeader(dbFileHeader));

        // ToDo Make it fprintf all tables after the header

        fclose(file);
    }
}

void destroyDBFileHeader(DBFileHeader *dbFileHeader)
{

}


int persistTable(DBFileHeader *dbFileHeader, Table *table)
{

}

Table *selectTable(DBFileHeader *dbFileHeader, const char *table_name)
{

}

int removeTable(DBFileHeader *dbFileHeader, const char *table_name)
{

}

void compress(DBFileHeader *dbFileHeader)
{

}