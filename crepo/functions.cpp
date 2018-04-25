#include "StdAfx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include "functions.h"
#include "crepo.h"

using namespace std;

/**
 *
 */

int execute_shell_command(const STRING& sCmd, bool bEcho) {
	FILE* ifp;
    int nRes = 0;

	ifp = popen( sCmd.c_str(), "r" );

	if (ifp) {
		while (!feof(ifp)) {
			char ch = fgetc(ifp);

			if (bEcho && !feof(ifp))
				printf("%c", ch);
		}
        nRes = pclose(ifp);
	}

	return nRes;
}


/**
 *
 */

int git_checkout(const STRING& cmd) {
    printf("Executing command %s\n", cmd.c_str());

	int nRes = execute_shell_command( cmd );

	return nRes;
}

int git_checkout(STRING sRepoURL,
                 STRING sBranchName,
                 STRING sCheckoutDir)
{
    STRING  sCmd;
    int     nRes = 0;

    printf("Cloning repo %s...\n", sRepoURL.c_str());

    sCmd = "git clone " + sRepoURL + " " + sCheckoutDir;

    nRes = execute_shell_command( sCmd );

#ifdef _DEBUG
    fprintf(stderr,"nRes = %d\n", nRes);
#endif // _DEBUG

    return nRes;
}
