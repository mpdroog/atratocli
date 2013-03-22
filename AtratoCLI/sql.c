//
//  sql.c
//  AtratoCLI
//
//  Created by M Droog on 3/22/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include "sql.h"
#include "sqlite3.h"
#include <stdio.h>

static sqlite3* _db = NULL;
char* sql_error = NULL;

int sql_open(const char* path)
{
    if (sqlite3_open(path, &_db)) {
        fprintf(stderr, "Can't open database %s\n", sqlite3_errmsg(_db));
        sqlite3_close(_db);
        return 1;
    }
    return 0;
}

int sql_exec(int(*callback)(void *NotUsed, int argc, char **argv, char **azColName), const char* query)
{
    int res = sqlite3_exec(_db, query, callback, 0, &sql_error);
    if (res != SQLITE_OK) {
        fprintf(stdout, "SQL Error: %s\n", sql_error);
        sqlite3_free(sql_error);
        sql_error = NULL;
        return 1;
    }
    
    return 0;
}

void sql_cleanup(void)
{
    sqlite3_free(sql_error);
    sqlite3_close(_db);
}