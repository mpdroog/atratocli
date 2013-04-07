//
//  api.h
//  AtratoCLI
//
//  Created by M Droog on 2/21/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#ifndef AtratoCLI_api_h
#define AtratoCLI_api_h

int api_init(const char* ca_path);
int api_login(const char* ldapUser, const char* ldapPass);
void api_cleanup(void);
int api_credential_search(const char* query, int(*searchFn)(const char* key), int(*printFn)(const char* key, const char* value));
void api_credential_add(const char* hostname, const char* website, const char* username, const char* password, const char* comment);
#endif