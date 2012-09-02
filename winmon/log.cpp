/************************************************************************/
/*                                                                      */
/************************************************************************/
#include <Windows.h>
#include <fstream>

#include "log.h"


/************************************************************************/
/* Check for already called log function
 *  it is needed to add timestamp for each service launch */
/************************************************************************/
bool WINAPI isLogAlreadyStarted( )
{
    static const TCHAR mutex_signature[] = _T( "686CFE74-8626-4BBA-8F78-DCA4747E8722" );

    CreateMutex( 0, FALSE, mutex_signature );

    return ( GetLastError( ) == ERROR_ALREADY_EXISTS );
}

/************************************************************************/
/* Return pointer to formatted date                                                                     */
/************************************************************************/
const TCHAR * WINAPI getDate( )
{
    static TCHAR date[9];

    _tstrdate_s( date );

    return date;
}

/************************************************************************/
/* Return pointer to formatted time                                                                     */
/************************************************************************/
const TCHAR * WINAPI getTime( )
{
    static TCHAR time[9];

    _tstrtime_s( time );

    return time;
}

/************************************************************************/
/* Compose log file name based on executable file name                                                                     */
/************************************************************************/
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

        TCHAR *ext_begin = _tcsrchr( log_name, _T( '.' ) ) + 1;
        _tcscpy_s( ext_begin, MAX_PATH - ( ext_begin - log_name ), _T( "log" ) );
        first_call = false;
    }

    return log_name;
}

/************************************************************************/
/* Log message to log file
 *  each message logged to file with timestamp
 *  each message followed with corresponding error description (if error_code is non-zero) */
/************************************************************************/
int WINAPI Log( const TCHAR *message, int error_code, const TCHAR *source_path )
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

        // check if log file already have any content and put new line to separate blocks
        if( log.rdbuf( )->pubseekoff( 0, std::ios_base::end ) > 0 )
        {
            log << std::endl;
        }

        log << _T("log started") << delimiter << getDate( ) << delimiter << getTime( ) << std::endl;
    }

    log << getTime( ) << delimiter;

    if( error_code == 0 )
    {
        log << message;
    }
    else
    {
        if( !FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_MAX_WIDTH_MASK, 
            0, error_code, MAKELANGID( LANG_NEUTRAL, SUBLANG_SYS_DEFAULT ), (LPTSTR) &error_buffer, 0, 0 ) )
            return GetLastError( );
        log << _T("Error: ") << message << delimiter << 
            _T("(") << error_code << _T(") ") << 
            error_buffer << delimiter << source_path;
        LocalFree( error_buffer );
    }
    
    // some (unicode?) symbols in message could fail output stream
    if( log.fail( ) )
    {
        log.close( );
        log.open( getLogFileName( ), std::ios::app );
    }

    log << std::endl;

    log.close( );

    return 0;
}
