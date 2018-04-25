#include "reaper.h"
#include "reapertypes.h"
#include "reaperEntry.h"

ReaperEntry::ReaperEntry(const STRING& sBranchSpec, const STRING& sCheckoutCmd,
                const STRING& sRepoCategory, const STRING& sRepoType,
                const STRING& sRepoOwner, const STRING& sDescription)
:   m_sBranchSpec(sBranchSpec),
    m_sCheckoutCmd(sCheckoutCmd),
    m_sDescription(sDescription),
    m_sRepoCategory(sRepoCategory),
    m_sRepoType(sRepoType),
    m_sRepoOwner(sRepoOwner)
{
    // ctor
}

ReaperEntry::~ReaperEntry()
{
    // dtor
}
