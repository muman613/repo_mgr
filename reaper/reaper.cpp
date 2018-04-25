/**
 *  @file       reaper.cpp
 *  @author     Michael A. Uman
 *  @date       February 24, 2015
 *  @note       The reaper project.
 */

#include "reaper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "sqlite3.h"
#include "utils.h"
#include "reaper.h"
#include "functions.h"

using namespace std;

const STRING      sAllString = "all";

/* Global variables */
STRING      sBranchSpec;
STRING      sRepoDbName     = "repositories.db";
STRING      sRepoDbPath;
STRING      sDestDir;
STRING      sWorkingBranch  = DEFAULT_WORKING_BRANCH;
STRING      sCheckoutDate;
STRING      sListCategory   = sAllString;

sqlite3*    pDBHandle   = 0L;
OPERATION   appOper     = APP_CHECKOUT;
bool        doCheckO    = true;
bool        bVerbose    = false;

/**
 *  ASCII art logo.
 */

unsigned char _logo_txt[] = {
    0x20, 0x5f, 0x5f, 0x5f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x0a, 0x7c, 0x20, 0x5f, 0x20, 0x5c, 0x5f, 0x5f,
    0x5f, 0x20, 0x5f, 0x5f, 0x20, 0x5f, 0x20, 0x5f, 0x20, 0x5f, 0x5f, 0x20,
    0x20, 0x5f, 0x5f, 0x5f, 0x20, 0x5f, 0x20, 0x5f, 0x20, 0x0a, 0x7c, 0x20,
    0x20, 0x20, 0x2f, 0x20, 0x2d, 0x5f, 0x29, 0x20, 0x5f, 0x60, 0x20, 0x7c,
    0x20, 0x27, 0x5f, 0x20, 0x5c, 0x2f, 0x20, 0x2d, 0x5f, 0x29, 0x20, 0x27,
    0x5f, 0x7c, 0x0a, 0x7c, 0x5f, 0x7c, 0x5f, 0x5c, 0x5f, 0x5f, 0x5f, 0x5c,
    0x5f, 0x5f, 0x2c, 0x5f, 0x7c, 0x20, 0x2e, 0x5f, 0x5f, 0x2f, 0x5c, 0x5f,
    0x5f, 0x5f, 0x7c, 0x5f, 0x7c, 0x20, 0x20, 0x0a, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7c, 0x5f, 0x7c,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x00,
};

/**
 *
 */

bool display_help(const char* szCmd) {
    printf("Checkout branch : %s [-n] [-v] [-b branchname] [-d dirname] [-w workbranch] [-s datespec]\n", szCmd);
    printf("List branches   : %s [-l category]\n", szCmd);

    return false;
}

/**
 *  Display the banner and additional information...
 */

void display_info(const char* szCmd) {
    displayBanner();
    printf("\nBuilt using sqlite3 version %s\n", sqlite3_libversion());
    return;
}

/**
 *  Parse the commandline arguments using 'getopt'...
 */

bool parse_commandline(int argc, char* const argv[]) {
    bool    bRes = false;
    int     opt;
    int     errflg = 0;

    get_repodb_path();

#ifdef  _DEBUG
    fprintf(stderr, "sRepoDbPath = %s\n", sRepoDbPath.c_str());
#endif

    while ((opt = getopt(argc, argv, ":b:d:nvw:hl:izs:")) != -1) {
        switch (opt) {
        case 'b':
            sBranchSpec = optarg;
            break;

        case 'd':
            sDestDir = optarg;
            break;

        case 'l':
            appOper = APP_LIST;
            sListCategory = optarg;
            break;

        case 'n':
            doCheckO = false;
            break;

        case 'v':
            bVerbose = true;
            break;

        case 'w':
            sWorkingBranch = optarg;
            break;

        case 'h':
            display_help(argv[0]);
            exit(0);
            break;

        case 'i':
            display_info(argv[0]);
            exit(0);
            break;

        case 'z':
            launch_video_playback();
            exit(0);
            break;

        case '?':
            errflg++;
            break;

        case ':':
            if (optopt == 'l') {
                appOper = APP_LIST;
                sListCategory = sAllString;
            } else {
                errflg++;
            }
            break;

        case 's':
            sCheckoutDate = optarg;
            break;

        default:
#ifdef  _DEBUG
            fprintf(stderr, "illegal opt = %c\n", opt);
#endif
            errflg++;
            break;
        }
    }

    if (errflg) {
        display_help(argv[0]);
        exit(-1);
    }

    /* Validate options before returning */
    if (appOper == APP_LIST) {
#ifdef  _DEBUG
        fprintf(stderr, "-- user specified list operation.\n");
#endif
        bRes = true;
    } else {
        if (!sBranchSpec.empty()) {
            if (sDestDir.empty()) {
                sDestDir = sBranchSpec;
            }
            bRes = true;
        } else {
            fprintf(stderr, "Must specify branch with -b <branchtag> option.\n");
            bRes = false;
        }
    }

    return bRes;
}

/**
 *  Display ASCII art logo and version information.
 */

void displayBanner() {
    printf("%s version %d.%d\n", _logo_txt, REAPER_VERSION_MAJOR, REAPER_VERSION_MINOR);
}

/**
 *
 */

int main(int argc, char* const argv[]) {
    int                 nRes = 0;
    REAPERENTRY_VECTOR  branchVec;

    if (!parse_commandline(argc, argv)) {
        //fprintf(stderr, "Invalid options!\n");
        return -1;
    }

    displayBanner();

    if (bVerbose) {
        printf("Using repository database @ %s ...\n", sRepoDbPath.c_str());
    }

    /* Open the database and perform operation */
    if (open_database(sRepoDbPath, &pDBHandle)) {
        STRING sVersion, sDate;

#ifdef  _DEBUG
        fprintf(stderr, "pDBHandle = %p\n", pDBHandle);
#endif
        if (get_database_version(pDBHandle, sVersion, sDate)) {
            printf("Database version %s [%s]\n", sVersion.c_str(), sDate.c_str());
        }

        if (get_branch_vector(pDBHandle, branchVec)) {
#ifdef  _DEBUG
            fprintf(stderr, "Found %zu branch specifications\n", branchVec.size());
#endif
            if (appOper == APP_LIST) {
                list_branches(branchVec, sListCategory);
            } else {
                CHECKOUT_PARMS      parms;
                int                 err = ERROR_NO_ERROR;

                parms.m_entryVector         = &branchVec;
                parms.m_bDoCheckout         = doCheckO;
                parms.m_sBranchName         = sBranchSpec;
                parms.m_sCheckoutDir        = sDestDir;
                parms.m_sWorkingBranchName  = sWorkingBranch;
                parms.m_sCheckoutDate       = sCheckoutDate;

                if ((err = checkout_branch(parms)) == ERROR_NO_ERROR) {
                    /* */
                    printf("Repository checked out...\n");
                } else {
                    switch (err) {
                    case ERROR_INVALID_BRANCH:
                        printf("Invalid branch specified!\n");
                        nRes = -15;
                        break;

                    case ERROR_INVALID_REPOTYPE:
                        printf("Invalid repo type!\n");
                        nRes = -20;
                        break;

                    default:
                        printf("An error occured while checking out...\n");
                        break;
                    }
                }
            }
        }
        close_database(pDBHandle);
    } else {
        fprintf(stderr, "ERROR: Unable to open repository database!\n");
        nRes = -10;
    }


    return nRes;
}
