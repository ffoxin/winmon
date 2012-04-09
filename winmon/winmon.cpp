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

#include "..\log\log.h"
#include "restorer.h"
#include "scm.h"
#include "service.h"
#include "winmon.h"

#pragma comment(lib, "..\\Debug\\log.lib")


//==============================================================================
//	MAIN
//==============================================================================

void __cdecl _tmain( int argc, TCHAR *argv[] )
{
	if( argc == 1 )
	{
		SERVICE_TABLE_ENTRY DispatchTable[] = 
		{
			{ service_name, (LPSERVICE_MAIN_FUNCTION) ServiceMain }, 
			{ 0, 0 }
		};

		if( !StartServiceCtrlDispatcher( DispatchTable ) )
		{
			WriteLogE( L"_tmain::StartServiceCtrlDispatcher" );
		}
	}
	else if( wcscmp( argv[argc - 1], L"install" ) == 0 )
	{
		InstallService( );
	}
	else if( wcscmp( argv[argc - 1], L"remove" ) == 0 )
	{
		RemoveService( );
	}
	else if( wcscmp( argv[argc - 1], L"start" ) == 0 )
	{
		StartService( );
	}
	else if( wcscmp( argv[argc - 1], L"stop" ) == 0 )
	{
		StopService( );
	}
	else if( wcscmp( argv[argc - 1], L"fix" ) == 0 )
	{
		WriteLog( L"running fix" );
		FixWindows( );
	}
}
