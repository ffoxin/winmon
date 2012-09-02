#ifndef log_h__
#define log_h__

#include <tchar.h>


/************************************************************************/
/* Log macro
 *  used to get error point in format:
 *  source_file:source_line_number */
/************************************************************************/
// line number to wide string
#define TINT2(x) _T(## #x )
#define TINT(x) TINT2(x)
#define TLINE TINT(__LINE__)

// source file name to wide string
#define TSTR2(x) _T(## x )
#define TSTR(x) TSTR2(x)
#define TFILE TSTR(__FILE__)

// concatenate source file name and line number
#define __SOURCE__ TFILE _T(":") TLINE

/************************************************************************/
/* Logging                                                                     */
/************************************************************************/
int WINAPI Log( const wchar_t *message, int error_code = 0, const TCHAR *source_path = __SOURCE__ );


#endif // log_h__
