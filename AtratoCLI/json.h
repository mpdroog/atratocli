//
//  json.h
//  AtratoCLI
//
//  Created by M Droog on 2/19/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#ifndef AtratoCLI_json_h
#define AtratoCLI_json_h

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "jsmn.h"

#define JSMN_STRICT

void json_init(void);
void json_cleanup(void);
int json_parse(char *msg);
char* json_readnext(char *key);
#endif
