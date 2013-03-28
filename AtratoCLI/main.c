//
//  main.c
//  AtratoCLI
//
//  Created by M Droog on 2/16/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>

#include "api.h"
#include "http.h"
#include "db.h"
#include "strutils.h"
#include "stdin.h"
#include "env.h"

int verbose = 0;

int internal_find_key(const char* key);
int internal_find_value(const char* key, const char* value);
int internal_store_value(const char* key, const char* value);
int internal_store_key(const char* key);

static void main_credential_add(void);
static void main_credential_search(const char* search);
static void main_credential_cache(void);
static void main_db_init(void);

static void internal_canonical_name(void)
{
    fprintf(stdout, "atratocli, version 1.0\n");
    fprintf(stdout, "bugs can be reported to mpdroog@rootdev.nl\n\n");
}

static void internal_help(const char* name)
{
    fprintf(stdout, "usage: %s [--version] [--help] [--verbose]\n", name);
    fprintf(stdout, "                 <command> [<args>]\n\n");
    
    fprintf(stdout, "\nThe most commonly used commands:\n");    
    fprintf(stdout, "   credential search <value> Search credentials\n");
    fprintf(stdout, "   credential add            Add credential\n");    
}

int main (int argc, const char* argv[])
{
    const char* class = NULL;
    const char* method = NULL;
    
    if (argc < 2) {
        internal_help(argv[0]);
        return 0;
    }
    
    static struct option long_options[] = {
        {"verbose", no_argument, &verbose, 1},
        {"version", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'}
    };
    
    int option_index = 0;
    int c = -1;
    while ((c = getopt_long(argc, (char* const*)argv, "h", long_options, &option_index)) != -1) {
        switch (c) {
            case 'v':
                internal_canonical_name();
                return 0;
            case 'h':
                internal_help(argv[0]);
                return 0;
            case '?':
                // Already handled by getopt_long
                return 1;
        }
    }

    if (optind+2 > argc) {
        internal_help(argv[0]);
        return 1;
    }
    {
        class = argv[optind];
        method = argv[optind+1];
        optind += 2;
    }

    char* username = NULL;
    char* password = NULL;
    for (;;) {
        if (username == NULL) {
            char* tmpUsername = getenv("USER");
            username = malloc(sizeof(char) * 200);
            if (username == NULL) {
                fprintf(stderr, "Failed allocating memory\n");
                exit(1);
            }
            strncpy(username, tmpUsername, 200);            
        } else {
            char* newuser = stdin_input_read("LDAP username:", 1, NULL, 200);
            if (username != NULL) {
                free(username);
            }
            username = newuser;
        }
        fprintf(stdout, "LDAP password for '%s':", username);
        fflush(stdout);
        password = getpass("");
        if (password == NULL) {
            fprintf(stderr, "Failed reading password\n");
            return 1;
        }
        if (strlen(password) > 0) {
            break;
        }
        printf("\n");
    }
    
    if (api_init() != 0) {
        printf("Failed loading API\n");
        return 1;
    }
    if (api_login(username, password) != 0) {
        return 1;
    }
    printf("Connected\n");
    
    if (strcmp(class, "credential") == 0 && strcmp(method, "search") == 0) {
        main_credential_search(argv[optind]);
    } else if (strcmp(class, "credential") == 0 && strcmp(method, "add") == 0) {
        main_credential_add();
    } else if (strcmp(class, "db") == 0 && strcmp(method, "init") == 0) {
        main_db_init();
        main_credential_cache();        
    } else {
        fprintf(stdout, "Unsupported command %s::%s\n", class, method);
    }

    api_cleanup();
    return 0;
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
    if (value == NULL) {
        printf("%-30s", "NULL");
    }
    else {
        char *decoded = str_replace("\\/", "/", value);
        char *max = str_substr(0, 28, decoded);
        printf("%-30s", max);
        free(max);
        free(decoded);
    }
    
    return 0;
}

static void main_db_init(void)
{
    // TODO: Delete file?
    const char* const home = env_homedir();
    if (home == NULL) {
        fprintf(stderr, "Failed resolving homedir\n");
        abort();
    }
    char* path = malloc(sizeof(char) * 256); // Assumption
    sprintf(path, "%s/%s", home, ".aci/");
    free((void*)home);
    
    if (verbose) {
        fprintf(stdout, "Settings folder: %s\n", path);
    }
    if (env_createfolder(path) == 1) {
        fprintf(stderr, "Failed creating settings folder\n");
        return ;
    }
    strcat(path, "at_ccc.db");
    if (env_isfile(path) == 0) {
        if (env_unlink(path) == 1) {
            fprintf(stderr, "Failed deleting file %s\n", path);
            return;
        }
    }    
    if (db_open(path) == 1) {
        fprintf(stderr, "Failed loading db\n");
        return ;        
    }
    if (db_init() == 1) {
        fprintf(stderr, "Failed initializing database\n");
    }
    db_cleanup();
    fprintf(stdout, "Successfully created database: %s\n", path);
    free(path);
}

static void main_credential_cache(void)
{
    const char* const home = env_homedir();
    if (home == NULL) {
        fprintf(stderr, "Failed resolving homedir\n");
        abort();
    }
    char* path = malloc(sizeof(char) * 256); // Assumption
    sprintf(path, "%s/%s", home, ".aci/");
    free((void*)home);

    if (verbose) {
        fprintf(stdout, "Settings folder: %s\n", path);
    }
    if (env_createfolder(path) == 1) {
        free(path);
        fprintf(stderr, "Failed creating settings file\n");
        return ;
    }
    strcat(path, "at_ccc.db");
    if (db_open(path) == 1) {
        free(path);
        fprintf(stderr, "Failed loading cache (SQLDB)\n");
        return ;        
    }
    free(path);
    //db_init(); // Ignore init errr
    if (db_statement() == 1) {
        fprintf(stderr, "Failed compiling statement\n");
        return;
    }
    fprintf(stdout, "Writing credentials to cache\n");    
    int status = api_credential_search("", &internal_store_key, &internal_store_value);
    if (status == 1) {
        fprintf(stderr, "Failed reading credentials\n");
        return;
    }
    fprintf(stdout, "Updated cache\n");
    db_statement_store();
    db_statement_next();
    db_cleanup();
}

int internal_store_key(const char* key)
{
    static int first = 0;
    if (first == 0 && strcmp(key, "credential_hostname") == 0) {
        first = 1;
    } else if (strcmp(key, "credential_hostname") == 0){
        if (db_statement_store() == 1) {
            fprintf(stderr, "Failed saving statement\n");
            abort();
        }
        if (db_statement_next() == 1) {
            fprintf(stderr, "Failed starting next statement\n");
            abort();
        }
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


int internal_store_value(const char* key, const char* value)
{
    if (value == NULL) {
        if (db_statement_null(key) == 1) {
            fprintf(stderr, "Failed writing %s to cache\n", value);
            abort();
        }
        return 0;
    }
    if (db_statement_string(key, value) == 1) {
        fprintf(stderr, "Failed writing %s to cache\n", value);
        abort();
    }
    return 0;
}

static void main_credential_search(const char* search)
{
    printf("%-30s%-30s%-30s%-30s\n", "Hostname", "Website", "Username", "Password");
    for (int i = 0; i < 111; i++) {
        printf("-");
    }
    printf("\n");

    int status = api_credential_search(search, &internal_find_key, &internal_find_value);
    if (status == 1) {
        fprintf(stderr, "Failed reading credentials\n");
        return;
    }
    printf("\n");
}

static void main_credential_add(void)
{
    const char* hostname = stdin_input_read("Hostname", 1, NULL, 255);
    const char* website = stdin_input_read("Website", 0, NULL, 255);
    const char* username = stdin_input_read("Username", 1, NULL, 255);
    const char* password = stdin_input_read("Password", 1, NULL, 255);
    const char* comment = stdin_input_read("Comment", 1, NULL, 255);
    
    if (hostname == NULL || website == NULL || username == NULL || password == NULL || comment == NULL) {
        goto cleanup;
    }
    
    api_credential_add(hostname, website, username, password, comment);    
cleanup:
    free((void*)hostname);
    free((void*)website);
    free((void*)username);
    free((void*)password);
    free((void*)comment);    
}

