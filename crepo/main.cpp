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

#include "StdAfx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <popt.h>
#include "functions.h"
#include "utils.h"
#include "repoObject.h"
#include "actions.h"

using namespace std;

const char*     szAppName   = CREPO_APP_NAME;
const char*     szBuildDate = CREPO_BUILD_DATE;


//bool checkout_handler(const crepo_parms& parms);
//bool diff_handler(const crepo_parms& parms);

struct action_dispatch_table dispatch[] = {
    { "help",       ACTION_HELP,     help_handler,      },
    { "version",    ACTION_VERSION,  version_handler,   },
    { "co",         ACTION_CHECKOUT, checkout_handler,  },
    { "diff",       ACTION_DIFF,     diff_handler,      },
    { "stat",       ACTION_STAT,     stat_handler,      },
    { "branch",     ACTION_BRANCH,   branch_handler,    },
    { "tag",        ACTION_TAG,      tag_handler,       },
    { "sync",       ACTION_SYNC,     sync_handler,      },
};

/**
 *
 */

bool parse_args(crepo_parms& parms, int argc, const char* argv[]) {
    bool bRes = false;
    poptContext popt;
    int rc;
    char *szBranch    = nullptr,
         *szDirectory = nullptr;
    const char * leftOver = nullptr;

    if (argc < 2) {
        return false;
    }

    parms.eAction = ACTION_UNKNOWN;

    poptOption options[] = {
        { "branch",    'b', POPT_ARG_STRING, (void *)&szBranch,    'b', "Branch",    "Branchname", },
        { "directory", 'd', POPT_ARG_STRING, (void *)&szDirectory, 'd', "Directory", "path", },

//        POPT_AUTOHELP
        POPT_TABLEEND
    };

    for (size_t i = 0 ; i < sizeof(dispatch)/sizeof(action_dispatch_table) ; i++) {
        if (dispatch[i].option == argv[1]) {
            parms.eAction = dispatch[i].action;
#ifdef  _DEBUG
            fprintf(stderr, "Using action %s\n", dispatch[i].option.c_str());
#endif // _DEBUG
            break;
        }
    }

    if (parms.eAction == ACTION_UNKNOWN) {
        fprintf(stderr, "No action specified!\n");
        exit(-10);
    }

    popt = poptGetContext("crepo", argc-1, argv + 1, options, 0L);
    poptSetOtherOptionHelp(popt, "[OPTIONS]* <repository-url>");

    while ((rc = poptGetNextOpt(popt)) > 0) {
        switch (rc) {
        case 'b':
            parms.sRepoBranch = szBranch;
            break;
        case 'd':
            parms.sCheckoutDir = szDirectory;
            break;
        /* specific arguments are handled here */
        }
    }

    if (parms.eAction == ACTION_CHECKOUT) {
        leftOver = poptGetArg(popt);

        if (leftOver != nullptr) {
            parms.sRepoURL = leftOver;
            bRes           = true;
        }
    } else if (parms.eAction == ACTION_TAG) {
        leftOver = poptGetArg(popt);

        if (leftOver != nullptr) {
            parms.sTag     = leftOver;
            bRes           = true;
        }
    } else if (parms.eAction != ACTION_UNKNOWN) {
        bRes = true;
    }

    poptFreeContext( popt );

    return bRes;
}

/**
 *  Dispatch execution to action handler.
 */

bool dispatch_crepo(const crepo_parms& parms) {
    for (size_t i = 0 ; i < sizeof(dispatch)/sizeof(action_dispatch_table) ; i++) {
        if (dispatch[i].action == parms.eAction) {
            return (*dispatch[i].handler)(parms);
        }
    }

#ifdef  _DEBUG
    fprintf(stderr, "Invalid command!\n");
#endif // _DEBUG

    return false;
}

/**
 *
 */

int main(int argc, const char* argv[]) {
    crepo_parms     parms;
    bool            bRes = false;
    int             nRes = 0;

    if (parse_args( parms, argc, argv ) == true) {
        bRes = dispatch_crepo( parms );
        if (bRes != true)
            fprintf(stderr, "res = %d\n", bRes);
    } else {
        fprintf(stderr, "ERROR: Invalid parameters!\n");
    }

    return nRes;
}
