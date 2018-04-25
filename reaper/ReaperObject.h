#ifndef __REAPEROBJECT_H__
#define __REAPEROBJECT_H__

#include <stdio.h>
#include "sqlite3.h"
#include "reapertypes.h"
#include "reaperEntry.h"
//#include "checkoutParms.h"

/**
 *
 */

class ReaperObject
{
    public:
        typedef struct _parms {
            STRING              m_sBranchName;
            STRING              m_sCheckoutDir;
            STRING              m_sWorkingBranchName;
//          bool                m_bDoCheckout;
        } PARMS;

        ReaperObject();
        ReaperObject(STRING sRepoDBname);

        virtual ~ReaperObject();

        bool                    Open(STRING sRepoDBName);
        void                    Close();
        bool                    IsValid();

#ifdef  _DEBUG
        int                     Dump_Object(FILE* ofp = stdout);
#endif

//        void                    Set_Checkout_Directory(STRING sDirName);

        size_t                  Get_Branch_Count();
        bool                    Get_Branches(STRING_VECTOR& branchVec);
        STRING                  Get_RepoDB_Path();

        bool                    Checkout_Branch(PARMS& parms);

    protected:




    private:

        int                     execute_shell_command(const STRING& sCmd, bool bEcho);
        int                     repo_init(const STRING& cmd);
        int                     repo_sync(int options = 0);
        int                     repo_start(const STRING& branchName);
        int                     git_checkout(const STRING& sCmd);
        int                     git_start_branch(const STRING& branchName);
        STRING                  get_field_from_stmt(sqlite3_stmt* stmt, int field);
        void                    list_branches(STRING_PAIR_VECTOR& branchVec);



        bool                    get_branch_vector();
        bool                    get_repodb_path();

        bool                    open_database();
        void                    close_database();

        bool                    checkout_repo(PARMS& parms, ReaperEntry& entry);
        bool                    checkout_git(PARMS& parms, ReaperEntry& entry);

        bool                    m_bIsValid;
        STRING                  m_sRepoDatabaseName;
        STRING                  m_sRepoDatabasePath;
//        STRING                  m_sCheckoutDirectory;
//        STRING                  m_sCheckoutBranchName;

        //STRING_PAIR_VECTOR      m_branchVec;
        REAPERENTRY_VECTOR      m_reaperVec;
        //PARMS                   m_parms;

        sqlite3*                m_dbHandle;
};

#endif // __REAPEROBJECT_H__
