//------------------------------------------------------------------------------
//  MODULE      :   ReaperObject.cpp
//  PROJECT     :   Reaper
//  AUTHOR      :   Michael A. Uman
//  DATE        :   July 30, 2013
//------------------------------------------------------------------------------

//#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <assert.h>
#include "utils.h"
#include "ReaperObject.h"
//#include "checkoutParms.h"

/**
 *
 */

/**
 *
 */

ReaperObject::ReaperObject()
:   m_bIsValid(false),
    m_dbHandle(0L)
{
    //ctor
}

ReaperObject::ReaperObject(STRING sRepoDBName)
:   m_bIsValid(false),
    m_dbHandle(0L)
{
    //ctor
    Open(sRepoDBName);
}

ReaperObject::~ReaperObject()
{
    //dtor
    Close();
}

/**
 *
 */

bool ReaperObject::IsValid()
{
    return m_bIsValid;
}

/**
 *
 */

STRING ReaperObject::Get_RepoDB_Path()
{
    return m_sRepoDatabasePath;
}

/**
 *
 */

bool ReaperObject::Open(STRING sRepoDBName)
{
    bool    bRes = false;

#ifdef  _DEBUG
    fprintf(stderr, "%s()\n", __FUNCTION__);
#endif

    m_sRepoDatabaseName = sRepoDBName;

    if (get_repodb_path()) {
        if (open_database()) {
            bRes = get_branch_vector();
            close_database();
        }
    }

    m_bIsValid = bRes;

#ifdef  _DEBUG
    fprintf(stderr, "%s\n", m_bIsValid?"OK":"FAIL");
#endif

    return bRes;
}

/**
 *
 */

void ReaperObject::Close()
{
#ifdef  _DEBUG
    fprintf(stderr, "%s()\n", __FUNCTION__);
#endif

    //
    m_reaperVec.clear();
    m_bIsValid = false;
}

/**
 *  If the environment REAPERDB exists, read the full path from the variable.
 *  Otherwise find the path to the executable and append the database name to it.
 */

bool ReaperObject::get_repodb_path() {
    STRING  sExePath;
    char*   szTmp = 0L;

#ifdef	_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif

    if ((szTmp = getenv("REAPERDB")) == 0) {
        sExePath = get_exe_path();
        m_sRepoDatabasePath = sExePath + STRING("/") + m_sRepoDatabaseName;
    } else {
        m_sRepoDatabaseName = basename(szTmp);
        m_sRepoDatabasePath = szTmp;
    }

    return true;
}

/**
 *
 */

bool ReaperObject::open_database() {
	bool        bRes = false;
//    struct stat statbuf;

#ifdef  _DEBUG
    fprintf(stderr, "%s()\n", __FUNCTION__);
#endif

    if (file_exists( m_sRepoDatabasePath.c_str() )) {

        if (sqlite3_open( m_sRepoDatabasePath.c_str(), &m_dbHandle) == SQLITE_OK) {
#ifdef  _DEBUG
            fprintf(stderr, "-- database opened!\n");
#endif
            bRes = true;
        }
    } else {
#ifdef  _DEBUG
        fprintf(stderr, "ERROR: Unable to locate db file '%s'...\n", m_sRepoDatabasePath.c_str());
#endif
    }

	return bRes;
}

/**
 *
 */

void ReaperObject::close_database() {
#ifdef  _DEBUG
    fprintf(stderr, "%s()\n", __FUNCTION__);
#endif

    if (m_dbHandle != 0L) {
        sqlite3_close( m_dbHandle );
#ifdef  _DEBUG
		fprintf(stderr, "-- database closed!\n");
#endif
    }

	return;
}

/**
 *
 */

bool ReaperObject::get_branch_vector() {
	sqlite3_stmt*	stmt;
	int				rc;
	STRING		    sSQL;

#ifdef  _DEBUG
    fprintf(stderr, "get_branch_vector()\n");
#endif

	m_reaperVec.clear();

	sSQL = "SELECT branch, subbranch, checkout, owner, category, type, purpose from repos order by category, branch, subbranch";

	rc = sqlite3_prepare_v2( m_dbHandle, sSQL.c_str(), -1, &stmt, 0 );
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

			m_reaperVec.push_back(ReaperEntry(sBranchSpec, sRepoCmd, sRepoCat,
                                       sRepoType, sRepoOwner, sRepoDesc));
		}

		sqlite3_finalize(stmt);
	}

	return (m_reaperVec.size() != 0)?true:false;
}
/**
 *
 */

