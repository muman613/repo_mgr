/**
 *      @file       utils.cpp
 *      @author     Michael A. Uman
 *      @date       May 24, 2013
 */

#include "reaper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>

using namespace std;

/**
 *
 */

string get_exe_path() {
    string  sPath;
    pid_t   pid;
    char    sProcPath[128];
    char    buf[256];
    string  sExePath;

    pid = getpid();

    sprintf(sProcPath, "/proc/%d/exe", pid);
    if (readlink(sProcPath, buf, sizeof(buf)) != -1) {
        sPath = dirname(buf);
    } else {
        perror("get_exe_path");
    }

    return sPath;
}

/**
 *
 */

bool file_exists(string sFilename) {
    bool bRes =false;
    struct stat statbuf;

    if (stat( sFilename.c_str(), &statbuf ) == 0) {
        bRes = true;
    }

    return bRes;
}

int make_directory(string sDirName) {
//    int     nRes = 0;
    struct stat statbuf;

    if (stat( sDirName.c_str(), &statbuf ) == 0) {
        /* check if a file is in the way */
        if (S_ISREG(statbuf.st_mode)) {
#ifdef  _DEBUG
            fprintf(stderr, "WARNING: Cannot create directory because file exists with name '%s'\n", sDirName.c_str());
#endif
            return -10;
        }
        /* check if directory already exists */
        if (S_ISDIR(statbuf.st_mode)) {
#ifdef  _DEBUG
            fprintf(stderr, "WARNING: Directory '%s' already exists!\n", sDirName.c_str());
#endif
            return -20;
        }
    }

    return mkdir(sDirName.c_str(), S_IRWXU|S_IRWXG|S_IROTH|S_IWOTH);
}
