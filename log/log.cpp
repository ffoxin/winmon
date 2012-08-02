#define _CRT_SECURE_NO_WARNINGS


#include <Windows.h>
#include <fstream>
#include <iostream>
#include <locale>
#include <tchar.h>

#include "log.h"


const TCHAR const log_file[]      = L"Global\\{5F676F6C-6966-656C-0000-0000C64A7D4F}";
const TCHAR const tab[]           = L"\t";


bool __stdcall FirstCall( const TCHAR *name )
{
    HANDLE mutex = CreateMutex( 0, FALSE, name );
    int error = GetLastError( );

    return ( error != ERROR_ALREADY_EXISTS );
}


//==============================================================================
//  LOG FILE
//==============================================================================

int __stdcall WriteLog( const TCHAR *msg, const int error, const TCHAR *location )
{
    static TCHAR        log_path[MAX_PATH]  = { };
    static bool         first_call          = FirstCall( log_file );

    std::wofstream      log;
    TCHAR               date[9];
    TCHAR               time[9];
    LPTSTR              err_buf;

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

        log << date << tab << L"log started" << std::endl;
    }

    _wstrtime( time );
    log << time << tab;
    
    if( error == 0 )
    {
        log << msg;
    }
    else
    {
        if( !FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_MAX_WIDTH_MASK, 
            0, error, MAKELANGID( LANG_NEUTRAL, SUBLANG_SYS_DEFAULT ), (LPTSTR) &err_buf, 0, 0 ) )
            return GetLastError( );
        log << L"Error: " << msg << tab << 
            L"(" << error << L") " << 
            err_buf << tab << location;
        LocalFree( err_buf );
    }

    if( log.fail( ) )
    {
        log.close( );
        log.open( log_path, std::ios::app );
    }
    
    log << std::endl;

    log.close( );

    return 0;
}


//==============================================================================
//  LOG CONSOLE
//==============================================================================

int __stdcall WriteConsoleLog( const TCHAR *msg, const int error, const TCHAR *location )
{
    std::wstreambuf     *buf;
    TCHAR               time[9];
    LPTSTR              err_buf;

    buf = std::wcout.rdbuf( );

    std::wcout.imbue( std::locale( ".866" ) );

    _wstrtime( time );
    std::wcout << time << tab;
    
    if( error == 0 )
    {
        std::wcout << msg;
    }
    else
    {
        if( !FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_MAX_WIDTH_MASK, 
            0, error, MAKELANGID( LANG_NEUTRAL, SUBLANG_SYS_DEFAULT ), (LPTSTR) &err_buf, 0, 0 ) )
            return GetLastError( );
        std::wcout << L"Error: " << msg << tab << 
            L"(" << error << L") " << 
            err_buf << tab << location;
        LocalFree( err_buf );
    }

    if( std::wcout.fail( ) )
    {
        std::wcout.rdbuf( buf );
    }

    std::wcout << std::endl;

    return 0;
}
