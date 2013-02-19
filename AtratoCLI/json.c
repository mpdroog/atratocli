//
//  json.c
//  AtratoCLI
//
//  Created by M Droog on 2/19/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include "json.h"

jsmn_parser parser;
char *lastmsg;
jsmntok_t tokens[256];

void json_init(void) 
{
    // TODO: Initialized?
    jsmn_init(&parser);
}
void json_cleanup(void)
{
    // Nothing to do here
}

int json_parse(char *msg)
{
    // TODO: 256 max
    bzero(&tokens, sizeof(jsmntok_t)*256);
    jsmnerr_t response = jsmn_parse(&parser, msg, tokens, 256);
    if (response != JSMN_SUCCESS) {
        fprintf(stderr, "Failed reading JSON response");
        return 0;
    }
    lastmsg = msg;
    return response;
}

char* json_readnext(char *key)
{
    int nextmatch = 0;
    // TODO: Very inefficient xD
    for (int i = 0; i < 256; i++) {
        jsmntok_t pos = tokens[i];
        if (pos.type == JSMN_STRING && pos.end > 0) {
            int strsize = pos.end - pos.start;
            char* msg = (char*) malloc(sizeof(char) * (strsize+1));
            if (msg == NULL) {
                fprintf(stderr, "Failed allocating memory?");
                return NULL;
            }
            memcpy(msg, (const void*) lastmsg + pos.start, strsize);
            msg[strsize] = '\0';
            if (nextmatch == 1) {
                return msg;
            }
            if (strcmp(msg, key) == 0) {
                // Exact match, we need next value
                nextmatch = 1;
            }
            //printf("%s\n", msg);
            free(msg);
        }
    }
    
    return NULL;
}