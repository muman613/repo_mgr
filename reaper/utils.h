#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>


std::string     get_exe_path();
bool            file_exists(std::string sFilename);
int             make_directory(std::string sDirName);

#endif // __UTILS_H__
