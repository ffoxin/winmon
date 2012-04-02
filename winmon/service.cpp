#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <Windows.h>
#include <Dbt.h>
#include <sstream>

#include "..\common\log.h"
#include "restorer.h"
#include "service.h"


extern TCHAR			*service_name;

SERVICE_STATUS			g_ServiceStatus;
SERVICE_STATUS_HANDLE	g_ServiceStatusHandle;
HANDLE                  g_ServiceStopEvent = 0;
HDEVNOTIFY				g_DeviceNotify;
size_t					g_WinCount;
size_t					g_WinMaxCount;


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

	DEV_BROADCAST_DEVICEINTERFACE dev_int;
	dev_int.dbcc_size			= sizeof( DEV_BROADCAST_DEVICEINTERFACE );
	dev_int.dbcc_devicetype		= DBT_DEVTYP_DEVICEINTERFACE;

	g_DeviceNotify = RegisterDeviceNotification( g_ServiceStatusHandle, &dev_int, 
		DEVICE_NOTIFY_SERVICE_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES );

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
	std::wstringstream				output;

	switch( dwControl )
	{

	case SERVICE_CONTROL_STOP:
		UpdateServiceStatus( SERVICE_STOP_PENDING, NO_ERROR, 0 );

		SetEvent( g_ServiceStopEvent );
		UpdateServiceStatus( g_ServiceStatus.dwCurrentState, NO_ERROR, 0 );

		UnregisterDeviceNotification( g_DeviceNotify );
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
		PDEV_BROADCAST_HDR				dev_hdr;
		PDEV_BROADCAST_DEVICEINTERFACE	dev_int;
		TCHAR							*is_display;
		
		dev_hdr = (PDEV_BROADCAST_HDR) lpEventData;
		if( dev_hdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE )
			dev_int = (PDEV_BROADCAST_DEVICEINTERFACE) dev_hdr;

		output << L"Device event\t";

		if( !( dwEventType & ( DBT_DEVICEARRIVAL | DBT_DEVICEREMOVECOMPLETE ) ) )
			return;

		is_display = wcsstr( dev_int->dbcc_name, detect_token );
		if( is_display != 0 )
		{
			output << detect_token << L"\t";
			
			g_WinCount = 0;
			g_WinMaxCount = 0;

			HWINSTA hWinSta;
			hWinSta = OpenWindowStation( L"WinSta0", FALSE, 
				WINSTA_ACCESSCLIPBOARD		| 
				WINSTA_ACCESSGLOBALATOMS	| 
				WINSTA_CREATEDESKTOP		| 
				WINSTA_ENUMDESKTOPS			| 
				WINSTA_ENUMERATE			| 
				WINSTA_EXITWINDOWS			| 
				WINSTA_READATTRIBUTES		| 
				WINSTA_READSCREEN			| 
				WINSTA_WRITEATTRIBUTES );
			if( !hWinSta )
			{
				WriteLog( L"ControlHandlerEx::OpenWindowStation", GetLastError( ) );
				return;
			}

			if( !SetProcessWindowStation( hWinSta ) )
			{
				WriteLog( L"ControlHandlerEx::SetProcessWindowStation", GetLastError( ) );
				return;
			}

			HDESK hDesktop;
			hDesktop = OpenDesktop( L"Default", 0, FALSE, 
				DESKTOP_CREATEMENU			| 
				DESKTOP_CREATEWINDOW		| 
				DESKTOP_ENUMERATE			| 
				DESKTOP_HOOKCONTROL			| 
				DESKTOP_JOURNALPLAYBACK		| 
				DESKTOP_JOURNALRECORD		| 
				DESKTOP_READOBJECTS			| 
				DESKTOP_SWITCHDESKTOP		| 
				DESKTOP_WRITEOBJECTS );
			if( !hDesktop )
			{
				WriteLog( L"ControlHandlerEx::OpenDesktop", GetLastError( ) );
				return;
			}

			if( !SetThreadDesktop( hDesktop ) )
			{
				WriteLog( L"ControlHandlerEx::SetThreadDesktop", GetLastError( ) );
				return;
			}

			if( !EnumDesktopWindows( hDesktop, EnumWindowsProc, 0 ) )
			{
				WriteLog( L"ControlHandlerEx::EnumDesktopWindows", GetLastError( ) );
				return;
			}

			CloseDesktop( hDesktop );

			output << g_WinMaxCount << " of " << g_WinCount;
		}

		WriteLog( const_cast<TCHAR *>( output.str( ).c_str( ) ) );

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


#ifdef _MSC_VER
	#undef _CRT_SECURE_NO_WARNINGS
#endif
