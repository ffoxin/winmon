#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
#endif


#include <Windows.h>
#include <fstream>
#include <iostream>
#include <locale>

#include "log.h"


static	TCHAR	log_path[MAX_PATH]		= { };
static	bool	first_call				= true;


//==============================================================================
//	LOG FILE
//==============================================================================

int WriteLog( const TCHAR *msg, const int error )
{
	std::wofstream		log;
	TCHAR				date[9];
	TCHAR				time[9];
	LPTSTR				err_buf;

	if( *log_path == 0 )
	{
		if( !GetModuleFileName( 0, log_path, MAX_PATH ) )
			return -1;

		wcscpy( wcsrchr( log_path, L'.' ) + 1, L"log" );

		//_wunlink( log_path );
	}

	log.open( log_path, std::ios::app );
	if( log.fail( ) )
		return -1;
	
	log.imbue( std::locale( "rus_rus" ) );

	_wstrdate( date );
	_wstrtime( time );

	if( first_call )
	{
		first_call = false;
		if( log.rdbuf( )->pubseekoff( 0, std::ios_base::end ) > 0 )
			log << std :: endl;
		log << "log started " << date << std::endl;
	}

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

int WriteConsoleLog( const TCHAR *msg, const int error )
{
	std::wstreambuf		*buf;
	TCHAR				time[9];
	LPTSTR				err_buf;

	buf = std::wcout.rdbuf( );

	std::wcout.imbue( std::locale( ".866" ) );

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


#ifdef _MSC_VER
	#undef _CRT_SECURE_NO_WARNINGS
#endif
