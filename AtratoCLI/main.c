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
#include <termios.h>
#include <strings.h>
#include <string.h>

#include "api.h"
#include "http.h"
#include "db.h"
#include "strutils.h"
#include "stdin.h"
#include "env.h"
#include "table.h"

/** Global flag if additional logging is needed */
int verbose = 0;

int internal_find_key(const char* key);
int internal_find_value(const char* key, const char* value);
int internal_store_value(const char* key, const char* value);
int internal_store_key(const char* key);
// TODO: CONST!!
int internal_dbfind(void* unused, int argc, char** argv, char** colname);

static void main_credential_add(void);
static void main_credential_search(const char* search);
static void main_credential_cache(void);
static void main_db_init(void);
static char* internal_db_path(int include_file);
static char* internal_capath(void);
static int main_connect(const char* username, const char* password);

static char* internal_capath(void)
{
    char* settings = env_homedir();
    if (settings == NULL) {
        return NULL;
    }
    size_t size = sizeof(char) * (strlen(settings) + strlen(PATH_SETTINGS) + strlen(PATH_CABUNDLE) + 1);
    char* path = malloc(size);
    if (path == NULL) {
        free(settings);        
        return NULL;
    }
    bzero(path, size);
    
    strcpy(path, settings);
    strcat(path, PATH_SETTINGS);
    strcat(path, PATH_CABUNDLE);
    
    if (verbose) {
        fprintf(stdout, "Resolved CA Path to: %s\n", path);
    }
    free(settings);
    return path;
}

int main_connect(const char* username, const char* password)
{
    char* path = internal_capath();
    if (path == NULL) {
        printf("Failed finding ca path\n");
        return 1;
    }
    if (api_init(path) != 0) {
        printf("Failed loading API\n");
        return 1;
    }
    if (api_login(username, password) != 0) {
        return 1;
    }
    printf("Connected\n");
    // TODO: free path.....
    return 0;
}

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

