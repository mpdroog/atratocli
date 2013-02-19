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

#endif