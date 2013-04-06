//
//  env.h
//  AtratoCLI
//
//  Created by M Droog on 3/28/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#ifndef AtratoCLI_env_h
#define AtratoCLI_env_h

char* env_homedir(void);
int env_isfile(const char* path);
int env_isdir(const char* path);
int env_createfolder(const char* path);
int env_unlink(const char* path);
size_t env_getpass(char** lineptr, size_t* n, FILE* stream);

#endif
