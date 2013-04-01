//
//  table.h
//  AtratoCLI
//
//  Created by M Droog on 4/1/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#ifndef AtratoCLI_table_h
#define AtratoCLI_table_h

int table_init(void);
int table_analyze(const char* const key, const char* const value);
int table_print(FILE* output);
void table_cleanup(void);
#endif
