//
//  db.c
//  AtratoCLI
//
//  Created by M Droog on 3/22/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include "db.h"
#include "sql.h"
#include <stdio.h>
#include <string.h>

extern int verbose;

const char* const _structure_credentials = "CREATE TABLE 'website_credentials' ("
"'credential_username' text NOT NULL,"
"'credential_value' text NOT NULL,"
"'credential_website' text NOT NULL,"
"'credential_hostname' text"
"CONSTRAINT 'unique_entry' UNIQUE (credential_username, credential_value, credential_website, credential_hostname))";

const char* const _query_credential_add = "INSERT INTO website_credentials VALUES(?, ?, ?, ?)";

static sql_stmt* _stmt = NULL;
static int internal_field_index(const char* key);

int db_open(void)
{
    return sql_open(DB_PATH);
}

int db_init(void)
{
    return sql_exec(NULL, _structure_credentials);
}

void db_cleanup(void)
{
    if (sql_statement_close(_stmt) == 1) {
        fprintf(stderr, "Failed flushing statement\n");
    }
    sql_cleanup();
}

int db_statement(void)
{
    _stmt = sql_statement(_query_credential_add);
    if (_stmt == NULL) {
        sql_errmsg();
        return 1;
    }
    return 0;
}

int db_statement_null(const char* key)
{
    return sql_statement_bind_null(_stmt, internal_field_index(key));
}

int db_statement_string(const char* key, const char* value)
{
    if (verbose) {
        printf("Key: %s value: %s\n", key, value);
    }
    return sql_statement_bind_string(_stmt, internal_field_index(key), value);
}

int db_statement_next(void)
{
    return sql_statement_reset(_stmt);
}

int db_statement_store(void)
{
    return sql_statement_store(_stmt);
}

static int internal_field_index(const char* key)
{
    if (strcmp(key, "credential_username") == 0) {
        return 1;
    }
    if (strcmp(key, "credential_value") == 0) {
        return 2;
    }
    if (strcmp(key, "credential_website") == 0) {
        return 3;
    }
    if (strcmp(key, "credential_hostname") == 0) {
        return 4;
    }
    return 0;
}