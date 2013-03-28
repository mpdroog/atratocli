//
//  stdin.c
//  AtratoCLI
//
//  Created by M Droog on 3/28/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "stdin.h"

char* stdin_input_read(const char* const msg, const int required, const char* const def_value, const int max_size)
{
    fprintf(stdout, "%s", msg);
    if (required) {
        fprintf(stdout, "*");
    }
    if (def_value != NULL) {
        fprintf(stdout, " [%s] ", def_value);        
    }
    fprintf(stdout, " : ");
    
    char* input = malloc(sizeof(char) * max_size +1);
    if (input == NULL) {
        return NULL;
    }
    bzero(input, sizeof(char) * max_size + 1);
    
    int pos = 0;
    int c = 0;
    for (;;) {
        c = fgetc(stdin);
        if (c == EOF || c == '\n') {
            break;
        }
        input[pos] = c;
        pos++;
    }
    return input;
}