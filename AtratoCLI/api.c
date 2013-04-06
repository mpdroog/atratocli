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
static int internal_request(const char* type, const char* query);

static int loginstage = 0;
static const char* _ldapUser = NULL;
static const char* _ldapKey = NULL;

int api_init(void) {
    if (http_init() != 0) {
        return 1;
    }
    return 0;
}

static int internal_request(const char* type, const char* query)
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
    if (json_parse(response->buffer) == 0) {
        return 1;
    }
    
    free(response);
    int error = json_readprimitive("result");
    if (error == 1) {
        fprintf(stderr, "Server Error: %s\n", json_readstring("reason"));
        return 1;
    }
    
    return 0;
}

/**
 * 1 - Get hash(salt) from server
 * 2 - Combine password with salt
 */
int api_login(const char* ldapUser, const char* ldapPass) {
    if (internal_request("GET", API_HASH) == 1) {
        return 1;
    }
    char* hash = json_readstring("result");
    // Validate hash as MD5
    {
        if (hash == NULL) {
            fprintf(stderr, "Received no hash from server");
            http_cleanup();        
            return 1;
        }
        if (strlen(hash) != 32) {
            free(hash);
            http_cleanup();        
            fprintf(stderr, "Received invalid hash from server");
            return 1;
        }
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
    
    _ldapUser = ldapUser;
    _ldapKey = ldapPass;
    
    loginstage = 1;
    return 0;
}

int api_credential_search(const char* query, int(*searchFn)(const char* key), int(*printFn)(const char* key, const char* value))
{
    if (loginstage == 0) {
        return 1;
    }

    http_post_add("username", _ldapUser);    
    http_post_add("password", _ldapKey);    
    
    http_post_add("search", query);
    if (internal_request("POST", API_CREDENTIALS) == 1) {
        return 1;
    }
    http_post_clear();

    return json_array_search("result", searchFn, printFn);
}

void api_credential_add(const char* hostname, const char* website, const char* username, const char* password, const char* comment)
{
    if (loginstage == 0) {
        return;
    }
    
    // LDAP...
    http_post_add("username", _ldapUser);    
    http_post_add("password", _ldapKey);    
    
    http_post_add("hostname", hostname);
    http_post_add("website", website);
    http_post_add("username", username);
    http_post_add("value", password);
    http_post_add("comment", comment);

    if (internal_request("POST", API_CREDENTIAL_ADD) == 1) {
        http_post_clear();        
        return;
    }
    http_post_clear();

    int error = json_readprimitive("result");
    if (error == 1) {
        fprintf(stdout, "Failed storing credential\n");
    }
}

void api_cleanup(void) {
    http_cleanup();
}
