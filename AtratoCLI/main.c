//
//  main.c
//  AtratoCLI
//
//  Created by M Droog on 2/16/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include <unistd.h>
#include <stdio.h>

#include "api.h"
#include "http.h"

#define PASSWORD_LDAP_LENGTH 255

int main (int argc, const char * argv[])
{
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
        return 1;
    }
    if (api_login(username, password) != 0) {
        printf("Failed login\n");
        return 1;
    }
    printf("Logged in\n");

    api_credential_search(API_CREDENTIALS);
    api_cleanup();
    return 0;
}

