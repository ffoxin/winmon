#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <Dbt.h>
#include <UserEnv.h>
#include <WtsApi32.h>

#include "log.h"
#include "restorer.h"

#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "Wtsapi32.lib")

extern SERVICE_STATUS_HANDLE	g_ServiceStatusHandle;
static HDEVNOTIFY				g_DeviceInterfaceNotification;


void RegisterDeviceInterfaceNotification( SERVICE_STATUS_HANDLE hServiceStatusHandle )
{
	hServiceStatusHandle;

    DEV_BROADCAST_DEVICEINTERFACE dev_int;
    dev_int.dbcc_size			= sizeof( DEV_BROADCAST_DEVICEINTERFACE );
    dev_int.dbcc_devicetype		= DBT_DEVTYP_DEVICEINTERFACE;

    g_DeviceInterfaceNotification = RegisterDeviceNotification( g_ServiceStatusHandle, &dev_int, 
        DEVICE_NOTIFY_SERVICE_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES );
}

void UnregisterDeviceInterfaceNotification( )
{
    UnregisterDeviceNotification( g_DeviceInterfaceNotification );
}

void ReleaseEnv( HANDLE hToken, HANDLE hTokenDup, LPVOID pEnv )
{
	DestroyEnvironmentBlock( pEnv );

	CloseHandle( hToken );
	CloseHandle( hTokenDup );
}

void DeviceEventProc( DWORD dwEventType, LPVOID lpEventData )
{
    PDEV_BROADCAST_HDR				dev_hdr;
    PDEV_BROADCAST_DEVICEINTERFACE	dev_int;

    dev_hdr = (PDEV_BROADCAST_HDR) lpEventData;
    if( dev_hdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE )
        dev_int = (PDEV_BROADCAST_DEVICEINTERFACE) dev_hdr;
    else
        return;

    Log( _T("Event type: DeviceInterface") );

    if( !( dwEventType & ( DBT_DEVICEARRIVAL | DBT_DEVICEREMOVECOMPLETE ) ) )
        return;

    Log( _T("Detect device change") );
	Log( dev_int->dbcc_name );

    if( _tcsstr( dev_int->dbcc_name, detect_token ) != 0 )
    {
        TCHAR	cmdline[MAX_PATH];

        GetModuleFileName( 0, cmdline, MAX_PATH );
        _tcscat( cmdline, _T(" fix") );

		// new code 
		HANDLE hToken = 0;
		LPVOID pEnv = 0;

		// get the ID of the current active Windows session at the console
		DWORD dwSessionId = WTSGetActiveConsoleSessionId( );
		if( dwSessionId == 0xFFFFFFFF )
		{
			Log( _T("WTSGetActiveConsoleSessionId failed") );
			return;
		}

		// get the token for that session
		if( !WTSQueryUserToken( dwSessionId, &hToken ) )
		{
			Log( _T("WTSQueryUserToken"), GetLastError( ) );
			return;
		}

		// create an environment that will be passing to the process
		STARTUPINFO si;
		ZeroMemory( &si, sizeof( STARTUPINFO ) );
		si.cb = sizeof( STARTUPINFO );
		si.lpDesktop = _T("winsta0\\default");

		DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT | CREATE_NO_WINDOW;

		if( !CreateEnvironmentBlock( &pEnv, hToken, FALSE ) )
		{
			Log( _T("CreateEnvironmentBlock"), GetLastError( ) );
			CloseHandle( hToken );
			return;
		}

		// use CreateProcessAsUser with the user token and the created environment
		PROCESS_INFORMATION pi;
		ZeroMemory( &pi, sizeof( PROCESS_INFORMATION ) );
		
		Log( _T("Call CreateProcessAsUser") );
		if( !CreateProcessAsUser( hToken, 0, cmdline, 0, 0, FALSE, dwCreationFlag, pEnv, 0, &si, &pi ) )
		{
			Log( _T("CreateProcessAsUser"), GetLastError( ) );
		}

		// release captured resources
		DestroyEnvironmentBlock( pEnv );
		CloseHandle( hToken );
    }
}

BOOL CALLBACK EnumWindowsProc( HWND hWnd, LPARAM lParam )
{
	lParam;

    WINDOWPLACEMENT		wp;
    TCHAR				title[MAX_PATH];

    GetWindowText( hWnd, title, MAX_PATH );
    GetWindowPlacement( hWnd, &wp );

    if( wp.showCmd == SW_MAXIMIZE )
    {
        //Log( title );
		wp.showCmd = SW_SHOWNORMAL;
		SetWindowPlacement( hWnd, &wp );
		wp.showCmd = SW_MAXIMIZE;
		SetWindowPlacement( hWnd, &wp );
    }

    return TRUE;
}

void FixWindows( )
{
    EnumWindows( EnumWindowsProc, 0 );
}
