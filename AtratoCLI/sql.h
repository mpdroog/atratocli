//
//  sql.h
//  AtratoCLI
//
//  Created by M Droog on 3/22/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#ifndef AtratoCLI_sql_h
#define AtratoCLI_sql_h

#include "sqlite3.h"
typedef sqlite3_stmt sql_stmt;

int sql_open(const char* path);
int sql_exec(int(*callback)(void *NotUsed, int argc, char **argv, char **azColName), const char* query);
sql_stmt* sql_statement(const char* query);
int sql_statement_bind_string(sql_stmt* stmt, const int index, const char* value);
int sql_statement_store(sql_stmt* stmt);
int sql_statement_reset(sql_stmt* stmt);
int sql_statement_close(sql_stmt* stmt);
void sql_cleanup(void);
void sql_errmsg(void);

#endif
