//
//  table.c
//  AtratoCLI
//
//  Created by M Droog on 4/1/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>

#include "table.h"
#include "strutils.h"

extern int verbose;

typedef struct table_col {
    char* key;
    int size;
    
    struct table_col* next;
} table_col;

typedef struct table_values {
    const char* key;
    const char* value;
    
    struct table_values* next;
} table_values;


static unsigned int _columns = 0;
static unsigned short _width = 0;
static table_col* _col_first = NULL;
static table_values* _values_first = NULL;
static table_values* _values_last = NULL;

static unsigned short internal_get_width(void);
static int internal_column_update_size(const char* const key, const char* const value);
static int internal_row_store(const char* const key, const char* const value);

static unsigned short internal_get_width(void)
{
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        return w.ws_col;
    } else {
        return 1;
    }
}

int table_init(void)
{
    _width = internal_get_width();
    return _width;
}

int table_analyze(const char* const key, const char* const value)
{
    int ret = 0;
    ret = internal_column_update_size(key, value);
    if (ret != 0) {
        return 1;
    }
    
    return internal_row_store(key, value);
}

static int internal_row_store(const char* const key, const char* const value)
{    
    table_values* new = malloc(sizeof(table_values));
    if (new == NULL) {
        if (verbose) {
            fprintf(stderr, "malloc failed\n");
        }
        return 1;
    }
    bzero(new, sizeof(table_values));
    // Add key
    {
        new->key = malloc(sizeof(char) * strlen(key) +1);
        if (new->key == NULL) {
            return 1;
        }
        strcpy(new->key, key);
    }
    // Add value
    {
        new->value = malloc(sizeof(char) * strlen(value) +1);
        if (new->value == NULL) {
            return 1;
        }
        strcpy(new->value, value);
    }
    
    if (_values_last == NULL) {
        _values_first = new;
        _values_last = new;
    } else {
        _values_last->next = new;
        _values_last = new;
    }
    
    return 0;
}

static int internal_column_update_size(const char* const key, const char* const value)
{
    int value_len = (int) strlen(value);
    
    // Look in existing
    table_col* row_last = NULL;
    {
        table_col* row = _col_first;
        while (row != NULL) {
            if (strcmp(row->key, key) == 0) {
                if (value_len > row->size) {
                    // Updating maxlen
                    row->size = value_len;
                }
                return 0;
            }
            row_last = row;
            row = row->next;
        }
    }
        
    // When here no existing found, add!
    {
        table_col* row = malloc(sizeof(table_col));
        if (row == NULL) {
            if (verbose) {
                fprintf(stderr, "malloc failed\n");
            }
            return 1;
        }
        bzero(row, sizeof(table_col));
        
        row->key = malloc(sizeof(char) * strlen(key) +1);
        if (row->key == NULL) {
            return 1;
        }
        strcpy(row->key, (const char*)key);
        row->size = value_len;
        
        if (_col_first == NULL) {
            _col_first = row;
        } else {
            row_last->next = row;
        }
        _columns++;
    }
    
    return 0;
}

int table_print(FILE* output)
{
    // Calculate widths
    // _width = screen size
    // _columns = column count

    // Print header
    {
        table_col* col = _col_first;
        while (col != NULL) {
            char* msg = str_substr(0, col->size, col->key);
            fprintf(output, "%-*s", col->size+1, msg);
            free(msg);
            col = col->next;
        }
        fprintf(output, "\n");
    }
    
    table_values* value = _values_first;
    int pos = 0;
    while (value != NULL) {
        if (pos > 0 && pos % _columns == 0) {
            fprintf(output, "\n");
        }
        // Dirty
        table_col* col = _col_first;
        int size = 0;
        while (col != NULL) {
            if (strcmp(col->key, value->key) == 0) {
                size = col->size;
                break;
            }            
            col = col->next;
        }
        if (size == 0) {
            fprintf(stderr, "Failed finding index\n");
            return 1;
        }
        // Dirty end
        char* msg = str_substr(0, size, value->value);
        fprintf(output, "%-*s", size+1, msg);
        free(msg);
        
        value = value->next;
        pos++;
    }

    fprintf(output, "\n");    
    return 0;
}

void table_cleanup(void)
{
    // Free our column linkedlist
    {
        table_col* row = _col_first;
        while (row != NULL) {
            table_col* next = row->next;
            free((void*) row->key);
            free(row);
            row = next;
        }
    }
    
    // Free our column linkedlist
    {
        table_values* row = _values_first;
        while (row != NULL) {
            table_values* next = row->next;
            free((void*) row->key);            
            free(row);
            row = next;
        }
    }    
}
