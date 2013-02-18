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
#include <curl/curl.h>
#include "jsmn.h"

#define JSMN_STRICT
#define API_URL "http://atrato.cc/api.php"
#define API_HASH "?c=authentication&m=hash"
#define API_LOGIN "?c=authentication&m=login"
#define API_LIST "?c=general&m=list"

struct JsonStr {
    char *buffer;
    size_t len;
    size_t pos;
};
typedef struct JsonStr JsonStr;

CURL *_curl;
FILE *_cookiejar;
char* _filename;

void cleanup(void);
char* geturl(char* query);
static size_t curlresponse(void *ptr, size_t size, size_t nmemb, void *stream);

// -------------- Functions

void cleanup(void)
{
    curl_easy_cleanup(_curl);
    fclose(_cookiejar);
    free(_filename);
}

char* geturl(char* query)
{
    if (256 - strlen(query) + strlen(API_URL) < 1) {
        fprintf(stderr, "Url too long!");
        abort();
    }
    char *url = malloc(sizeof(char)*256);
    strcpy(url, API_URL);
    strcat(url, query);
    return url;
}

static size_t curlresponse(void *ptr, size_t size, size_t nmemb, void *jsonp)
{
    JsonStr *json = (JsonStr*) jsonp;
    size_t available = json->len - json->pos;
    size_t needed = size * nmemb;
    
    if (needed > available) {
        void *new = realloc(json->buffer, available+needed);
        if (new == NULL) {
            fprintf(stderr, "Failed to re-allocate more memory"); 
            return 0;
        }
        // TODO: Dangerous, mixing stack with dynamic
        json->buffer = new;
        json->len = json->len + needed;
    }
    
    memcpy((void*)&json->buffer[json->pos], ptr, needed);
    return needed;
}

int main (int argc, const char * argv[])
{
    // TODO: Add verbose/debug flag?
    _curl = curl_easy_init();
    if (_curl == NULL) {
        printf("Error loading cURL lib");
        return 1;
    }
    _filename = tmpnam(NULL);
    if (_filename == NULL) {
        printf("Failed creating temp cookiejar file");
        return 1;
    }
    printf("Created cookiejar: %s\n", _filename);
    
    _cookiejar = tmpfile();
    if (_cookiejar == NULL) {
        printf("Failed creating cookiejar");
        return 1;
    }
    
    JsonStr json;
    bzero(&json, sizeof(JsonStr));
    curl_easy_setopt(_curl, CURLOPT_URL, geturl(API_HASH));
    curl_easy_setopt(_curl, CURLOPT_USERAGENT, "Atrato CLI");
    curl_easy_setopt(_curl, CURLOPT_COOKIEJAR, _filename);
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, curlresponse);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &json);
    
    CURLcode res = curl_easy_perform(_curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "cURL failed: %s\n", curl_easy_strerror(res));
        return 1;
    }

    cleanup();    
    // TODO: What if buffer is zero? Clear memory?
    json.buffer[json.len+1] = '\0'; // Make sure End-Of-String is given
    printf("Raw response: %s\n\n", json.buffer);
    
    jsmn_parser parser;
    jsmn_init(&parser);
    
    jsmntok_t tokens[256];
    bzero(&tokens, sizeof(jsmntok_t)*256);
    jsmnerr_t response = jsmn_parse(&parser, json.buffer, tokens, 256);
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
            memcpy(msg, (const void*) json.buffer + pos.start, strsize);
            msg[strsize] = '\0';
            printf("%s\n", msg);
            free(msg);
        }
    }
    
    printf("EOF\n");
    return 0;
}

