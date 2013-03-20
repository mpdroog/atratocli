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

#define PASSWORD_LDAP_LENGTH 255

int main (int argc, const char* argv[])
{
    char* search = NULL;
    int add = 0;
    
    if (argc < 2) {
        fprintf(stdout, "No CLI arguments\n");
        fprintf(stdout, "-c <searchvalue> -- Search credentials\n");
        fprintf(stdout, "-a -- Add credential\n");
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
    char *password = malloc(sizeof(char) * PASSWORD_LDAP_LENGTH + 1);
    if (username == NULL) {
        free(password);
        return -1;
    }
    printf("LDAP Username: ");
    printf("%s\n", username);
    printf("LDAP Password: ");
    
    if (scanf("%255s", password) != 1) {
        free(password);
        return -1;
    }
    printf("\n");
    
    if (api_init() != 0) {
        printf("Failed loading API\n");
        free(password);
        return 1;
    }
    if (api_login(username, password) != 0) {
        printf("Failed login\n");
        free(password);        
        return 1;
    }
    printf("Logged in\n");

    if (search != NULL) {
        // Search for given credential
        api_credential_search(search);
    } else if (add == 1) {
        // Add credential
        printf("Hostname*: ");
        printf("Website: ");
        printf("Username*:");
        printf("Password:");
        printf("Comment");
        //if (scanf("%255s", password) != 1) {
        
    } else {
        fprintf(stdout, "No instruction given\n");
    }
    api_cleanup();
    return 0;
}

