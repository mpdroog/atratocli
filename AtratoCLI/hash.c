//
//  hash.c
//  AtratoCLI
//
//  Created by M Droog on 2/19/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include "hash.h"

md5_context _md5;

void hash_md5init(void)
{
    md5_starts(&_md5);    
}
void hash_md5add(const char* input)
{
    md5_update(&_md5, (unsigned char*)input, strlen(input));
}

char* hash_md5str(void)
{
    unsigned char digest[16];
    md5_finish(&_md5, digest);    
    
    char *md5string = malloc(sizeof(char)*33);
    bzero(md5string, sizeof(char)*33);
    for(int i = 0; i < 16; ++i) {
        sprintf(&md5string[i*2], "%02x", (unsigned int)digest[i]);
    }
    return md5string;
}
