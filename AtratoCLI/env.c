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
#include <termios.h>

#include "env.h"

extern int verbose;

char* env_homedir(void)
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
int env_isdir(const char* path)
{
    struct stat sb;
    bzero(&sb, sizeof(struct stat));
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
int env_isfile(const char* path)
{
    struct stat sb;
    bzero(&sb, sizeof(struct stat));
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

int env_createfolder(const char* path)
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

int env_unlink(const char* path)
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

/**
 * getpass is obsolete (it won't compile with GCC on Ubuntu
 * without throwing out a warning)
 * @see http://linux.die.net/man/3/getpass
 * @see http://stackoverflow.com/questions/1196418/getting-a-password-in-c-without-using-getpass-3
 * @see http://www.gnu.org/software/libc/manual/html_node/getpass.html
 */
size_t env_getpass(char** lineptr, size_t* n, FILE* stream)
{
    struct termios old, new;
    ssize_t nread = 0;
    
    /* Turn echoing off and fail if we can't. */
    if (tcgetattr(fileno (stream), &old) != 0) {
        if (verbose) {
            fprintf(stderr, "Failed getting tty\n");
        }
        return 0;
    }
    new = old;
    new.c_lflag &= ~ECHO;
    if (tcsetattr(fileno (stream), TCSAFLUSH, &new) != 0) {
        if (verbose) {
            fprintf(stderr, "Failed turning echo off on tty\n");
        }        
        return 0;
    }
    
    /* Read the password. */
    nread = getline(lineptr, n, stream);
    if (nread == EINVAL) {
        fprintf(stderr, "Invalid argument(s) to getline\n");
    }
    /* Change last char to \0 because \n is added by getline */
    char* line = *lineptr;
    line[ strlen(line)-1 ] = '\0';
    
    /* Restore terminal. */
    if (tcsetattr(fileno(stream), TCSAFLUSH, &old) != 0) {
        if (verbose) {
            fprintf(stderr, "Failed restoring tty\n");            
        }
        return 0;
    }
    
    if (nread < 0) {
        if (verbose) {
            fprintf(stderr, "getline returned invalid\n");            
        }        
        return 0;
    }
    return nread;
}