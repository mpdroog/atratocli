//
//  api.h
//  AtratoCLI
//
//  Created by M Droog on 2/21/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#ifndef AtratoCLI_api_h
#define AtratoCLI_api_h

int api_init(void);
int api_login(const char* ldapUser, const char* ldapPass);
void api_cleanup(void);
void api_credential_search(const char* query);
void api_credential_add(const char* hostname, const char* website, const char* username, const char* password, const char* comment);
#endif