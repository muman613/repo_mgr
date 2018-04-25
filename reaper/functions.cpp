#include "reaper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include "sqlite3.h"
#include "utils.h"
#include "reaper.h"
#include "functions.h"
#include "ReaperObject.h"

using namespace std;

/**
 *
 */

int execute_shell_command(const STRING& sCmd, bool bEcho = true) {
//	int nRes = 0;
	FILE* ifp;

	ifp = popen( sCmd.c_str(), "r" );

	if (ifp) {
		while (!feof(ifp)) {
			char ch = fgetc(ifp);

			if (bEcho && !feof(ifp))
				printf("%c", ch);
		}
//		nRes = 1;
        pclose(ifp);
	}

	return 1;
}

/**
 *
 */

int repo_init(const STRING& cmd) {
	int nRes = execute_shell_command( cmd );

	return nRes;
}

/**
 *
 */

int repo_sync(int options) {
	STRING sCmd;
//	int nRes = 0;
	sCmd = "repo sync";

	return execute_shell_command( sCmd );
}

/**
 *
 */

int repo_start(const STRING& branchName) {
    STRING  sCmd;

    printf("Starting working branch %s...\n", branchName.c_str());
    sCmd = "repo start --all " + branchName;

    return execute_shell_command( sCmd );
}

/**
 *  Roll-back a repository to a particular date.
 */

int repo_rollback_date(STRING sDateSpec,
                       STRING sNewBranchSpec,
                       STRING sParentBranchSpec)
{
    int     nRes = -1;
    STRING  sRepoCommand;

    if (sNewBranchSpec == sParentBranchSpec) {
        sNewBranchSpec += "_" + sDateSpec;
    }

    printf("Rolling back repositories to date %s on branch %s\n",
           sDateSpec.c_str(), sNewBranchSpec.c_str());

    sRepoCommand =  "repo forall -c 'git checkout -b " + sNewBranchSpec +   \
                    " `git rev-list -n 1 --before=\"" + sDateSpec + "\" " + \
                    sParentBranchSpec + "`'";

    printf("Command : %s\n", sRepoCommand.c_str());

    nRes = execute_shell_command( sRepoCommand );

    return nRes;
}

int crepo_checkout(const STRING& cmd) {
	int nRes = execute_shell_command( cmd );

	return nRes;
}

/**
 *
 */

int git_checkout(const STRING& cmd) {
	int nRes = execute_shell_command( cmd );

	return nRes;
}

/**
 *  Start a new branch in Git.
 */

int git_start_branch(const STRING& sBranchName) {
    STRING sCmd = "git co -b " + sBranchName;
    printf("<%s>\n", sCmd.c_str());
    return execute_shell_command( sCmd );
}

/**
 *  Open the Sqlite3 database.
 */

bool open_database(STRING sDBname, sqlite3** dbHandle) {
	bool        bRes = false;
//    struct stat statbuf;

    if (file_exists( sDBname.c_str() )) {

        if (sqlite3_open( sDBname.c_str(), dbHandle) == SQLITE_OK) {
#ifdef  _DEBUG
            fprintf(stderr, "-- database opened!\n");
#endif
            bRes = true;
        }
    } else {
#ifdef  _DEBUG
        fprintf(stderr, "ERROR: Unable to locate db file '%s'...\n", sDBname.c_str());
#endif
    }

	return bRes;
}

/**
 *
 */

void close_database(sqlite3* dbHandle) {
    if (dbHandle != 0L) {
        sqlite3_close( dbHandle );
#ifdef  _DEBUG
		fprintf(stderr, "-- database closed!\n");
#endif
    }

	return;
}

/**
 *
 */

bool get_database_version(sqlite3* dbHandle, STRING& sVersion, STRING& sDate) {
	bool            bRes = false;
	STRING		    sSQL;
	sqlite3_stmt*	stmt;
	int				rc;

#ifdef  _DEBUG
    fprintf(stderr, "get_database_version()\n");
#endif

    sSQL = "SELECT * from version";
	rc = sqlite3_prepare_v2( dbHandle, sSQL.c_str(), -1, &stmt, 0 );
	if (rc == SQLITE_OK) {
        STRING sMajor, sMinor;

        if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            sMajor = get_field_from_stmt(stmt, 0);
            sMinor = get_field_from_stmt(stmt, 1);
            sDate  = get_field_from_stmt(stmt, 2);

            sVersion = sMajor + "." + sMinor;

            bRes = true;
        } else {
#ifdef _DEBUG
            fprintf(stderr, "ERROR: Unable to locate a record in version table!\n");
#endif // _DEBUG
        }
	}

	return bRes;
}

/**
 *
 */

