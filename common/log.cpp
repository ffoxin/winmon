#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
#endif


#include <Windows.h>
#include <fstream>
#include <iostream>
#include <locale>

#include "log.h"


#ifndef IMPORT
	#define IMPORT(type)		extern "C" __declspec(dllimport) type __stdcall
#endif

#ifndef _WINDEF_
	#define MAX_PATH          260
	typedef unsigned long DWORD;
	typedef struct {
		int unused;
	} *HMODULE;
#endif

#ifdef UNICODE
	#ifndef _TCHAR_DEFINED
		typedef wchar_t TCHAR;
	#endif
	#ifndef GetModuleFileName
	IMPORT(DWORD) GetModuleFileNameW( HMODULE hModule, const TCHAR lpFilename, DWORD nSize );
	#define GetModuleFilName GetModuleFileNameW
	#endif
static	TCHAR	log_path[MAX_PATH];
#else
	#ifndef VOID
		typedef char CHAR;
	#endif
	#ifndef GetModuleFileName
	IMPORT(DWORD) GetModuleFileNameA( HMODULE hModule, const CHAR lpFilename, DWORD nSize );
	#define GetModuleFilName GetModuleFileNameA
	#endif
static	CHAR	log_path[MAX_PATH];
#endif


//==============================================================================
//	LOG FILE
//==============================================================================

int InitLog( bool clear )
{
	if( !GetModuleFileName( 0, log_path, MAX_PATH ) )
		return -1;

	wcscpy( wcsrchr( log_path, L'.' ) + 1, L"log" );

	if( clear )
		_wunlink( log_path );

	return 0;
}

int WriteLog( TCHAR *msg )
{
	std::wofstream	log;
	TCHAR			time[9];
	
	log.open( log_path, std::ios::app );
	if( log.fail( ) )
		return -1;

	log.imbue( std::locale( "rus_rus" ) );

	_wstrtime( time );

	log << time << "\t" << msg << std::endl;
	if( log.fail( ) )
	{
		log.close( );
		log.open( log_path, std::ios::app );
		log << std::endl;
	}
	
	log.close( );

	return 0;
}

int WriteLog( TCHAR *msg, DWORD msg_id )
{
	std::wofstream	log;
	TCHAR			time[9];
	LPTSTR			err_buf;
	
	log.open( log_path, std::ios::app );
	if( log.fail( ) )
		return -1;

	_wstrtime( time );

	if( !FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 
		0, msg_id, MAKELANGID( LANG_NEUTRAL, SUBLANG_SYS_DEFAULT ), (LPWSTR) &err_buf, 0, 0 ) )
		return GetLastError( );

	log << time << "\t" << L"Error: " << msg << L"\t" << "(" << msg_id << ") " << err_buf;
	LocalFree( err_buf );

	log.close( );

	return 0;
}


//==============================================================================
//	LOG CONSOLE
//==============================================================================

int WriteConsoleLog( TCHAR *msg )
{
	TCHAR			time[9];
	
	std::wcout.imbue( std::locale( ".866" ) );

	_wstrtime( time );

	std::wcout << time << "\t" << msg << std::endl;
	if( std::wcout.fail( ) )
		std::wcout << std::endl;

	return 0;
}

int WriteConsoleLog( TCHAR *msg, DWORD msg_id )
{
	TCHAR			time[9];
	LPTSTR			err_buf;
	
	std::wcout.imbue( std::locale( ".866" ) );

	_wstrtime( time );

	if( !FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 
		0, msg_id, MAKELANGID( LANG_NEUTRAL, SUBLANG_SYS_DEFAULT ), (LPWSTR) &err_buf, 0, 0 ) )
		return GetLastError( );

	std::wcout << time << "\t" << L"Error: " << msg << L"\t" << "(" << msg_id << ") " << err_buf;
	LocalFree( err_buf );

	return 0;
}


#ifdef _MSC_VER
	#undef _CRT_SECURE_NO_WARNINGS
#endif
