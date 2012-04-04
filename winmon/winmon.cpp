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


#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <Windows.h>
#include <tchar.h>

#include "..\log\log.h"
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
			WriteLog( L"_tmain::StartServiceCtrlDispatcher", GetLastError( ) );
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
}


#ifdef _MSC_VER
	#undef _CRT_SECURE_NO_WARNINGS
#endif
