#ifndef __REAPERENTRY_H__
#define __REAPERENTRY_H__

#include "reapertypes.h"

/**
 *
 */

class ReaperEntry {
public:
    ReaperEntry(const STRING& sBranchSpec, const STRING& sCheckoutCmd,
                const STRING& sRepoCategory, const STRING& sRepoType,
                const STRING& sRepoOwner, const STRING& sRepoDesc);
    ~ReaperEntry();

    STRING  m_sBranchSpec;
    STRING  m_sCheckoutCmd;
    STRING  m_sDescription;
    STRING  m_sRepoCategory;
    STRING  m_sRepoType;                // git or repo
    STRING  m_sRepoOwner;
};

typedef std::vector<ReaperEntry>            REAPERENTRY_VECTOR;
typedef std::vector<ReaperEntry>::iterator  REAPERENTRY_VECTOR_ITER;


#endif // __REAPERENTRY_H__