int ReaperObject::execute_shell_command(const STRING& sCmd, bool bEcho = true) {
//	int nRes = 0;
	FILE* ifp;

#ifdef  _DEBUG
    fprintf(stderr, "%s(%s)\n", __FUNCTION__, sCmd.c_str());
#endif

	ifp = popen( sCmd.c_str(), "r" );

	if (ifp) {
		while (!feof(ifp)) {
			char ch = fgetc(ifp);

			if (bEcho && !feof(ifp))
				printf("%c", ch);
		}
//		nRes = 1;
	}

	return 1;
}

/**
 *
 */

int ReaperObject::repo_init(const STRING& cmd) {
	return execute_shell_command( cmd );
}

/**
 *
 */


int ReaperObject::repo_sync(int options) {
	STRING sCmd;
	sCmd = "repo sync";

	return execute_shell_command( sCmd );
}

#pragma GCC diagnostic ignored "-Wunused-result"

/**
 *
 */

bool ReaperObject::Checkout_Branch(PARMS& parms)
{
    REAPERENTRY_VECTOR          *branchVec = &m_reaperVec;
    REAPERENTRY_VECTOR_ITER     spIter;

    assert(branchVec != 0L);

#ifdef  _DEBUG
    fprintf(stderr, "checkout_branch(..., %s, %s, %s)\n",
                    parms.m_sBranchName.c_str(),
                    parms.m_sCheckoutDir.c_str(),
                    parms.m_bDoCheckout?"T":"F");
#endif

    for (spIter = branchVec->begin() ; spIter != branchVec->end() ; spIter++) {
        if ((*spIter).m_sBranchSpec == parms.m_sBranchName) {
            if ((*spIter).m_sRepoType == "repo") {
                checkout_repo(parms, (*spIter));
            } else if ((*spIter).m_sRepoType == "git") {
                checkout_git(parms, (*spIter));
            } else {
#ifdef  _DEBUG
                fprintf(stderr, "ERROR: Invalid repo type [%s]!\n",
                        (*spIter).m_sRepoType.c_str());
#endif
                return false;
            }

            return true;
        }
    }

    return false;
}

/**
 *  Checkout using repo command.
 */

bool ReaperObject::checkout_repo(PARMS& parms, ReaperEntry& entry) {
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
                return false;
            }

            sCurDirName = get_current_dir_name();
            (void)chdir( parms.m_sCheckoutDir.c_str() );
        }

        printf("<%s>\n", sRepoCmd.c_str());

        repo_init( sRepoCmd );
        repo_sync();

        if (!parms.m_sWorkingBranchName.empty()) {
            repo_start(parms.m_sWorkingBranchName);
        }

        if (sCurDirName != 0) {
            /* Return to the previously saved directory */
            (void)chdir( sCurDirName );
            free( sCurDirName );
        }
    } else {
        printf("Repo Command:\n\t%s ; repo sync\n", sRepoCmd.c_str());
    }

    return true;
}

/**
 *  Checkout using git command.
 */

bool ReaperObject::checkout_git(PARMS& parms, ReaperEntry& entry) {
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
    return false;
}

#pragma GCC diagnostic warning "-Wunused-result"

size_t ReaperObject::Get_Branch_Count()
{
    return m_reaperVec.size();
}

/**
 *
 */

bool ReaperObject::Get_Branches(STRING_VECTOR& branchVec)
{
#ifdef  _DEBUG
    fprintf(stderr, "%s()\n", __FUNCTION__);
#endif

    REAPERENTRY_VECTOR_ITER bIter;

    for (bIter = m_reaperVec.begin() ; bIter != m_reaperVec.end() ; bIter++)
    {
        branchVec.push_back((*bIter).m_sBranchSpec);
    }

    return (branchVec.size() > 0)?true:false;
}

/**
 *
 */

//void ReaperObject::Set_Checkout_Directory(STRING sDirName)
//{
//    m_sCheckoutDirectory = sDirName;
//}

/**
 *
 */

STRING ReaperObject::get_field_from_stmt(sqlite3_stmt* stmt, int field) {
    STRING          sResult;
    const char*     szFieldData = 0L;

    szFieldData = (const char*)sqlite3_column_text(stmt, field);

    if (szFieldData != 0) {
        sResult = szFieldData;
    }

    return sResult;
}

/**
 *
 */

int ReaperObject::repo_start(const STRING& branchName) {
    STRING  sCmd;

    printf("Starting working branch %s...\n", branchName.c_str());
    sCmd = "repo start --all " + branchName;

    return execute_shell_command( sCmd );
}

/**
 *
 */

int ReaperObject::git_checkout(const STRING& cmd) {
	int nRes = execute_shell_command( cmd );

	return nRes;
}

/**
 *  Start a new branch in Git.
 */

int ReaperObject::git_start_branch(const STRING& sBranchName) {
    STRING sCmd = "git co -b " + sBranchName;
    printf("<%s>\n", sCmd.c_str());
    return execute_shell_command( sCmd );
}
