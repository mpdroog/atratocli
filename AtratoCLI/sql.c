//
//  sql.c
//  AtratoCLI
//
//  Created by M Droog on 3/22/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include "sql.h"
#include <stdio.h>
#include <string.h>

static sqlite3* _db = NULL;
char* sql_error = NULL;

extern int verbose;

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

sql_stmt* sql_statement(const char* query)
{
    sqlite3_stmt *stmt = NULL;
    int res = sqlite3_prepare(_db, query, -1, &stmt, 0);
    if (res != SQLITE_OK) {
        return NULL;
    }
    return stmt;
}

int sql_statement_bind_string(sql_stmt* stmt, const int index, const char* value) {
    int res = sqlite3_bind_text(stmt, index, value, -1, SQLITE_TRANSIENT);
    if (res != SQLITE_OK) {
        if (verbose) {
            fprintf(stderr, "Error on using index %d with value %s\n", index, value);
        }
        return 1;
    }
    return 0;
}

int sql_statement_store(sql_stmt* stmt)
{
    int res = sqlite3_step(stmt);
    if (res != SQLITE_DONE) {
        return 1;
    }
    return 0;
}

int sql_statement_reset(sql_stmt* stmt)
{
    return sqlite3_reset(stmt) == SQLITE_OK;
}

int sql_statement_close(sql_stmt* stmt)
{
    return sqlite3_finalize(stmt) == SQLITE_OK;
}


void sql_cleanup(void)
{
    sqlite3_free(sql_error);
    sqlite3_close(_db);
}