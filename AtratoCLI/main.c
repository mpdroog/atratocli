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

#include "http.h"
#include "json.h"
#include "md5.h"

#define API_KEY "b9nIXMpr0AxBVy"

int main (int argc, const char * argv[])
{
    // TODO: Add verbose/debug flag?
    if (http_init() != 0) {
        return 1;
    }
        
    HttpResponse *json = http_get(API_HASH);
    if (json == NULL) {
        http_cleanup();        
        return 1;
    }
    //printf("Raw response: %s\n\n", json->buffer);
    
    json_parse(json->buffer);
    char* hash = json_readnext("result");
    if (hash == NULL) {
        fprintf(stderr, "Received no hash from server");
        http_cleanup();        
        return 1;
    }
    if (hash == NULL || strlen(hash) != 32) {
        free(hash);
        http_cleanup();        
        fprintf(stderr, "Received invalid hash from server");
        return 1;
    }
    
    http_post_add("username", "api@atrato.com");
    http_post_add("timezone", "Europe/Amsterdam");
    
    unsigned char digest[16];
    md5_context md5;
    md5_starts(&md5);
    md5_update(&md5, (unsigned char*)API_KEY, strlen(API_KEY));
    md5_update(&md5, (unsigned char*)hash, strlen(hash));
    md5_finish(&md5, digest);    

    char md5string[33];
    for(int i = 0; i < 16; ++i) {
        sprintf(&md5string[i*2], "%02x", (unsigned int)digest[i]);
    }
    printf("MD5: %s", md5string);
    
    http_post_add("key", md5string);
    HttpResponse *auth = http_post(API_LOGIN);
    printf("Raw response: %s\n\n", auth->buffer);
    
    http_cleanup();
    free(hash);
    free(json);
    free(auth);
    printf("EOF\n");
    return 0;
}

