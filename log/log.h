#ifndef log_h_1522_01042012
#define log_h_1522_01042012


#include <tchar.h>

#include "log_macro.h"


/************************************************************************/
/* Log types                                                                     */
/************************************************************************/
enum e_LogType_t { e_Info, e_Warning, e_Error, e_LogTypeCount };
enum e_LogTarget_t { e_Console, e_File, e_LogTargetCount };

/************************************************************************/
/* Log settings                                                                     */
/************************************************************************/
/************************************************************************/
/*          | Console   | File
 * Info     |   +       |   -
 * Warning  |   +       |   -
 * Error    |   +       |   +   */
/************************************************************************/
bool write_settings[e_LogTypeCount][e_LogTargetCount] = {
    { true, false },
    { true, false },
    { true, true }
};

/************************************************************************/
/* Write log                                                                     */
/************************************************************************/
int WINAPI WriteLog(const TCHAR *message, e_LogType_t log_type = e_Info, int error_code = 0);


#endif // log_h_1522_01042012