bool get_branch_vector(sqlite3* dbHandle, REAPERENTRY_VECTOR& sVec) {
	sqlite3_stmt*	stmt;
	int				rc;
	STRING		    sSQL;

#ifdef  _DEBUG
    fprintf(stderr, "get_branch_vector()\n");
#endif

	sVec.clear();

	sSQL = "SELECT branch, subbranch, checkout, owner, category, type, purpose from repos order by category, branch, subbranch";

	rc = sqlite3_prepare_v2( dbHandle, sSQL.c_str(), -1, &stmt, 0 );
	if (rc == SQLITE_OK) {
#ifdef  _DEBUG
        fprintf(stderr, "===========================================================\n");
#endif
		while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
			STRING sBranchSpec;
			STRING sBranch    = get_field_from_stmt(stmt, 0);
			STRING sSubBranch = get_field_from_stmt(stmt, 1);
            STRING sRepoCmd   = get_field_from_stmt(stmt, 2);
            STRING sRepoOwner = get_field_from_stmt(stmt, 3);
            STRING sRepoCat   = get_field_from_stmt(stmt, 4);
            STRING sRepoType  = get_field_from_stmt(stmt, 5);
            STRING sRepoDesc  = get_field_from_stmt(stmt, 6);

			if (sSubBranch.length() > 0) {
				sBranchSpec = sBranch + "/" + sSubBranch;
			} else {
				sBranchSpec = sBranch;
			}

#ifdef  _DEBUG
			fprintf(stderr, "spec : %s\n", sBranchSpec.c_str());
			fprintf(stderr, "Branch %s\nSubbranch %s\nrepoCmd %s\nrepoOwner %s\nrepoCat %s\nrepoType %s\nrepoDesc %s\n",
                    sBranch.c_str(), sSubBranch.c_str(), sRepoCmd.c_str(),
                    sRepoOwner.c_str(), sRepoCat.c_str(), sRepoType.c_str(),
                    sRepoDesc.c_str());
            fprintf(stderr, "===========================================================\n");
#endif

			sVec.push_back(ReaperEntry(sBranchSpec, sRepoCmd, sRepoCat,
                                       sRepoType, sRepoOwner, sRepoDesc));
		}

		sqlite3_finalize(stmt);
	}

	return (sVec.size() != 0)?true:false;
}
/**
 *  If the environment REAPERDB exists, read the full path from the variable.
 *  Otherwise find the path to the executable and append the database name to it.
 */

bool get_repodb_path() {
    STRING  sExePath;
    char*   szTmp = 0L;

#ifdef	_DEBUG
    fprintf(stderr, "get_repodb_path()\n");
#endif

    if ((szTmp = getenv("REAPERDB")) == 0) {
        sExePath = get_exe_path();
        sRepoDbPath = sExePath + STRING("/") + sRepoDbName;
    } else {
        sRepoDbPath = szTmp;
    }

    return true;
}

#pragma GCC diagnostic ignored "-Wunused-result"

/**
 *
 */

int checkout_branch(CHECKOUT_PARMS& parms) {
    REAPERENTRY_VECTOR          *branchVec = parms.m_entryVector;
    REAPERENTRY_VECTOR_ITER     spIter;
    int                         err = ERROR_NO_ERROR;

    assert(branchVec != 0L);

#ifdef  _DEBUG
    fprintf(stderr, "checkout_branch(..., %s, %s, %s)\n",
                    parms.m_sBranchName.c_str(),
                    parms.m_sCheckoutDir.c_str(),
                    parms.m_bDoCheckout?"T":"F");
#endif

    for (spIter = branchVec->begin() ; spIter != branchVec->end() ; spIter++) {
        if ((*spIter).m_sBranchSpec == parms.m_sBranchName) {
            STRING  sRepoType = (*spIter).m_sRepoType;

            if (sRepoType == "repo") {
                err = checkout_repo(parms, (*spIter));
            } else if (sRepoType == "git") {
                err = checkout_git(parms, (*spIter));
            } else if (sRepoType == "crepo") {
                err = checkout_crepo(parms, (*spIter));
            } else {
#ifdef  _DEBUG
                fprintf(stderr, "ERROR: Invalid repo type [%s]!\n",
                        sRepoType.c_str());
#endif
                return ERROR_INVALID_REPOTYPE;
            }

            return err;
        }
    }

    return ERROR_INVALID_BRANCH;
}

/**
 *  Checkout using repo command.
 */

