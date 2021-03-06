//
//  http.h
//  AtratoCLI
//
//  Created by M Droog on 2/18/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#ifndef AtratoCLI_http_h
#define AtratoCLI_http_h

#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>

#include "global.h"

/**
 * Initialize module for HTTP-Requests
 * @return int 0 on success else higher
 */
int http_init(const char* ca_path);
/** Cleanup memory/connections for module */
void http_cleanup(void);

/**
 * Request a given URL.
 * @param const char* query The API 'method' to call
 * @return HttpResponse*|NULL
 */
HttpResponse* http_get(const char* query);

HttpResponse* http_post(const char* query);

void http_post_add(const char* key, const char* value);

void http_post_clear(void);
#endif
