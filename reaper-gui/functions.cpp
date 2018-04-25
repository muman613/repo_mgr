#include "reaper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include "sqlite3.h"
#include "utils.h"
#include "reaper.h"
#include "functions.h"
#include "utils.h"
#include "CheckoutThread.h"
//#include "ReaperObject.h"

//#define ECHO_CONSOLE      1

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

static OUTCHARFUNC      pOutputCallback     = 0L;
static OUTPRINTFFUNC    pPrintfCallback     = 0L;
static void*            pUserData           = 0L;

void set_output_char_func(OUTCHARFUNC pFunc) {
    pOutputCallback = pFunc;
    return;
}

void set_printf_func(OUTPRINTFFUNC pFunc) {
    pPrintfCallback = pFunc;
    return;
}

void set_user_data(void* pData) {
    pUserData = pData;
    return;
}

/**
 *  Execute a shell command (testing for thread deletion)...
 *
 *  @note   Plan to re-write this function to use pipe/fork instead
 *          of popen in order to be able to kill the child process.
 */

//#define USE_POPEN2  1

//#undef ENABLE_MY_POPEN

#ifdef ENABLE_MY_POPEN
    #define POPEN       _popen
    #define PCLOSE      _pclose
#else
    #define POPEN       popen
    #define PCLOSE      pclose
#endif // ENABLE_MY_POPEN


int execute_shell_command(const STRING& sCmd, bool bEcho = true) {
    wxLogDebug("execute_shell_command('%s', %s)", sCmd, ((bEcho == true)?"TRUE":"FALSE"));

#ifdef USE_POPEN2
    int         ifd         = 0;
    pid_t       procID      = 0;
    STRING      sFullCmd    = sCmd + " 2>&1";

    procID = popen2( sFullCmd.c_str(), nullptr, &ifd );
    if (procID > 0) {
        FILE    *ifp = fdopen(ifd, "r");
        fd_set  fds;
        timeval to;
        int     res = 0;

        while (!feof(ifp)) {
            FD_ZERO(&fds);
            FD_SET(ifd, &fds);

            to.tv_sec   = 0;
            to.tv_usec  = 10000;

            res = select(ifd + 1, &fds, nullptr, nullptr, &to);
            if (res == -1) {
                perror("select");
            } else if (res > 0) {
                if (FD_ISSET(ifd, &fds)) {
                    char ch = fgetc(ifp);

#ifdef ECHO_CONSOLE
                    fprintf(stderr, "%c", ch);
#endif // ECHO_CONSOLE

                    if (bEcho && !feof(ifp) && (pOutputCallback != 0L))
                        (*pOutputCallback)(ch, pUserData);

                    /* Check if the thread is being canceled... */
                    if ((pCurrThread != nullptr) && (pCurrThread->TestDestroy())) {
                        wxLogDebug("-- exiting from execute_shell_command()!");

                        pclose(ifp);
                        return 0;
                    }
                } else {
                    wxLogDebug("-- Unknown file descriptor is set!");
                }
            } else {
                //wxLogDebug("timeout!");

                /* Check if the thread is being canceled... */
                if ((pCurrThread != nullptr) && (pCurrThread->TestDestroy())) {
                    wxLogDebug("-- exiting from execute_shell_command()!");

                    pclose(ifp);
                    return 0;
                }
            }
		}

        pclose(ifp);
    } else {

    }
#else
	FILE*   ifp         = nullptr;
    int     fd          = 0;
    STRING  sFullCmd    = sCmd + " 2>&1";

	ifp = POPEN( sFullCmd.c_str(), "r" );

	if (ifp) {
        fd_set fds;
        timeval to;

        fd = fileno( ifp );

		while (!feof(ifp)) {
            int res = 0;

            FD_ZERO(&fds);
            FD_SET(fd, &fds);

            to.tv_sec   = 0;
            to.tv_usec  = 10000;

            //wxLogDebug("before select!");

            res = select(fd + 1, &fds, nullptr, nullptr, &to);

            //wxLogDebug("after select [%d]!", res);

            if (res == -1) {
                perror("select");
            } else if (res > 0) {
                if (FD_ISSET(fd, &fds)) {
                    char ch = fgetc(ifp);

#ifdef ECHO_CONSOLE
                    fprintf(stderr, "%c", ch);
#endif // ECHO_CONSOLE

                    if (bEcho && !feof(ifp) && (pOutputCallback != 0L))
                        (*pOutputCallback)(ch, pUserData);

                    /* Check if the thread is being canceled... */
                    if ((pCurrThread != nullptr) && (pCurrThread->TestDestroy())) {
                        wxLogDebug("-- exiting from execute_shell_command()!");

                        kill(_pipe_pid(ifp), SIGKILL);

                        PCLOSE(ifp);
                        return 0;
                    }
                } else {
                    wxLogDebug("-- Unknown file descriptor is set!");
                }
            } else {
                //wxLogDebug("timeout!");

                /* Check if the thread is being canceled... */
                if ((pCurrThread != nullptr) && (pCurrThread->TestDestroy())) {
                    wxLogDebug("-- exiting from execute_shell_command()!");

                    kill(_pipe_pid(ifp), SIGKILL);

                    PCLOSE(ifp);
                    return 0;
                }
            }
		}

        PCLOSE(ifp);
	}
#endif // USE_POPEN2

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

#if 1
    (*pPrintfCallback)(pUserData, "Starting working branch %s...\n", (const char*)branchName.c_str());
#else
    printf("Starting working branch %s...\n", branchName.c_str());
#endif // 1
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

#if 1
    (*pPrintfCallback)(pUserData, "Rolling back repositories to date %s on branch %s\n",
           (const char*)sDateSpec.c_str(), (const char*)sNewBranchSpec.c_str());
#else
    printf("Rolling back repositories to date %s on branch %s\n",
           sDateSpec.c_str(), sNewBranchSpec.c_str());
#endif // 1

    sRepoCommand =  "repo forall -c 'git checkout -b " + sNewBranchSpec +   \
                    " `git rev-list -n 1 --before=\"" + sDateSpec + "\" " + \
                    sParentBranchSpec + "`'";

#if 1
    (*pPrintfCallback)(pUserData, "Command : %s\n", (const char*)sRepoCommand.c_str());
#else
    printf("Command : %s\n", sRepoCommand.c_str());
#endif // 1

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

#if 1
    (*pPrintfCallback)(pUserData, "<%s>\n", sCmd.c_str());
#else
    printf("<%s>\n", sCmd.c_str());
#endif // 1

    return execute_shell_command( sCmd );
}