int checkout_repo(CHECKOUT_PARMS& parms, ReaperEntry& entry) {
    STRING      sRepoCmd = entry.m_sCheckoutCmd;
    char*       sCurDirName = 0;

#ifdef  _DEBUG
    fprintf(stderr, "checkout_repo()\n");
#endif

    if (parms.m_bDoCheckout == true) {
        if (!parms.m_sCheckoutDir.empty()) {
            printf("Creating directory '%s'...\n", parms.m_sCheckoutDir.c_str());

            int nRes = make_directory( parms.m_sCheckoutDir );

            if (nRes != 0) {
                switch (nRes) {
                    case -10:
                        printf("ERROR: File '%s' in the way!\n", parms.m_sCheckoutDir.c_str());
                        break;
                    case -20:
                        printf("ERROR: Directory %s already exists!\n", parms.m_sCheckoutDir.c_str());
                        break;
                }

                return ERROR_INVALID_DEST;
            }

            sCurDirName = get_current_dir_name();
            (void)chdir( parms.m_sCheckoutDir.c_str() );
        }

        printf("<%s>\n", sRepoCmd.c_str());

        repo_init( sRepoCmd );
        repo_sync();

        if (parms.m_sCheckoutDate.empty()) {
            if (!parms.m_sWorkingBranchName.empty()) {
                repo_start(parms.m_sWorkingBranchName);
            }
        } else {
            /* Rollback checkout */
            repo_start(DEFAULT_WORKING_BRANCH);
            repo_rollback_date(parms.m_sCheckoutDate,
                               parms.m_sWorkingBranchName,
                               DEFAULT_WORKING_BRANCH);
        }

        if (sCurDirName != 0) {
            /* Return to the previously saved directory */
            (void)chdir( sCurDirName );
            free( sCurDirName );
        }
    } else {
        printf("Repo Command:\n\t%s ; repo sync\n", sRepoCmd.c_str());
    }

    return ERROR_NO_ERROR;
}

/**
 *  Checkout using git command.
 */

int checkout_git(CHECKOUT_PARMS& parms, ReaperEntry& entry) {
    STRING      sGitCmd     = entry.m_sCheckoutCmd;

    if (parms.m_bDoCheckout == true) {
        if (!parms.m_sCheckoutDir.empty()) {
            sGitCmd += " " + parms.m_sCheckoutDir;
        }

        printf("<%s>\n", sGitCmd.c_str());

        git_checkout( sGitCmd );

        if (!parms.m_sWorkingBranchName.empty()) {
            char*       sCurDirName = 0;

            sCurDirName = get_current_dir_name();
            (void)chdir( parms.m_sCheckoutDir.c_str() );

            git_start_branch(parms.m_sWorkingBranchName);

            (void)chdir( sCurDirName );
            free( sCurDirName );
        }

    } else {
        printf("Git Command:\n\t%s\n", sGitCmd.c_str());
    }

#ifdef  _DEBUG
    fprintf(stderr, "checkout_git()\n");
#endif

    return ERROR_NO_ERROR;
}

/**
 *  Checkout using 'crepo' command.
 */

int checkout_crepo(CHECKOUT_PARMS& parms, ReaperEntry& entry) {
    STRING      sCrepoCmd     = entry.m_sCheckoutCmd;

    if (parms.m_bDoCheckout == true) {
        if (!parms.m_sCheckoutDir.empty()) {
            sCrepoCmd += " -d " + parms.m_sCheckoutDir;
        }

        printf("<%s>\n", sCrepoCmd.c_str());

        crepo_checkout( sCrepoCmd );

#if 0
        if (!parms.m_sWorkingBranchName.empty()) {
            char*       sCurDirName = 0;

            sCurDirName = get_current_dir_name();
            (void)chdir( parms.m_sCheckoutDir.c_str() );

            git_start_branch(parms.m_sWorkingBranchName);

            (void)chdir( sCurDirName );
            free( sCurDirName );
        }
#endif // 0
    } else {
        printf("crepo Command:\n\t%s\n", sCrepoCmd.c_str());
    }

#ifdef  _DEBUG
    fprintf(stderr, "checkout_git()\n");
#endif

    return ERROR_NO_ERROR;
}

#pragma GCC diagnostic warning "-Wunused-result"

/**
 *
 */

void list_branches(REAPERENTRY_VECTOR& branchVec, const STRING& sCategory) {
    REAPERENTRY_VECTOR_ITER svIter;

    printf("Available branches:\n");

    for (svIter = branchVec.begin() ; svIter != branchVec.end() ; svIter++) {
        if ((sCategory == sAllString) ||
            (sCategory == (*svIter).m_sRepoCategory))
        {
            printf("    %-45s | %-12s | %s\n", (*svIter).m_sBranchSpec.c_str(),
                                      (*svIter).m_sRepoCategory.c_str(),
                                      (*svIter).m_sDescription.c_str());
        }
    }

    return;
}

/**
 *
 */

STRING get_field_from_stmt(sqlite3_stmt* stmt, int field) {
    STRING          sResult;
    const char*     szFieldData = 0L;

    szFieldData = (const char*)sqlite3_column_text(stmt, field);

    if (szFieldData != 0) {
        sResult = szFieldData;
    }

    return sResult;
}

/**
 *  Play the "Don't fear the Reaper" by BOC video...
 */

int launch_video_playback() {
    STRING  sCmd = "xdg-open https://www.youtube.com/watch?v=ClQcUyhoxTg";

    return execute_shell_command( sCmd, false );
}
