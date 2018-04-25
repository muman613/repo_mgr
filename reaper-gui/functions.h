#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include "reaper.h"
#include "reaperEntry.h"

/**
 *  Parameters for checkout...
 */

typedef struct checkout_parms {
    REAPERENTRY_VECTOR* m_entryVector;
    STRING              m_sBranchName;
    STRING              m_sCheckoutDir;
    STRING              m_sWorkingBranchName;
    STRING              m_sCheckoutDate;
//  bool                m_bDoCheckout;
} CHECKOUT_PARMS;


typedef void (*OUTCHARFUNC)(char ch, void* pUserData);
typedef void (*OUTPRINTFFUNC)(void* pUserData, const char* sFormat, ...);

/**
 *
 */

int execute_shell_command(const STRING& sCmd, bool bEcho);
int repo_init(const STRING& cmd);
int repo_sync(int options = 0);
int repo_rollback_date(STRING sDateSpec,
                       STRING sNewBranchSpec,
                       STRING sParentBranchSpec);
int git_checkout(const STRING& cmd);

bool get_repodb_path(STRING& sRepoDbPath);
bool open_database(STRING sDBname, sqlite3** dbHandle);
bool get_database_version(sqlite3* dbHandle, STRING& sVersion, STRING& sDate);
bool get_branch_vector(sqlite3* dbHandle, REAPERENTRY_VECTOR& sVec);
void list_branches(REAPERENTRY_VECTOR& branchVec, const STRING& sCategory = "all");
//int  checkout_branch(REAPERENTRY_VECTOR&  branchVec,
//                            const STRING& sBranchName,
//                            const STRING& sCheckOutDir,
//                            const STRING& sWorkingBranch,
//                            bool doCheckout);
int checkout_branch(CHECKOUT_PARMS& parms);

void close_database(sqlite3* dbHandle);

STRING get_field_from_stmt(sqlite3_stmt* stmt, int field);

int checkout_repo(CHECKOUT_PARMS& parms, ReaperEntry& entry);
int checkout_git(CHECKOUT_PARMS& parms, ReaperEntry& entry);
int checkout_crepo(CHECKOUT_PARMS& parms, ReaperEntry& entry);

int launch_video_playback();

void set_output_char_func(OUTCHARFUNC pFunc);
void set_printf_func(OUTPRINTFFUNC pFunc);
void set_user_data(void* pUserData);

extern const STRING sAllString;



#endif // __FUNCTIONS_H__
