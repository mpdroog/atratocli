//
//  strutils.c
//  AtratoCLI
//
//  Created by M Droog on 3/9/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "strutils.h"
#include "global.h"

char *str_substr(int start, int length, const char* subject)
{
    size_t inputlen = SAFE_SIZET(length - start);
    size_t len = strlen(subject);
    if (len > inputlen) {
        len = inputlen;
    }
    
    size_t required = sizeof(char) * len +1;
    char* sub = malloc(required);
    if (sub == NULL) {
        return NULL;
    }
    bzero(sub, required);
    strncpy(sub, subject+start, required-1);
    sub[required] = '\0';
    return sub;
}

/**
 * Search and replace for an infinit match of search-results.
 *
 * const char* search Character-combination to search
 * const char* replace Character-combination that replaces search
 * const char* subject Message to search/replace character in
 * @see http://php.net/manual/en/function.str-replace.php Based on str_replace in PHP
 * @see http://www.codekites.com/str_replace-for-c/ For origin of code
 * @return char* Result on success, NULL on failure 
 **/
char *str_replace(const char* search, const char* replace, const char* subject)
{
    const char* old = NULL;
	char *p = NULL, *new_subject = NULL;
	size_t c = 0, search_size = strlen(search);
	
	//Count how many occurences
	for(p = strstr(subject , search) ; p != NULL ; p = strstr(p + search_size , search))
	{
		c++;
	}
	
	//Final size
	c = ( strlen(replace) - search_size )*c + strlen(subject) +1;
	
	//New subject with new size
	new_subject = malloc( sizeof(char) * c );
    if (new_subject == NULL) {
        return NULL;
    }
    bzero(new_subject, sizeof(char) * c );
	
	//The start position
	old = subject;
	
	for(p = strstr(subject , search) ; p != NULL ; p = strstr(p + search_size , search))
	{
        long len = p - old;
        if (len < 0) {
            // Big trouble!!
            return NULL;
        }
		//move ahead and copy some text from original subject , from a certain position
		strncpy(new_subject + strlen(new_subject), old, (size_t)len);
		
		//move ahead and copy the replacement text
		strcpy(new_subject + strlen(new_subject), replace);
		
		//The new start position after this search match
		old = p + search_size;
	}
	
	//Copy the part after the last search match
	strcpy(new_subject + strlen(new_subject) , old);
	
	return new_subject;
}