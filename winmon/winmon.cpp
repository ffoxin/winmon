/*
 * <Fri, 30.03.2012, 22:54 UTC+3>
 * 
 * WinMon - Windows Monitor
 * 
 * Detects and restores affected maximized windows while external display being plugged/unplugged
 * 
 * Vital Kolas
 * 
 * ffoxin@gmail.com
 * 
 * 2012
 */

#include <Windows.h>
#include <tchar.h>

#include "log.h"
#include "restorer.h"
#include "scm.h"
#include "service.h"


/************************************************************************/
/* Service name
 *  this name will be displayed in SCM (Service Control Manager) */
/************************************************************************/
TCHAR service_name[] = _T("DualMon");


/************************************************************************/
/* Main entry point                                                                     */
/************************************************************************/
void __cdecl _tmain( int argc, TCHAR *argv[] )
{
    if( argc == 1 )
    {
        SERVICE_TABLE_ENTRY DispatchTable[] = {
            { service_name, (LPSERVICE_MAIN_FUNCTION) ServiceMain }, 
            { 0, 0 } };

        if( !StartServiceCtrlDispatcher( DispatchTable ) )
        {
            Log( _T("_tmain::StartServiceCtrlDispatcher"), GetLastError( ) );
        }
    }
    else if( _tcscmp( argv[argc - 1], _T("install") ) == 0 )
    {
        ServiceInstall( );
    }
    else if( _tcscmp( argv[argc - 1], _T("remove") ) == 0 )
    {
        ServiceRemove( );
    }
    else if( _tcscmp( argv[argc - 1], _T("start") ) == 0 )
    {
        ServiceStart( );
    }
    else if( _tcscmp( argv[argc - 1], _T("stop") ) == 0 )
    {
        ServiceStop( );
    }
    else if( _tcscmp( argv[argc - 1], _T("fix") ) == 0 )
    {
        FixWindows( );
    }
}
