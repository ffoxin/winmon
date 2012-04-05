#define _CRT_SECURE_NO_WARNINGS


#include <Windows.h>
#include <fstream>
#include <iostream>
#include <locale>

#include "log.h"


const TCHAR log_file[]		= L"Global\\{5F676F6C-6966-656C-0000-0000C64A7D4F}";
const TCHAR log_console[]	= L"Global\\{5F676F6C-6F63-736E-6F6C-6500FB4A7D4F}";


bool __stdcall FirstCall( const TCHAR *name )
{
	HANDLE mutex = CreateMutex( 0, FALSE, name );
	int error = GetLastError( );
	
	return ( error != ERROR_ALREADY_EXISTS );
}


//==============================================================================
//	LOG FILE
//==============================================================================


int __stdcall WriteLog( const TCHAR *msg, const int error )
{
	static TCHAR		log_path[MAX_PATH]	= { };
	static bool			first_call			= FirstCall( log_file );

	std::wofstream		log;
	TCHAR				date[9];
	TCHAR				time[9];
	LPTSTR				err_buf;

	if( !*log_path )
	{
		if( !GetModuleFileName( 0, log_path, MAX_PATH ) )
			return -1;

		wcscpy( wcsrchr( log_path, L'.' ) + 1, L"log" );
	}

	log.open( log_path, std::ios::app );
	if( log.fail( ) )
		return -1;
	
	log.imbue( std::locale( "rus_rus" ) );

	if( first_call )
	{
		first_call = false;

		_wstrdate( date );
		if( log.rdbuf( )->pubseekoff( 0, std::ios_base::end ) > 0 )
			log << std :: endl;

		log << date << "\tlog started" << std::endl;
	}

	_wstrtime( time );
	log << time << "\t";

	if( error == 0 )
	{
		log << msg << std::endl;
	}
	else
	{
		if( !FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 
			0, error, MAKELANGID( LANG_NEUTRAL, SUBLANG_SYS_DEFAULT ), (LPTSTR) &err_buf, 0, 0 ) )
			return GetLastError( );

		log << "Error: " << msg << "\t" << "(" << error << ") " << err_buf;
		LocalFree( err_buf );
	}

	if( log.fail( ) )
	{
		log.close( );
		log.open( log_path, std::ios::app );
		log << std::endl;
	}
	
	log.close( );

	return 0;
}


//==============================================================================
//	LOG CONSOLE
//==============================================================================

int __stdcall WriteConsoleLog( const TCHAR *msg, const int error )
{
	static bool			first_call = FirstCall( log_console );

	std::wstreambuf		*buf;
	TCHAR				date[9];
	TCHAR				time[9];
	LPTSTR				err_buf;

	buf = std::wcout.rdbuf( );

	std::wcout.imbue( std::locale( ".866" ) );

	if( first_call )
	{
		first_call = false;

		_wstrdate( date );
		std::wcout << date << "\tlog started" << std::endl;
	}

	_wstrtime( time );
	std::wcout << time << "\t";
	
	if( error == 0 )
	{
		std::wcout << msg << std::endl;
	}
	else
	{
		if( !FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, 
			0, error, MAKELANGID( LANG_NEUTRAL, SUBLANG_SYS_DEFAULT ), (LPTSTR) &err_buf, 0, 0 ) )
			return GetLastError( );

		std::wcout << time << "\t" << L"Error: " << msg << L"\t" << "(" << error << ") " << err_buf;
		LocalFree( err_buf );
	}

	if( std::wcout.fail( ) )
	{
		std::wcout.rdbuf( buf );
		std::wcout << std::endl;
	}
	
	return 0;
}
