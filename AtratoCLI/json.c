//
//  json.c
//  AtratoCLI
//
//  Created by M Droog on 2/19/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include "json.h"

jsmn_parser _parser;
const char *_lastmsg = NULL;
jsmntok_t _tokens[256];

void json_init(void) 
{
    // TODO: Initialized?
    jsmn_init(&_parser);
}

int json_parse(const char *msg)
{
    // TODO: 256 max
    bzero(&_tokens, sizeof(jsmntok_t)*256);
    jsmnerr_t response = jsmn_parse(&_parser, msg, _tokens, 256);
    if (response != JSMN_SUCCESS) {
        fprintf(stderr, "Failed reading JSON response %d", response);
        return 0;
    }
    _lastmsg = msg;
    return response;
}

int json_readprimitive(const char *key)
{
    int nextmatch = 0;
    // TODO: Very inefficient xD
    for (int i = 0; i < 256; i++) {
        jsmntok_t pos = _tokens[i];
        if (nextmatch == 1 && pos.type == JSMN_PRIMITIVE && pos.start > 0) {
            //int strsize = pos.end - pos.start;
            char character = *(_lastmsg + pos.start);
            
            if (character == 't') {
                return 1;
            }
            return 0;
        }
        if (nextmatch == 0 && pos.type == JSMN_STRING && pos.end > 0) {
            // TODO: Duplicate
            int strsize = pos.end - pos.start;
            char* msg = (char*) malloc(sizeof(char) * (strsize+1));
            if (msg == NULL) {
                fprintf(stderr, "Failed allocating memory?");
                return 0;
            }
            memcpy(msg, (const void*) _lastmsg + pos.start, strsize);
            msg[strsize] = '\0';
            if (strcmp(msg, key) == 0) {
                // Exact match, we need next value
                nextmatch = 1;
            }
            free(msg);
        }
    }
    
    return 0;
}
char* json_readstring(const char *key)
{
    int nextmatch = 0;
    // TODO: Very inefficient xD
    for (int i = 0; i < 256; i++) {
        jsmntok_t pos = _tokens[i];
        if (pos.type == JSMN_STRING && pos.end > 0) {
            // TODO: Duplicate
            int strsize = pos.end - pos.start;
            char* msg = (char*) malloc(sizeof(char) * (strsize+1));
            if (msg == NULL) {
                fprintf(stderr, "Failed allocating memory?");
                return NULL;
            }
            memcpy(msg, (const void*) _lastmsg + pos.start, strsize);
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