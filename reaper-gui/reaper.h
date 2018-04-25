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

#define     ERROR_NO_ERROR              0
#define     ERROR_INVALID_BRANCH        1
#define     ERROR_INVALID_DEST          2
#define     ERROR_NO_DATABASE           3
#define     ERROR_INVALID_REPOTYPE      4
#define     ERROR_OPERATION_CANCELED    5

#endif // __REAPER_H__
