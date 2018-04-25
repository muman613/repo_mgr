/*
    This file is part of crepo.

    crepo is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    crepo is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with crepo.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 *      @file       utils.cpp
 *      @author     Michael A. Uman
 *      @date       May 24, 2013
 */

#include "StdAfx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>
#include "utils.h"
#include "crepo.h"

const char *ansiReset     	="\033[0m";
const char *ansiBoldBlack 	="\033[1;30m";
const char *ansiBoldRed   	="\033[1;31m";
const char *ansiBoldGreen 	="\033[1;32m";
const char *ansiBoldBrown 	="\033[1;33m";
const char *ansiBoldBlue  	="\033[1;34m";
const char *ansiBoldMagenta ="\033[1;34m";
const char *ansiBoldCyan  	="\033[1;34m";
const char *ansiBoldGrey  	="\033[1;34m";

/**
 *
 */

STRING get_exe_path() {
    STRING  sPath;
    pid_t   pid;
    char    sProcPath[128];
    char    buf[256];
    STRING  sExePath;

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

bool file_or_dir_exists(STRING sFilename, int flags) {
    bool bRes =false;
    struct stat statbuf;

    if (stat( sFilename.c_str(), &statbuf ) == 0) {
        if (flags & FILE_FLAG) {
            if (S_ISREG(statbuf.st_mode))
                bRes = true;
        }

        if (flags & DIR_FLAG) {
            if (S_ISDIR(statbuf.st_mode))
                bRes = true;
        }
    }

    return bRes;
}

int make_directory(STRING sDirName) {
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

bool get_current_directory(STRING& sPath) {
    const char* szTmp = get_current_dir_name();
    sPath = szTmp;
    free((void*)szTmp);

    return true;
}
