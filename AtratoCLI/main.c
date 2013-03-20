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

#include "api.h"
#include "http.h"

static const char* internal_input_read(const char* msg, int required, const char* def_value, int max_size);
static void main_credential_add(void);

static const char* internal_input_read(const char* msg, int required, const char* def_value, int max_size)
{
    fprintf(stdout, "%s", msg);
    if (required) {
        fprintf(stdout, "*");
    }
    if (def_value != NULL) {
        fprintf(stdout, " [%s] ", def_value);        
    }
    fprintf(stdout, " : ");
    
    char* input = malloc(sizeof(char) * max_size + 1);
    if (input == NULL) {
        return 0;
    }
    char* search = malloc(sizeof(char) * 4); // Assumption..
    if (search == NULL) {
        free(input);
        return 0;
    }
    bzero(search, sizeof(char) * 4);
    sprintf(search, "%%%ds", max_size);
    
    if (scanf(search, input) != 1) {
        free(input);
        free(search);
        return 0;
    }
    
    return input;
}

int main (int argc, const char* argv[])
{
    char* search = NULL;
    int add = 0;
    
    if (argc < 2) {
        fprintf(stdout, "atratocli, version 1.0\n\n");
        fprintf(stdout, "usage: atratocli [-a][-c value]\n");
        fprintf(stdout, "   c searchvalue : Search credentials\n");
        fprintf(stdout, "   a             : Add credential\n");
        return 0;
    }
    
    int c = -1;
    while ((c = getopt(argc, (char* const*)argv, "ac:")) != -1) {
        switch (c) {
            case 'a':
                add = 1;
                break;
            case 'c':
                search = optarg;
                break;
            case '?':
                if (optopt == 'c') {
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                } else if (isprint (optopt)) {
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                return 1;
            default:
                abort();
        }
    }
    
    char *username = getenv("USER");
    printf("LDAP Username: ");
    printf("%s\n", username);
    const char* password = internal_input_read("LDAP Password", 1, NULL, 255);
    if (password == NULL) {
        free((void*)password);
        return 1;
    }    
    printf("\n");
    
    if (api_init() != 0) {
        printf("Failed loading API\n");
        free((void*)password);
        return 1;
    }
    if (api_login(username, password) != 0) {
        printf("Failed login\n");
        free((void*)password);        
        return 1;
    }
    printf("Logged in\n");

    if (search != NULL) {
        api_credential_search(search);
    } else if (add == 1) {
        main_credential_add();
    } else {
        fprintf(stdout, "No instruction given\n");
    }
    api_cleanup();
    return 0;
}

static void main_credential_add(void)
{
    const char* hostname = internal_input_read("Hostname", 1, NULL, 255);
    const char* website = internal_input_read("Website", 0, NULL, 255);
    const char* username = internal_input_read("Username", 1, NULL, 255);
    const char* password = internal_input_read("Password", 1, NULL, 255);
    const char* comment = internal_input_read("Comment", 1, NULL, 255);
    
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