/**
 *  Open the Sqlite3 database.
 */

bool open_database(STRING sDBname, sqlite3** dbHandle) {
	bool        bRes = false;

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

bool get_repodb_path(STRING& sRepoDbPath) {
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
                wxLogDebug("ERROR: Invalid repo type [%s]!", sRepoType);
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
    int         res         = 0;

#ifdef  _DEBUG
    fprintf(stderr, "checkout_repo()\n");
#endif

    if (parms.m_bDoCheckout == true) {
        if (!parms.m_sCheckoutDir.empty()) {
            (*pPrintfCallback)(pUserData, "Creating directory '%s'...\n",
                               (const char*)parms.m_sCheckoutDir.c_str());

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

        (*pPrintfCallback)(pUserData, "<%s>\n", sRepoCmd.c_str());

        res = repo_init( sRepoCmd );
        wxLogDebug("repo_init() result = %d", res);
        if (res == 0)
            return ERROR_OPERATION_CANCELED;

        res = repo_sync();
        wxLogDebug("repo_sync() result = %d", res);
        if (res == 0)
            return ERROR_OPERATION_CANCELED;

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
        (*pPrintfCallback)(pUserData, "Repo Command:\n\t%s ; repo sync\n",
                           (const char*)sRepoCmd.c_str());
    }

    return ERROR_NO_ERROR;
}

/**
 *  Checkout using git command.
 */

int checkout_git(CHECKOUT_PARMS& parms, ReaperEntry& entry) {
    STRING      sGitCmd     = entry.m_sCheckoutCmd;
    int         res         = 0;

    wxLogDebug("checkout_git()");

    if (parms.m_bDoCheckout == true) {
        if (!parms.m_sCheckoutDir.empty()) {
            sGitCmd += " " + parms.m_sCheckoutDir;
        }

        (*pPrintfCallback)(pUserData, "<%s>\n", sGitCmd.c_str());

        res = git_checkout( sGitCmd );

        if (res == 1) {
            if (!parms.m_sWorkingBranchName.empty()) {
                char*       sCurDirName = 0;

                sCurDirName = get_current_dir_name();
                (void)chdir( parms.m_sCheckoutDir.c_str() );

                git_start_branch(parms.m_sWorkingBranchName);

                (void)chdir( sCurDirName );
                free( sCurDirName );
            }
        } else {
//            (*pPrintfCallback)(pUserData, "Checkout aborted!\n");
            return ERROR_OPERATION_CANCELED;
        }
    } else {
        (*pPrintfCallback)(pUserData, "Git Command:\n\t%s\n",
                           (const char*)sGitCmd.c_str());
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
    STRING      sCrepoCmd       = entry.m_sCheckoutCmd;
    int         res             = 0;

    if (parms.m_bDoCheckout == true) {
        if (!parms.m_sCheckoutDir.empty()) {
            sCrepoCmd += " -d " + parms.m_sCheckoutDir;
        }

        (*pPrintfCallback)(pUserData, "<%s>\n", sCrepoCmd.c_str());

        res = crepo_checkout( sCrepoCmd );

        if (res == 0)
            return ERROR_OPERATION_CANCELED;
    } else {
        (*pPrintfCallback)(pUserData, "<%s>\n", sCrepoCmd.c_str());
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
