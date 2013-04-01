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
#include <stdlib.h>

static sqlite3* _db = NULL;
char* sql_error = NULL;

extern int verbose;

int sql_open(const char* path)
{
    if (_db != NULL) {
        fprintf(stderr, "Sqlite3 connection already open\n");
        return 1;
    }
    
    if (verbose) {
        fprintf(stdout, "SQLite open: %s\n", path);
    }
    if (sqlite3_open(path, &_db)) {
        if (verbose) {
            fprintf(stderr, "SQL: %s\n", sqlite3_errmsg(_db));
        }
        sqlite3_close(_db);
        return 1;
    }
    return 0;
}

int sql_exec(int(*callback)(void *NotUsed, int argc, char **argv, char **azColName), const char* query)
{
    if (verbose) {
        fprintf(stdout, "SQL Query: %s\n", query);
    }    
    int res = sqlite3_exec(_db, query, callback, 0, &sql_error);
    if (res != SQLITE_OK) {
        if (verbose) {
            fprintf(stderr, "SQL Error: %s\n", sql_error);
        }
        sqlite3_free(sql_error);
        sql_error = NULL;
        return 1;
    }
    return 0;
}

sql_stmt* sql_statement(const char* query)
{
    sqlite3_stmt *stmt = NULL;
    int res = sqlite3_prepare_v2(_db, query, -1, &stmt, NULL);
    if (res != SQLITE_OK) {
        sql_errmsg();
        return NULL;
    }
    return stmt;
}

int sql_statement_bind_null(sql_stmt* stmt, const int index)
{
    int res = sqlite3_bind_null(stmt, index);
    if (res != SQLITE_OK) {
        if (verbose) {
            fprintf(stderr, "SQL: Error binding index %d with NULL\n", index);
        }
        return 1;
    }
    return 0;
}

int sql_statement_bind_string(sql_stmt* stmt, const int index, const char* value)
{
    int res = sqlite3_bind_text(stmt, index, value, -1, SQLITE_TRANSIENT);
    if (res != SQLITE_OK) {
        if (verbose) {
            fprintf(stderr, "SQL: Error binding index %d with value %s\n", index, value);
        }
        return 1;
    }
    return 0;
}

int sql_statement_store(sql_stmt* stmt)
{
    int res = sqlite3_step(stmt);
    if (res != SQLITE_DONE) {
        sql_errmsg();
        return 1;
    }
    return 0;
}

int sql_statement_reset(sql_stmt* stmt)
{
    if (sqlite3_reset(stmt) == SQLITE_OK) {
        return 0;        
    }
    
    sql_errmsg();    
    return 1;
}

int sql_statement_close(sql_stmt* stmt)
{
    if (sqlite3_finalize(stmt) == SQLITE_OK) {
        return 0;
    }

    sql_errmsg();
    return 1;
}

void sql_errmsg(void)
{
    if (verbose) {
        fprintf(stderr, "SQL: %s\n", sqlite3_errmsg(_db));
    }
}


void sql_cleanup(void)
{
    sqlite3_free(sql_error);
    sqlite3_close(_db);
    _db = NULL;
}