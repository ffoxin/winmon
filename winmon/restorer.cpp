#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <Dbt.h>

#include "..\log\log.h"
#include "restorer.h"


extern SERVICE_STATUS_HANDLE	g_ServiceStatusHandle;
static HDEVNOTIFY				g_DeviceInterfaceNotification;


void RegisterDeviceInterfaceNotification( SERVICE_STATUS_HANDLE hServiceStatusHandle )
{
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

void DeviceEventProc( DWORD dwEventType, LPVOID lpEventData )
{
    PDEV_BROADCAST_HDR				dev_hdr;
    PDEV_BROADCAST_DEVICEINTERFACE	dev_int;

    dev_hdr = (PDEV_BROADCAST_HDR) lpEventData;
    if( dev_hdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE )
        dev_int = (PDEV_BROADCAST_DEVICEINTERFACE) dev_hdr;
    else
        return;

    WriteLog( L"Event type: DeviceInterface" );

    if( !( dwEventType & ( DBT_DEVICEARRIVAL | DBT_DEVICEREMOVECOMPLETE ) ) )
        return;

    WriteLog( L"Detect device change" );

    if( wcsstr( dev_int->dbcc_name, detect_token ) != 0 )
    {
        TCHAR					buf[MAX_PATH];
        TCHAR					module[MAX_PATH];
        TCHAR					params[MAX_PATH];
        STARTUPINFO				si;
        PROCESS_INFORMATION		pi;

        wcscpy( buf, L"Token detected: " );
        wcscat( buf, detect_token );
        WriteLog( buf );

        GetModuleFileName( 0, module, MAX_PATH );
        wcscpy( params, module );
        wcscat( params, L" fix" );

        memset( &si, 0, sizeof( si ) );
        si.cb = sizeof( si );
        memset( &pi, 0, sizeof( pi ) );

        CreateProcess( module, L"fix", 0, 0, FALSE, NORMAL_PRIORITY_CLASS, 0, 0, &si, &pi );
    }
}

BOOL CALLBACK EnumWindowsProc( HWND hWnd, LPARAM lParam )
{
    WINDOWPLACEMENT		wp;
    TCHAR				title[MAX_PATH];

    GetWindowText( hWnd, title, MAX_PATH );
    GetWindowPlacement( hWnd, &wp );

    if( wp.showCmd == SW_MAXIMIZE )
    {
        WriteLog( title );
        /*wp.showCmd = SW_SHOWNORMAL;
        SetWindowPlacement( hWnd, &wp );
        wp.showCmd = SW_MAXIMIZE;
        SetWindowPlacement( hWnd, &wp );*/
    }

    return TRUE;
}

void FixWindows( )
{
    EnumWindows( EnumWindowsProc, 0 );
}
