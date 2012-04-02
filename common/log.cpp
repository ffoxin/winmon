#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
#endif


#include <Windows.h>
#include <fstream>
#include <iostream>

#include "log.h"


static TCHAR log_path[MAX_PATH];


//==============================================================================
//	LOG FILE
//==============================================================================

int InitLog( )
{
	if( !GetModuleFileName( 0, log_path, MAX_PATH ) )
		return -1;

	wcscpy( wcsrchr( log_path, L'.' ) + 1, L"log" );

	return 0;
}

int WriteLog( TCHAR *msg )
{
	std::wofstream	log;
	TCHAR			time[9];
	
	log.open( log_path, std::ios::app );
	if( log.fail( ) )
		return -1;

	_wstrtime( time );

	log << time << "\t" << msg << std::endl;

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
	
	_wstrtime( time );

	std::wcout << time << "\t" << msg << std::endl;

	return 0;
}

int WriteConsoleLog( TCHAR *msg, DWORD msg_id )
{
	TCHAR			time[9];
	LPTSTR			err_buf;
	
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
