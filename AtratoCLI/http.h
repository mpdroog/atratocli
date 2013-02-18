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
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>

#include "global.h"

#define API_URL "http://atrato.cc/api.php"
#define API_HASH "?c=authentication&m=hash"
#define API_LOGIN "?c=authentication&m=login"
#define API_LIST "?c=general&m=list"

/**
 * Initialize module for HTTP-Requests
 * @return int 0 on success else higher
 */
int http_init(void);
/** Cleanup memory/connections for module */
void http_cleanup(void);

/**
 * Request a given URL.
 * @param const char* query The API 'method' to call
 * @return HttpResponse*|NULL
 */
HttpResponse* http_get(const char* query);
#endif
