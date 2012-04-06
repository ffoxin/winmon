#ifndef LOG_H_1522_01042012
#define LOG_H_1522_01042012


#ifndef _TCHAR_DEFINED
	#ifdef UNICODE
		typedef	wchar_t	TCHAR;
	#else
		typedef	char	TCHAR;
	#endif
#endif

#define WINT(x) L## #x
#define TOWINT(x) WINT(x)
#define __WLINE__ TOWINT(__LINE__)

#define WSTR(x) L ## x
#define TOWSTR(x) WSTR(x)
#define __WFILE__ TOWSTR(__FILE__)

#define __WCOLON__ L":"

#define __WFILELINE__ __WFILE__ __WCOLON__ __WLINE__
		
#define WriteLogE(x)			WriteLog( x, GetLastError( ), __WFILELINE__ )
#define WriteLogP(x)			WriteLog( WSTR( #x ), GetLastError( ), __WFILELINE__ )
#define WriteLogC(x)			if( x ) WriteLog( WSTR(#x) )
#define WriteLogCE(x)			if( x ) WriteLogP( x )

#define WriteConsoleLogE(x)		WriteConsoleLog( x, GetLastError( ), __WFILELINE__ )
#define WriteConsoleLogP(x)		WriteConsoleLog( WSTR( #x ), GetLastError( ), __WFILELINE__ )
#define WriteConsoleLogC(x)		if( x ) WriteConsoleLog( WSTR(#x) )
#define WriteConsoleLogCE(x)	if( x ) WriteConsoleLogP( x )


//==============================================================================
//	Logging to file
//==============================================================================

int __stdcall WriteLog( const TCHAR *msg, const int error = 0, const TCHAR *location = 0 );


//==============================================================================
//	Logging to console
//==============================================================================

int __stdcall WriteConsoleLog( const TCHAR *msg, const int error = 0, const TCHAR *location = 0 );


#endif // LOG_H_1522_01042012
