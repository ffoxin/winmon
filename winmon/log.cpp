/************************************************************************/
/*                                                                      */
/************************************************************************/
#include <Windows.h>
#include <fstream>

#include "log.h"


/*
*const TCHAR const log_file[]      = L"Global\\{5F676F6C-6966-656C-0000-0000C64A7D4F}";
const TCHAR const tab[]           = L"\t";


bool __stdcall FirstCall( const TCHAR *name )
{
HANDLE mutex = CreateMutex( 0, FALSE, name );
int error = GetLastError( );

return ( error != ERROR_ALREADY_EXISTS );
}

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
}*/

const TCHAR mutex_signature[] = _T( "686CFE74-8626-4BBA-8F78-DCA4747E8722" );

bool WINAPI isLogAlreadyStarted( )
{
    HANDLE mutex = CreateMutex( 0, FALSE, mutex_signature );
    int error = GetLastError( );

    return ( error == ERROR_ALREADY_EXISTS );
}

const TCHAR * WINAPI getDate( )
{
    static TCHAR date[9];

    _wstrdate( date );

    return date;
}

const TCHAR * WINAPI getTime( )
{
    static TCHAR time[9];

    _wstrtime( time );

    return time;
}

const TCHAR * WINAPI getLogFileName( )
{
    static bool first_call = true;
    static TCHAR log_name[MAX_PATH];

    if( first_call )
    {
        if( !GetModuleFileName( 0, log_name, MAX_PATH ) )
        {
            return 0;
        }

        _tcscpy( _tcsrchr( log_name, _T( '.' ) ) + 1, _T( "log" ) );
        first_call = false;
    }

    return log_name;
}

int WINAPI Log( const TCHAR *msg )
{
    static const TCHAR delimiter[] = _T( " " );

    static bool     is_started = isLogAlreadyStarted( );

    std::wofstream  log;
    TCHAR           *error_buffer;

    log.open( getLogFileName( ), std::ios::app );
    if( log.fail( ) )
    {
        return -1;
    }

    log.imbue( std::locale( "rus_rus" ) );

    if( !is_started )
    {
        is_started = true;

        _wstrdate( date );
        if( log.rdbuf( )->pubseekoff( 0, std::ios_base::end ) > 0 )
        {
            log << std::endl;
        }

        log << L"log started" << delimiter << getDate( ) << delimiter << getTime( ) << std::endl;
    }

    log << getTime( ) << delimiter;

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
