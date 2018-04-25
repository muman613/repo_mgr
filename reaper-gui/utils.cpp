/**
 *      @file       utils.cpp
 *      @author     Michael A. Uman
 *      @date       May 24, 2013
 */

#include "reaper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "utils.h"

using namespace std;

/**
 *
 */

string get_exe_path() {
    string  sPath;
    pid_t   pid;
    char    sProcPath[128];
    char    buf[256];
    string  sExePath;

    pid = getpid();

    sprintf(sProcPath, "/proc/%d/exe", pid);
    if (readlink(sProcPath, buf, sizeof(buf)) != -1) {
        sPath = dirname(buf);
    } else {
        perror("get_exe_path");
    }

    return sPath;
}

/**
 *
 */

bool file_exists(string sFilename) {
    bool bRes =false;
    struct stat statbuf;

    if (stat( sFilename.c_str(), &statbuf ) == 0) {
        bRes = true;
    }

    return bRes;
}

int make_directory(string sDirName) {
//    int     nRes = 0;
    struct stat statbuf;

    if (stat( sDirName.c_str(), &statbuf ) == 0) {
        /* check if a file is in the way */
        if (S_ISREG(statbuf.st_mode)) {
#ifdef  _DEBUG
            fprintf(stderr, "WARNING: Cannot create directory because file exists with name '%s'\n", sDirName.c_str());
#endif
            return -10;
        }
        /* check if directory already exists */
        if (S_ISDIR(statbuf.st_mode)) {
#ifdef  _DEBUG
            fprintf(stderr, "WARNING: Directory '%s' already exists!\n", sDirName.c_str());
#endif
            return -20;
        }
    }

    return mkdir(sDirName.c_str(), S_IRWXU|S_IRWXG|S_IROTH|S_IWOTH);
}

#define READ 0
#define WRITE 1

/**
 *  popen2 open pipes to launched process.
 */

pid_t popen2(const char *command, int *infp, int *outfp) {
    int p_stdin[2],
        p_stdout[2];
    pid_t pid;

    if (pipe(p_stdin) != 0 || pipe(p_stdout) != 0)
        return -1;

    pid = fork();

    if (pid < 0)
        return pid;
    else if (pid == 0) {
        close(p_stdin[WRITE]);
        dup2(p_stdin[READ], READ);
        close(p_stdout[READ]);
        dup2(p_stdout[WRITE], WRITE);
        execl("/bin/sh", "sh", "-c", command, NULL);
        perror("execl");
        exit(1);
    }

    if (infp == NULL)
        close(p_stdin[WRITE]);
    else
        *infp = p_stdin[WRITE];

    if (outfp == NULL)
        close(p_stdout[READ]);
    else
        *outfp = p_stdout[READ];

    return pid;
}

//#define ENABLE_MY_POPEN

#ifdef ENABLE_MY_POPEN

static pid_t *pids    = nullptr;

/**
 *
 */

FILE* _popen(const char* program, register const char* type) {
	register FILE *iop;
	int pdes[2], fds, pid;

	if (((*type != 'r') && (*type != 'w')) || type[1])
		return (NULL);

	if (pids == NULL) {
		if ((fds = getdtablesize()) <= 0)
			return (NULL);
		if ((pids = (pid_t *)malloc((u_int)(fds * sizeof(pid_t)))) == NULL)
			return (NULL);
		bzero((char *)pids, fds * sizeof(int));
	}

	if (pipe(pdes) < 0) {
		perror("pipe");
		return (NULL);
	}

	switch (pid = vfork()) {
	case -1:			/* error */
		(void) close(pdes[0]);
		(void) close(pdes[1]);
		return (NULL);
		/* NOTREACHED */
	case 0:				/* child */
		if (*type == 'r') {
			if (pdes[1] != fileno(stdout)) {
				(void) dup2(pdes[1], fileno(stdout));
				(void) close(pdes[1]);
			}
			(void) close(pdes[0]);
		} else {
			if (pdes[0] != fileno(stdin)) {
				(void) dup2(pdes[0], fileno(stdin));
				(void) close(pdes[0]);
			}
			(void) close(pdes[1]);
		}
		execl("/bin/sh", "sh", "-c", program, NULL);
		_exit(127);
		/* NOTREACHED */
	}
	/* parent; assume fdopen can't fail...  */
	if (*type == 'r') {
		iop = fdopen(pdes[0], type);
		(void) close(pdes[1]);
	} else {
		iop = fdopen(pdes[1], type);
		(void) close(pdes[0]);
	}
	pids[fileno(iop)] = pid;
	return (iop);
}

/**
 *
 */

int _pclose(FILE* iop) {
	register int fdes;
	sigset_t omask, nmask;
	union wait pstat;
	register int pid;

	/*
	 * pclose returns -1 if stream is not associated with a
	 * `popened' command, if already `pclosed', or waitpid
	 * returns an error.
	 */
	if (pids == NULL || pids[fdes = fileno(iop)] == 0)
		return (-1);
	(void) fclose(iop);
	sigemptyset(&nmask);
	sigaddset(&nmask, SIGINT);
	sigaddset(&nmask, SIGQUIT);
	sigaddset(&nmask, SIGHUP);
	(void) sigprocmask(SIG_BLOCK, &nmask, &omask);
	do {
		pid = waitpid(pids[fdes], (int *) &pstat, 0);
	} while (pid == -1 && errno == EINTR);
	(void) sigprocmask(SIG_SETMASK, &omask, NULL);
	pids[fdes] = 0;
	return (pid == -1 ? -1 : pstat.w_status);
}

/**
 *
 */

int _pipe_pid(FILE* iop) {
    register int fdes;

	if (pids == NULL || pids[fdes = fileno(iop)] == 0)
		return (-1);

    return pids[fdes];
}
#endif // ENABLE_MY_POPEN
