#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include "crepo.h"

enum {
    FILE_FLAG = (1L << 0),
    DIR_FLAG  = (1L << 1),
};

STRING          get_exe_path();
bool            file_or_dir_exists(STRING sFilename, int flags);
int             make_directory(STRING sDirName);
bool            get_current_directory(STRING& sPath);

/* ANSI utils */
extern const char *ansiReset;
extern const char *ansiBoldBlack;
extern const char *ansiBoldRed;
extern const char *ansiBoldGreen;
extern const char *ansiBoldBrown;
extern const char *ansiBoldBlue;
extern const char *ansiBoldMagenta;
extern const char *ansiBoldCyan;
extern const char *ansiBoldGrey;

#endif // __UTILS_H__
