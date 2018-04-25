#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>

#define ENABLE_MY_POPEN 1

std::string     get_exe_path();
bool            file_exists(std::string sFilename);
int             make_directory(std::string sDirName);
pid_t           popen2(const char *command, int *infp, int *outfp);

#ifdef ENABLE_MY_POPEN
FILE*           _popen(const char* program, register const char* type);
int             _pclose(FILE* iop);
int             _pipe_pid(FILE* iop);
#endif // ENABLE_MY_POPEN

#endif // __UTILS_H__
