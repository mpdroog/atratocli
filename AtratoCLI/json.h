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
#include "strutils.h"

//#define JSMN_STRICT

void json_init(void);
int json_parse(const char *msg);
char* json_readstring(const char *key);
int json_readprimitive(const char *key);

void json_array(void);
#endif
