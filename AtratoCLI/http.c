//
//  http.c
//  AtratoCLI
//
//  Created by M Droog on 2/18/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include "http.h"

/** cURL Handler */
static CURL *_curl;
/** Filepointer where cookies can be stored */
static FILE *_cookiejar;
/** Path on filesystem to cookiejar */
static char* _filename;
/** Path on filesystem to CA (Certificate Authority) list */
static const char* _capath = NULL;

/** Data to write to server */
static char* _postFields;
/** Amount of fields */
static int _postCount;

extern int verbose;

/**
 * Construct URL to request from server.
 * Warning: Don't forget to free returned URL!
 *
 * @param const char* query Query to append
 * @return const char* API_URL + query
 */
static char* internal_geturl(const char* query);

/**
 * Read cURL response and migrate to own datastructure.
 * @param void *ptr Memory location to start reading from
 * @param size_t size Size of every line
 * @param size_t nmemb Amount of line
 * @param void *stream Own datastructure assigned in CURLOPT_WRITEDATA
 * @return int Amount of read bytes
 */
static size_t internal_curlresponse(void *ptr, size_t size, size_t nmemb, void *stream);

int http_init(const char* ca_path)
{
    _capath = ca_path;
    _curl = curl_easy_init();
    if (_curl == NULL) {
        fprintf(stderr, "Error loading cURL lib\n");
        return 1;
    }
    _filename = tmpnam(NULL);
    if (_filename == NULL) {
        fprintf(stderr, "Failed creating temp cookiejar file\n");
        return 1;
    }
    if (verbose) {
        printf("Created cookiejar: %s\n", _filename);
    }
    
    _cookiejar = tmpfile();
    if (_cookiejar == NULL) {
        fprintf(stderr, "Failed creating cookiejar\n");
        return 1;
    }
    return 0;
}

void http_cleanup(void)
{
    curl_easy_cleanup(_curl);
    fclose(_cookiejar);
    free(_filename);
}

HttpResponse* http_post(const char* query)
{
    HttpResponse *response = malloc(sizeof(HttpResponse));
    if (response == NULL) {
        return NULL;
    }
    bzero(response, sizeof(HttpResponse));

    char *url = internal_geturl(query);
    if (url == NULL) {
        fprintf(stderr, "Failed getting URL for %s", query);
        free(response);
        free(url);
        return NULL;
    }
    if (verbose) {
        fprintf(stdout, ">> POST %s\n", url);
    }
    curl_easy_setopt(_curl, CURLOPT_URL, url);
    curl_easy_setopt(_curl, CURLOPT_USERAGENT, "Atrato CLI");
    curl_easy_setopt(_curl, CURLOPT_COOKIEJAR, _filename);
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, internal_curlresponse);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, response);
    curl_easy_setopt(_curl, CURLOPT_POST, 1L);
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, _postFields);

    // SSL flags
    curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(_curl, CURLOPT_CAINFO, _capath);
    curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(_curl, CURLOPT_SSL_CIPHER_LIST, SSL_CIPHER_LIST);
    
    CURLcode res = curl_easy_perform(_curl);
    free((void*)url);
    
    if (res != CURLE_OK) {
        fprintf(stderr, "HTTP failed: %s\n", curl_easy_strerror(res));
        free(response);
        return NULL;
    }
    if (response->len == 0) {
        fprintf(stderr, "HTTP Received nothing from webserver\n");
        free(response);
        return NULL;
    }
    
    response->buffer[response->pos] = '\0'; // Make sure End-Of-String is given
    if (verbose) {
        fprintf(stdout, "<< %s\n", response->buffer);
    }
    return response;    
}

HttpResponse* http_get(const char* query)
{
    HttpResponse *response = malloc(sizeof(HttpResponse));
    bzero(response, sizeof(HttpResponse));
    
    char *url = internal_geturl(query);
    if (url == NULL) {
        free(response);
        return NULL;
    }
    if (verbose) {
        fprintf(stdout, ">> GET %s\n", url);
    }

    curl_easy_setopt(_curl, CURLOPT_URL, url);
    curl_easy_setopt(_curl, CURLOPT_USERAGENT, "Atrato CLI");
    curl_easy_setopt(_curl, CURLOPT_COOKIEJAR, _filename);
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, internal_curlresponse);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, response);

    // SSL flags
    curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(_curl, CURLOPT_CAINFO, _capath);
    curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(_curl, CURLOPT_SSL_CIPHER_LIST, SSL_CIPHER_LIST);
    
    CURLcode res = curl_easy_perform(_curl);
    free(url);
    if (res != CURLE_OK) {
        fprintf(stderr, "HTTP failed: %s\n", curl_easy_strerror(res));
        free(response);
        return NULL;
    }
    if (response->len == 0) {
        fprintf(stderr, "HTTP received nothing from webserver\n");
        free(response);
        return NULL;
    }
    
    response->buffer[response->pos] = '\0'; // Make sure End-Of-String is given
    if (verbose) {
        fprintf(stdout, "<< %s\n", response->buffer);
    }    
    return response;
}

void http_post_add(const char* key, const char* value)
{
    if (_postCount == 0) {
        // TODO: Hardcoded..
        _postFields = malloc(sizeof(char)*2096);
        bzero((void*)_postFields, sizeof(char)*2096);
    }
    
    if (_postCount != 0) {
        strcat(_postFields, "&");
    }
    strcat(_postFields, key);
    strcat(_postFields, "=");
    strcat(_postFields, value);
    _postCount++;
}

void http_post_clear(void)
{
    _postCount = 0;
    bzero((void*)_postFields, sizeof(char)*2096);
}

char* internal_geturl(const char* query)
{
    if (256 - strlen(query) + strlen(API_URL) < 1) {
        fprintf(stderr, "URL too long!\n");
        return NULL;
    }
    char *url = malloc(sizeof(char)*256);
    strcpy(url, API_URL);
    strcat(url, query);
    return url;
}

static size_t internal_curlresponse(void *ptr, size_t size, size_t nmemb, void *jsonp)
{
    HttpResponse *response = (HttpResponse*) jsonp;
    size_t available = response->len - response->pos;
    if (available != 0) {
        available--;
    }
    size_t needed = size * nmemb;
    
    if ((needed/sizeof(char)) > available) {
        void *new = realloc(response->buffer, sizeof(char) * (response->len + needed +1));
        if (new == NULL) {
            if (response->buffer != NULL) {
                free(response->buffer);
            }
            fprintf(stderr, "Failed to re-allocate more memory\n"); 
            exit(1);
        }
        response->buffer = new;
        response->len += needed +1;
    }
    
    memcpy((void*)&response->buffer[response->pos], ptr, needed);
    response->pos += needed;
    return needed;
}