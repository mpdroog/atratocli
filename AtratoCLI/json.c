//
//  json.c
//  AtratoCLI
//
//  Created by M Droog on 2/19/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include "json.h"
#define JSON_TOKENS 1024

jsmn_parser _parser = {};
const char *_lastmsg = NULL;
jsmntok_t _tokens[JSON_TOKENS];

void json_init(void) 
{
    // TODO: Initialized?
    jsmn_init(&_parser);
}

int json_parse(const char *msg)
{
    // TODO: tokens max
    bzero(&_tokens, sizeof(jsmntok_t)*JSON_TOKENS);
    jsmnerr_t response = jsmn_parse(&_parser, msg, _tokens, JSON_TOKENS);
    if (response == JSMN_ERROR_NOMEM) {
        fprintf(stderr, "JSON token array too small\n");
        return 0;
    }
    if (response == JSMN_ERROR_INVAL) {
        printf("Server response: %s", msg);
        fprintf(stderr, "JSON contains invalid tokens\n");
        return 0;
    }
    if (response == JSMN_ERROR_PART) {
        fprintf(stderr, "JSON is received broken\n");
        return 0;
    }
    
    if (response != JSMN_SUCCESS) {
        fprintf(stderr, "Failed reading JSON response %d\n", response);
        return 0;
    }
    _lastmsg = msg;
    return 1;
}

int json_readprimitive(const char *key)
{
    int nextmatch = 0;
    // TODO: Very inefficient xD
    for (int i = 0; i < JSON_TOKENS; i++) {
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
    for (int i = 0; i < JSON_TOKENS; i++) {
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

void json_array_search(const char* baseName, int(*searchFn)(const char* key), int(*printFn)(const char* key, const char* value) ) {
    int nextmatch = 0;
    int end = 0;
    int printcount = 0;
    int printNextKey = 0;
    
    for (int i = 0; i < JSON_TOKENS; i++) {
        jsmntok_t pos = _tokens[i];
        // pos == JSMN_ARRAY then end = end of last string
        if (nextmatch == 1 && end == 0) {
            end = pos.end;
        }
        if (end > 0 && pos.start > end) {
            break;
        }
        
        if (pos.type == JSMN_PRIMITIVE && pos.end > 0) {
            printcount++;
            printf("%-30s", "NULL");
        }
        if (pos.type == JSMN_STRING && pos.end > 0) {
            // TODO: Duplicate
            int strsize = pos.end - pos.start;
            char* msg = (char*) malloc(sizeof(char) * (strsize+1));
            if (msg == NULL) {
                fprintf(stderr, "Failed allocating memory?");
                return;
            }
            memcpy(msg, (const void*) _lastmsg + pos.start, strsize);
            msg[strsize] = '\0';
            if (nextmatch == 1 && end > 0) {
                printcount++;
                if (printcount % 2 == 0 && printNextKey == 1) {
                    printNextKey = 0;
                    printFn("X", msg);
                }
                else if (printcount % 2 == 1) {
                    printNextKey = searchFn(msg);
                }
            }
            if (strcmp(msg, baseName) == 0) {
                // Exact match, we need next values
                nextmatch = 1;
            }
            free(msg);
        }
    }
}

void json_array(void) {
    int nextmatch = 0;
    int end = 0;
    int printcount = 0;
    
    printf("%-30s%-30s%-30s%-30s\n", "Hostname", "Website", "Username", "Password");
    for (int i = 0; i < 111; i++) {
        printf("-");
    }
    printf("\n");

    // TODO: Very inefficient xD
    for (int i = 0; i < JSON_TOKENS; i++) {
        jsmntok_t pos = _tokens[i];
        // pos == JSMN_ARRAY then end = end of last string
        if (nextmatch == 1 && end == 0) {
            end = pos.end;
        }
        if (end > 0 && pos.start > end) {
            break;
        }
        
        if (pos.type == JSMN_PRIMITIVE && pos.end > 0) {
            printcount++;
            printf("%-30s", "NULL");
        }
        if (pos.type == JSMN_STRING && pos.end > 0) {
            // TODO: Duplicate
            int strsize = pos.end - pos.start;
            char* msg = (char*) malloc(sizeof(char) * (strsize+1));
            if (msg == NULL) {
                fprintf(stderr, "Failed allocating memory?");
                return;
            }
            memcpy(msg, (const void*) _lastmsg + pos.start, strsize);
            msg[strsize] = '\0';
            if (nextmatch == 1 && end > 0) {
                printcount++;
                if (printcount % 2 == 0) {
                    char *decoded = str_replace("\\/", "/", msg);
                    char *max = str_substr(0, 28, decoded);
                    printf("%-30s", max);
                    free(max);
                    free(decoded);
                }
                
                if (printcount % 8 == 0) {
                    printf("\n");
                }
            }
            if (strcmp(msg, "result") == 0) {
                // Exact match, we need next values
                nextmatch = 1;
            }
            //printf("%s\n", msg);
            free(msg);
        }
    }
    printf("\n");
}