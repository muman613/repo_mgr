#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include <string>
#include "crepo.h"


int execute_shell_command(const STRING& sCmd, bool bEcho = true);
int git_checkout(const STRING& cmd);
int git_checkout(STRING sRepoURL,
                 STRING sBranchName,
                 STRING sCheckoutDir);

#endif // __FUNCTIONS_H__
