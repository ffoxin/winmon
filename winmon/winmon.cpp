/*	WinMonitor
*
*	Detects maximized windows and fix them when external display been plugged/unplugged
*
*	Vital Kolas
*	
*	ffoxin@gmail.com
*
*	Fri, 30.03.2012, 22:54 UTC+3
*/


#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <tchar.h>

#include "log.h"
#include "restorer.h"
#include "scm.h"
#include "service.h"
#include "winmon.h"


//==============================================================================
//	MAIN
//==============================================================================

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
        InstallService( );
    }
    else if( _tcscmp( argv[argc - 1], _T("remove") ) == 0 )
    {
        RemoveService( );
    }
    else if( _tcscmp( argv[argc - 1], _T("start") ) == 0 )
    {
        StartService( );
    }
    else if( _tcscmp( argv[argc - 1], _T("stop") ) == 0 )
    {
        StopService( );
    }
    else if( _tcscmp( argv[argc - 1], _T("fix") ) == 0 )
    {
        FixWindows( );
    }
}
