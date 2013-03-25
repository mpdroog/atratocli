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

const char* _structure_credentials = "CREATE TABLE 'website_credentials' ("
"'credential_username' text NOT NULL,"
"'credential_value' text NOT NULL,"
"'credential_website' text NOT NULL,"
"'credential_hostname' text)";

const char* _query_credential_add = "INSERT INTO website_credentials(?, ?, ?, ?)";

static sql_stmt* _stmt = NULL;
static int index = 0;

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
        return 1;
    }
    index = 0;
    return 0;
}

int db_statement_string(const char* value)
{
    return sql_statement_bind_string(_stmt, ++index, value);
}

int db_statement_next(void)
{
    return sql_statement_reset(_stmt);
}

int db_statement_store(void)
{
    return sql_statement_store(_stmt);
}