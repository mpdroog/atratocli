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
static const char* _ldapUser = NULL;
static const char* _ldapKey = NULL;

int internal_find_key(const char* key);
int internal_find_value(const char* key, const char* value);

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
    if (strlen(hash) != 32) {
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
    if (json_parse(response->buffer) == 0) {
        return 1;
    }
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
int api_login(const char* ldapUser, const char* ldapPass) {
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
    
    _ldapUser = ldapUser;
    _ldapKey = ldapPass;
    
    loginstage = 1;
    return 0;
}

void api_credential_search(const char* query)
{
    if (loginstage == 0) {
        return;
    }

    // LDAP...
    http_post_add("username", _ldapUser);    
    http_post_add("password", _ldapKey);    
    
    http_post_add("search", query);
    if (internal_request("POST", API_CREDENTIALS) == 1) {
        return;
    }
    http_post_clear();

    printf("%-30s%-30s%-30s%-30s\n", "Hostname", "Website", "Username", "Password");
    for (int i = 0; i < 111; i++) {
        printf("-");
    }
    printf("\n");
    json_array_search("result", &internal_find_key, &internal_find_value);
    printf("\n");
}

int internal_find_key(const char* key)
{
    if (strcmp(key, "credential_hostname") == 0) {
        printf("\n");
    }
    if (
        strcmp(key, "credential_hostname") == 0 ||
        strcmp(key, "credential_website") == 0 ||
        strcmp(key, "credential_username") == 0 ||
        strcmp(key, "credential_value") == 0
        ) {
        return 1;
    }
    return 0;
}

int internal_find_value(const char* key, const char* value)
{
    char *decoded = str_replace("\\/", "/", value);
    char *max = str_substr(0, 28, decoded);
    printf("%-30s", max);
    free(max);
    free(decoded);
    
    return 0;
}

void api_cleanup(void) {
    http_cleanup();
    // No need to free _ldapUser
    free((void*)_ldapKey);
}
