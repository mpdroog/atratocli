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
"'website_credential_index' integer NOT NULL,"
"'credential_username' text NOT NULL,"
"'credential_value' text NOT NULL,"
"'credential_website' text NOT NULL,"
"'credential_hostname' text)";

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
    sql_cleanup();
}
