#include <Windows.h>

#include "..\log\log.h"
#include "restorer.h"
#include "service.h"


extern TCHAR			*service_name;

SERVICE_STATUS			g_ServiceStatus;
SERVICE_STATUS_HANDLE	g_ServiceStatusHandle;
HANDLE                  g_ServiceStopEvent = 0;


//==============================================================================
//	FUNCTION DEFINITIONS
//==============================================================================

void InitService( DWORD dwArgc, LPTSTR *lpszArgv );
void ControlHandlerEx( DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext );
void UpdateServiceStatus( DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint );


//==============================================================================
//	MAIN
//==============================================================================

void ServiceMain( DWORD dwArgc, LPTSTR *lpszArgv )
{
	g_ServiceStatusHandle = RegisterServiceCtrlHandlerEx( 
		service_name, (LPHANDLER_FUNCTION_EX) ControlHandlerEx, 0 );

	if( g_ServiceStatusHandle == 0 )
		return;
	
	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	g_ServiceStatus.dwControlsAccepted =	SERVICE_ACCEPT_STOP | 
											SERVICE_ACCEPT_SHUTDOWN | 
											SERVICE_ACCEPT_HARDWAREPROFILECHANGE;

	RegisterDeviceInterfaceNotification( g_ServiceStatusHandle );

	UpdateServiceStatus( SERVICE_START_PENDING, NO_ERROR, 3000 );

	InitService( dwArgc, lpszArgv );
}

void InitService( DWORD dwArgc, LPTSTR *lpszArgv )
{
	g_ServiceStopEvent = CreateEvent( 0, TRUE, FALSE, 0 );

	if( !g_ServiceStopEvent )
	{
		UpdateServiceStatus( SERVICE_STOPPED, NO_ERROR, 0 );
		return;
	}

	UpdateServiceStatus( SERVICE_RUNNING, NO_ERROR, 0 );

	while( true )
	{
		WaitForSingleObject( g_ServiceStopEvent, INFINITE );
		UpdateServiceStatus( SERVICE_STOPPED, NO_ERROR, 0 );
		return;
	}
}

void ControlHandlerEx( DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext )
{
	switch( dwControl )
	{

	case SERVICE_CONTROL_STOP:
		UnregisterDeviceInterfaceNotification( );

		UpdateServiceStatus( SERVICE_STOP_PENDING, NO_ERROR, 0 );

		SetEvent( g_ServiceStopEvent );
		UpdateServiceStatus( g_ServiceStatus.dwCurrentState, NO_ERROR, 0 );

		WriteLog( L"Stopping" );
		break;

	case SERVICE_CONTROL_SHUTDOWN:
		UpdateServiceStatus( SERVICE_STOPPED, NO_ERROR, 0 );
		WriteLog( L"Shutdown" );
		break;

	case SERVICE_CONTROL_HARDWAREPROFILECHANGE:
		WriteLog( L"Hardware profile changed notification" );
		break;

	case SERVICE_CONTROL_DEVICEEVENT:
		WriteLog( L"Device event" );
		DeviceEventProc( dwEventType, lpEventData );
		break;

	case SERVICE_CONTROL_INTERROGATE:
		break;

	default:
		break;

	}
}

void UpdateServiceStatus( DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint )
{
	g_ServiceStatus.dwCurrentState = dwCurrentState;
	g_ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
	g_ServiceStatus.dwWaitHint;

	if( dwCurrentState == SERVICE_START_PENDING )
		g_ServiceStatus.dwControlsAccepted = 0;
	else
		g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	if( dwCurrentState & ( SERVICE_RUNNING | SERVICE_STOPPED ) )
		g_ServiceStatus.dwCheckPoint = 0;
	else
		g_ServiceStatus.dwCheckPoint++;

	SetServiceStatus( g_ServiceStatusHandle, &g_ServiceStatus );
}
