//
//  sql.h
//  AtratoCLI
//
//  Created by M Droog on 3/22/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#ifndef AtratoCLI_sql_h
#define AtratoCLI_sql_h

int sql_open(const char* path);
int sql_exec(int(*callback)(void *NotUsed, int argc, char **argv, char **azColName), const char* query);
void sql_cleanup(void);

#endif
