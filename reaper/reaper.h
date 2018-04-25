#ifndef __REAPER_H__
#define __REAPER_H__

#include <string>
#include <vector>
#include <utility>
#include "sqlite3.h"
#include "reapertypes.h"
#include "functions.h"
#include "reaperEntry.h"
#include "utils.h"

typedef enum __operation {
    APP_LIST,
    APP_CHECKOUT,
} OPERATION;


#define     REAPER_VERSION_MAJOR                1
#define     REAPER_VERSION_MINOR                4

extern STRING      sBranchSpec;
extern STRING      sRepoDbName;
extern STRING      sRepoDbPath;

bool parse_commandline(int argc, char* const argv[]);
void displayBanner();

#define DEFAULT_WORKING_BRANCH      "working_branch"

enum {
    ERROR_NO_ERROR = 0,
    ERROR_INVALID_BRANCH,
    ERROR_INVALID_DEST,
    ERROR_NO_DATABASE,
    ERROR_INVALID_REPOTYPE,
    ERROR_GENERIC_ERROR,
};

#endif // __REAPER_H__
