//
//  hash.h
//  AtratoCLI
//
//  Created by M Droog on 2/19/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#ifndef AtratoCLI_hash_h
#define AtratoCLI_hash_h

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>

#include "md5.h"

void hash_md5init(void);
void hash_md5add(const char* input);
char* hash_md5str(void);

#endif
