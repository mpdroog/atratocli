//
//  db.h
//  AtratoCLI
//
//  Created by M Droog on 3/22/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#ifndef AtratoCLI_db_h
#define AtratoCLI_db_h

int db_open(const char* const path);
void db_cleanup(void);
int db_init(void);

int db_statement(void);
int db_statement_null(const char* key);
int db_statement_string(const char* key, const char* value);
int db_statement_next(void);
int db_statement_store(void);

#endif
