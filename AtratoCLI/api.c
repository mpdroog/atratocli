//
//  api.c
//  AtratoCLI
//
//  Created by M Droog on 2/21/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include "api.h"
#include "http.h"
#include "json.h"
#include "md5.h"
#include "hash.h"

#define API_KEY "b9nIXMpr0AxBVy"
static int internal_checksalt(char* hash);
static int internal_request(char* type, char* query);

static int loginstage = 0;

int api_init(void) {
    if (http_init() != 0) {
        return 1;
    }
    return 0;
}

static int internal_checksalt(char* hash)
{
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
    
    return 0;
}

static int internal_request(char* type, char* query)
{
    json_init();
    HttpResponse *response = NULL;
    
    if (strcmp(type, "GET") == 0) {
        response = http_get(API_HASH);
    }
    else if(strcmp(type, "POST") == 0) {
        response = http_post(query);
    }
    if (response == NULL) {
        return 1;
    }
    json_parse(response->buffer);
    free(response);
    int error = json_readprimitive("result");
    if (error == 1) {
        return 1;
    }
    
    return 0;
}

/**
 * 1 - Get hash(salt) from server
 * 2 - Combine password with salt
 */
int api_login(void) {
    if (internal_request("GET", API_HASH) == 1) {
        return 1;
    }
    char* hash = json_readstring("result");
    if (internal_checksalt(hash) == 1) {
        return 1;
    }
    
    http_post_add("username", "api@atrato.com");
    http_post_add("timezone", "Europe/Amsterdam");

    hash_md5init();    
    hash_md5add(API_KEY);
    hash_md5add(hash);
    char* md5string = hash_md5str();
    http_post_add("key", md5string);
    
    if (internal_request("POST", API_LOGIN) == 1) {
        return 1;
    }    
    http_post_clear();
    
    free(md5string);
    free(hash);
    
    loginstage = 1;
    return 0;
}

void api_credential_search(const char* query)
{
    if (loginstage == 0) {
        return;
    }

    // LDAP...
    http_post_add("username", "mark");    
    http_post_add("password", "xx");    
    
    http_post_add("search", "atrato");
    if (internal_request("POST", API_CREDENTIALS) == 1) {
        return;
    }
    http_post_clear();
    // TODO: Return jsmn? need to return array of data
    
}

void api_cleanup(void) {
    http_cleanup();
}
