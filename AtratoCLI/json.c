//
//  json.c
//  AtratoCLI
//
//  Created by M Droog on 2/19/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include "json.h"
#include "global.h"

#define JSON_TOKENS 2048

extern int verbose;

jsmn_parser _parser;
const char *_lastmsg = NULL;
jsmntok_t _tokens[JSON_TOKENS];
char *_lastkey = NULL;

char* internal_get_string(const jsmntok_t const token);

void json_init(void) 
{
    bzero(&_parser, sizeof(jsmn_parser));
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
            char character = *(_lastmsg + pos.start);
            
            if (character == 't') {
                return 1;
            }
            return 0;
        }
        if (nextmatch == 0 && pos.type == JSMN_STRING && pos.end > 0) {
            // TODO: Duplicate
            size_t strsize = SAFE_SIZET(pos.end - pos.start);
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
            size_t strsize = SAFE_SIZET(pos.end - pos.start);
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

// Allocate and return a stripped out string
char* internal_get_string(const jsmntok_t const token)
{
    size_t strsize = SAFE_SIZET(token.end - token.start);
    char* msg = (char*) malloc(sizeof(char) * (strsize+1));
    if (msg == NULL) {
        fprintf(stderr, "JSON: Malloc failed\n");
        return NULL;
    }
    memcpy(msg, (const void*) _lastmsg + token.start, strsize);
    // TODO: Validate response?
    msg[strsize] = '\0';
    return msg;
}

int json_array_search(const char* baseName, int(*searchFn)(const char* key), int(*printFn)(const char* key, const char* value) )
{
    size_t begin_index = 0;
    size_t end_offset = 0;

    // Step1. Find base element we start reading from
    // base element is a stringkey pointing to an array    
    for (begin_index = 0; begin_index < JSON_TOKENS; begin_index++) {
        jsmntok_t token = _tokens[begin_index];
        jsmntok_t next;
        bzero(&next, sizeof(jsmntok_t));

        if (end_offset == 0) {
            if (token.type == JSMN_STRING) {
                char* key = internal_get_string(token);
                if (strcmp(key, baseName) == 0) {
                    // Found base element
                    begin_index++;
                    next = _tokens[begin_index];
                    if (next.type != JSMN_ARRAY) {
                        fprintf(stderr, "JSON: Malformed JSON, baseName not containing array\n");
                        return 1;
                    }
                    if (next.end == 0) {
                        fprintf(stderr, "JSON: Malformed JSON, baseName it's array is 0chars in memory\n");
                        return 1;
                    }
                    
                    int end = next.end;
                    if (end < 0) {
                        return 1;
                    }
                    end_offset = (size_t) end;
                    if (verbose) {
                        fprintf(stdout, "JSON: Found end offset at %ld\n", end_offset);
                    }
                    
                    // Stop iteration now we have position
                    break;
                }
                free(key);
            }
        }
    }
    
    if (end_offset == 0) {
        fprintf(stderr, "Failed finding baseName: %s\n", baseName);
        return 1;
    }

    // Step2. Iterate on values
    // % 2 == 0 means key, else value if interested
    int printNext = 0;
    int offset = 0;
    for (size_t i = begin_index; i < JSON_TOKENS; i++) {
        jsmntok_t token = _tokens[i];
        // Check if we can stop processing
        size_t start = SAFE_SIZET(token.start);
        if (start > end_offset) {
            break;
        }
        
        if (token.type == JSMN_STRING || token.type == JSMN_PRIMITIVE) {
            if (offset % 2 == 0) {
                // Every 2 steps check if value after key needs
                // to be printed
                if (token.type != JSMN_STRING) {
                    fprintf(stderr, "Key is expected to be String, received something else #%d\n", token.type);
                    return 1;
                }
                char* key = internal_get_string(token);
                if (searchFn(key) == 1) {
                    printNext = 1;
                    _lastkey = key;
                }
            } else if(printNext == 1) {
                // Value that needs to be printed
                printNext = 0;
                if (token.type == JSMN_PRIMITIVE) {
                    printFn(_lastkey, NULL);
                }
                else if (token.type == JSMN_STRING) {
                    char* value = internal_get_string(token);
                    // TODO: result ignored?
                    printFn(_lastkey, value);
                    _lastkey = NULL;
                    free(value);
                }
                else {
                    fprintf(stderr, "Unsupported type #%d\n", token.type);
                    return 1;
                }
                
                // Free the last key
                free(_lastkey);
                _lastkey = NULL;
            }
            offset++;
        }
    }
    
    return 0;
}