//
//  env.c
//  AtratoCLI
//
//  Created by M Droog on 3/28/13.
//  Copyright 2013 Rootdev Enterprise. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>
#include <errno.h>

#include "env.h"

extern int verbose;

const char* const env_homedir(void)
{
    struct passwd *pw = getpwuid(getuid());
    if (pw == NULL) {
        return NULL;
    }
    // +1 for \0 and \1 for the trailing slash /
    size_t size = sizeof(char) * (strlen(pw->pw_dir) +2);
    char* dir = malloc(size);
    if (dir == NULL) {
        return NULL;
    }
    bzero(dir, size);
    strcpy(dir, pw->pw_dir);
    strcat(dir, "/");
    return dir;
}

/**
 * -1 = Error on I/O
 *  0 = Folder exists
 *  1 = Path not directtory
 *  2 = Not exist
 */
int env_isdir(const char* const path)
{
    struct stat sb = {};
    int res = stat(path, &sb);
    if (res == -1 && errno == 2) {
        return 2;
    }
    if (res != 0) {
        if (verbose) {
            fprintf(stderr, "POSIX Error: %d %s\n", errno, strerror(errno));
        }
        return -1;
    }
    
    if (S_ISDIR(sb.st_mode)) {
        return 0;
    }
    return 1;
}

/**
 * -1 = Error on I/O
 *  0 = File exists
 *  1 = Path not regular file
 *  2 = Not exist
 */
int env_isfile(const char* const path)
{
    struct stat sb;
    bzero(&sb, sizeof(stat));
    int res = stat(path, &sb);
    if (res == -1 && errno == 2) {
        return 2;
    }
    if (res != 0) {
        if (verbose) {
            fprintf(stderr, "POSIX Error: %d %s\n", errno, strerror(errno));
        }
        return -1;
    }
    
    if (S_ISREG(sb.st_mode)) {
        return 0;
    }
    return 1;
}

int env_createfolder(const char* const path)
{
    int status = env_isdir(path);
    if (status == 0) {
        // Folder exist, return success
        return 0;
    }
    if (status == 2) {
        if (mkdir(path, 0777) == 0) {
            fprintf(stdout, "Succesfully created directory: %s\n", path);
            return 0;
        }
        if (verbose) {
            fprintf(stderr, "POSIX Error: %d %s\n", errno, strerror(errno));
        }        
    }
    return 1;
}

int env_unlink(const char* const path)
{
    int res = unlink(path);
    if (res == 0) {
        return 0;
    }
    if (verbose) {
        fprintf(stderr, "POSIX Error: %d %s\n", errno, strerror(errno));
    }    
    return 1;
}