int main (int argc, char* const argv[])
{    
    const char* class = NULL;
    const char* method = NULL;
    
    if (argc < 2) {
        internal_help(argv[0]);
        return 0;
    }
    
    static const struct option long_options[] = {
        {"verbose", no_argument, &verbose, 1},
        {"version", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'}
    };
    
    int option_index = 0;
    int c = -1;
    while ((c = getopt_long(argc, argv, "h", long_options, &option_index)) != -1) {
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
    size_t password_length = 0;

    for (;;) {
        // On first pass read username from environment
        if (username == NULL) {
            const char* env_username = getenv("USER");
            const size_t env_username_len = strlen(env_username);
            username = malloc(sizeof(char) * env_username_len +1);
            if (username == NULL) {
                fprintf(stderr, "Failed allocating memory\n");
                exit(1);
            }
            bzero(username, sizeof(char) * env_username_len + 1);
            strncpy(username, env_username, env_username_len);

        // On second pass read username from stdin
        } else {
            char* newuser = stdin_input_read("LDAP username:", 1, NULL, 200);
            free(username);
            username = newuser;
        }
        
        // Read password
        fprintf(stdout, "LDAP password for '%s':", username);
        fflush(stdout);
        env_getpass(&password, &password_length, stdin);
        fprintf(stdout, "\n");

        if (password == NULL) {
            fprintf(stderr, "Failed reading password\n");
            return 1;
        }
        
        // Stop iterating when username+password are there
        if (strlen(password) > 0) {
            break;
        }
        printf("\n");
    }
    
    char* setting_path = internal_db_path(0);
    if (verbose) {
        fprintf(stdout, "Settings folder: %s\n", setting_path);
    }
    if (env_createfolder(setting_path) == 1) {
        fprintf(stderr, "Failed creating settings folder\n");
        free(setting_path);
        return 1;
    }    

    if (strcmp(class, "credential") == 0 && strcmp(method, "search") == 0) {
        main_credential_search(argv[optind]);
    } else if (strcmp(class, "credential") == 0 && strcmp(method, "add") == 0) {
        main_connect(username, password);
        main_credential_add();
    } else if (strcmp(class, "db") == 0 && strcmp(method, "init") == 0) {
        main_connect(username, password);        
        main_db_init();
        main_credential_cache();        
    } else {
        fprintf(stdout, "Unsupported command %s::%s\n", class, method);
    }

    free(username);
    free(password);
    free(setting_path);
    api_cleanup();
    return 0;
}

int internal_find_key(const char* key)
{
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
        value = "NULL";
    }
    return table_analyze(key, value);
}

static char* internal_db_path(int include_file)
{
    char* home = env_homedir();
    if (home == NULL) {
        fprintf(stderr, "Failed resolving homedir\n");
        abort();
    }
    size_t size = sizeof(char) * (strlen(home) + strlen(PATH_SETTINGS) +1);
    if (include_file == 1) {
        size += sizeof(char) * strlen(PATH_SQLDB);
    }
    char* path = malloc(size);
    if (path == NULL) {
        fprintf(stderr, "Malloc failed\n");
        free(home);            
        abort();
    }
    bzero(path, size);
    strcpy(path, home);
    strcat(path, PATH_SETTINGS);
    if (include_file == 1) {
        strcat(path, PATH_SQLDB);
    }
    
    free(home);    
    return path;
}

static void main_db_init(void)
{
    char* path_db = internal_db_path(1);
    if (env_isfile(path_db) == 0) {
        if (env_unlink(path_db) == 1) {
            fprintf(stderr, "Failed deleting file %s\n", path_db);
            abort();
        }
    }    
    if (db_open(path_db) == 1) {
        free(path_db);
        fprintf(stderr, "Failed loading db\n");
        abort();
    }
    if (db_init() == 1) {
        free(path_db);        
        fprintf(stderr, "Failed initializing database\n");
        abort();
    }
    db_cleanup();
    if (verbose) {
        fprintf(stdout, "Successfully created database: %s\n", path_db);
    }
    free(path_db);
}

static void main_credential_cache(void)
{
    char* path = internal_db_path(0);
    if (verbose) {
        fprintf(stdout, "Settings folder: %s\n", path);
    }
    if (env_createfolder(path) == 1) {
        free(path);
        fprintf(stderr, "Failed creating settings file\n");
        abort();
    }
    free(path);
    path = internal_db_path(1);

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
    table_init();
    
    char* path = internal_db_path(1);
    if (db_open(path) == 1) {
        fprintf(stderr, "Failed opening db\n");        
        free(path);
        return;
    }
    int status = db_credential_find(&internal_dbfind, search);
    if (status == 1) {
        fprintf(stderr, "Failed reading credentials\n");
        free(path);        
        return;
    }
    
    table_print(stdout);
    table_cleanup();
    db_cleanup();
    free(path);    
}

int internal_dbfind(void* unused, int argc, char** argv, char** colname)
{
    UNUSED(unused);
    for (int i = 0; i < argc; i++) {
        const char* key = colname[i];
        const char* value = argv[i];
        if (internal_find_key(key)) {
            internal_find_value(key, value);
        }
    }
    return 0;
}

static void main_credential_add(void)
{
    char* hostname = stdin_input_read("Hostname", 1, NULL, 255);
    char* website = stdin_input_read("Website", 0, NULL, 255);
    char* username = stdin_input_read("Username", 1, NULL, 255);
    char* password = stdin_input_read("Password", 1, NULL, 255);
    char* comment = stdin_input_read("Comment", 1, NULL, 255);
    
    if (hostname == NULL || website == NULL || username == NULL || password == NULL || comment == NULL) {
        goto cleanup;
    }
    
    api_credential_add(hostname, website, username, password, comment);    
cleanup:
    free(hostname);
    free(website);
    free(username);
    free(password);
    free(comment);    
}

