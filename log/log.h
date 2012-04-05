#ifndef LOG_H_1522_01042012
#define LOG_H_1522_01042012


#ifndef _TCHAR_DEFINED
	#ifdef UNICODE
		typedef	wchar_t	TCHAR;
	#else
		typedef	char	TCHAR;
	#endif
#endif


//==============================================================================
//	Logging to file
//==============================================================================

int __stdcall WriteLog( const TCHAR *msg, const int error = 0 );


//==============================================================================
//	Logging to console
//==============================================================================

int __stdcall WriteConsoleLog( const TCHAR *msg, const int error = 0 );


#endif // LOG_H_1522_01042012
