#ifndef log_macro_h__
#define log_macro_h__


/************************************************************************/
/* Debug macro                                                                     */
/************************************************************************/
// line number to wide string
#define WINT2(x) L## #x
#define WINT(x) WINT2(x)
#define WLINE WINT(__LINE__)

// source.cpp to wide string
#define WSTR2(x) L ## x
#define WSTR(x) WSTR2(x)
#define WFILE WSTR(__FILE__)

// concatenate source.cpp:line_number
#define __DBG_INFO__ WFILE L":" WLINE


#endif // log_macro_h__
