//
//  global.h
//  AtratoCLI
//
//  Created by M Droog on 2/18/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#ifndef AtratoCLI_global_h
#define AtratoCLI_global_h

/** Default response structure */
struct HttpResponse {
    char *buffer;
    size_t len;
    size_t pos;
};
typedef struct HttpResponse HttpResponse;

#define VERSION "0.1"
#define API_URL "https://atrato.cc/api.php"
//#define API_URL "http://dev.ccc.atrato/api.php"
#define API_HASH "?c=authentication&m=hash"
#define API_LOGIN "?c=authentication&m=login"
#define API_CREDENTIALS "?c=credentials&m=credentials"
#define API_CREDENTIAL_ADD "?c=credentials&m=add"

#define PATH_SETTINGS ".aci/"
#define PATH_SQLDB "at_ccc.db"
#define PATH_CABUNDLE "ca-bundle.crt"

/**
 * At least 128bit
 * @see http://www.openssl.org/docs/apps/ciphers.html
 */
#define SSL_CIPHER_LIST "HIGH"

/**
 * Ignore unused argument(s)
 * @see http://stackoverflow.com/questions/3599160/unused-parameter-warnings-in-c-code
 */
#define UNUSED(x) (void)(x)
#define SAFE_SIZET(x) { (x) >= 0 ? (size_t) (x) : 0 }

#endif