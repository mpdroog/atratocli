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

int main (int argc, const char * argv[])
{
    if (api_init() != 0) {
        printf("Failed loading API\n");
        return 1;
    }
    if (api_login() != 0) {
        printf("Failed login\n");
        return 1;
    }
    printf("Logged in\n");
    
    api_credential_search(API_CREDENTIALS);
    api_cleanup();
    return 0;
}

