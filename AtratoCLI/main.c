//
//  main.c
//  AtratoCLI
//
//  Created by M Droog on 2/16/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "jsmn.h"
#include "global.h"
#include "http.h"

#define JSMN_STRICT

// -------------- Functions

int main (int argc, const char * argv[])
{
    // TODO: Add verbose/debug flag?
    if (http_init() != 0) {
        return 1;
    }
        
    HttpResponse *json = http_get(API_HASH);
    http_cleanup();
    if (json == NULL) {
        return 1;
    }
    printf("Raw response: %s\n\n", json->buffer);
    
    jsmn_parser parser;
    jsmn_init(&parser);
    
    jsmntok_t tokens[256];
    bzero(&tokens, sizeof(jsmntok_t)*256);
    jsmnerr_t response = jsmn_parse(&parser, json->buffer, tokens, 256);
    if (response != JSMN_SUCCESS) {
        fprintf(stderr, "Failed reading JSON response");
        return 1;
    }
    
    for (int i = 0; i < 256; i++) {
        jsmntok_t pos = tokens[i];
        if (pos.type == JSMN_STRING && pos.end > 0) {
            int strsize = pos.end - pos.start;
            char* msg = (char*) malloc(sizeof(char) * (strsize+1));
            if (msg == NULL) {
                fprintf(stderr, "Failed allocating memory?");
                return 1;
            }
            memcpy(msg, (const void*) json->buffer + pos.start, strsize);
            msg[strsize] = '\0';
            printf("%s\n", msg);
            free(msg);
        }
    }
    
    free(json);
    printf("EOF\n");
    return 0;
}

