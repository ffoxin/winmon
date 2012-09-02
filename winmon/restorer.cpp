/************************************************************************/
/*                                                                      */
/************************************************************************/
#include <Windows.h>
#include <Dbt.h>
#include <UserEnv.h>
#include <WtsApi32.h>

#include "log.h"
#include "restorer.h"

#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "Wtsapi32.lib")

extern SERVICE_STATUS_HANDLE    g_ServiceStatusHandle;
static HDEVNOTIFY               g_DeviceInterfaceNotification;


void RegisterDeviceInterfaceNotification( SERVICE_STATUS_HANDLE hServiceStatusHandle )
{
    DEV_BROADCAST_DEVICEINTERFACE dev_int;
    dev_int.dbcc_size       = sizeof( DEV_BROADCAST_DEVICEINTERFACE );
    dev_int.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

    g_DeviceInterfaceNotification = RegisterDeviceNotification( hServiceStatusHandle, &dev_int, 
        DEVICE_NOTIFY_SERVICE_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES );
}

void UnregisterDeviceInterfaceNotification( )
{
    UnregisterDeviceNotification( g_DeviceInterfaceNotification );
}

void DeviceEventProc( DWORD dwEventType, LPVOID lpEventData )
{
    PDEV_BROADCAST_HDR              dev_hdr;
    PDEV_BROADCAST_DEVICEINTERFACE  dev_int;

    dev_hdr = (PDEV_BROADCAST_HDR) lpEventData;
    if( dev_hdr->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE )
    {
        return;
    }

    Log( _T("Event type: DeviceInterface") );

    dev_int = (PDEV_BROADCAST_DEVICEINTERFACE) dev_hdr;

    if( !( dwEventType & ( DBT_DEVICEARRIVAL | DBT_DEVICEREMOVECOMPLETE ) ) )
    {
        return;
    }

    Log( _T("Detect device change") );
    Log( dev_int->dbcc_name );

    if( _tcsstr( dev_int->dbcc_name, detect_token ) == NULL )
    {
        return;
    }
    
    TCHAR cmdline[MAX_PATH];

    GetModuleFileName( NULL, cmdline, MAX_PATH );
    _tcscat_s( cmdline, _T(" fix") );

    // get the ID of the current active Windows session at the console
    DWORD dwSessionId = WTSGetActiveConsoleSessionId( );
    if( dwSessionId == 0xFFFFFFFF )
    {
        Log( _T("WTSGetActiveConsoleSessionId failed") );

        return;
    }
    
    // get the token for that session
    HANDLE hToken = 0;
    
    if( !WTSQueryUserToken( dwSessionId, &hToken ) )
    {
        Log( _T("WTSQueryUserToken"), GetLastError( ) );

        return;
    }
    
    // create an environment that will be passing to the process
    LPVOID  pEnv = 0;
    DWORD   dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT | CREATE_NO_WINDOW;

    STARTUPINFO si;
    ZeroMemory( &si, sizeof( STARTUPINFO ) );
    si.cb = sizeof( STARTUPINFO );
    si.lpDesktop = _T("winsta0\\default");
    
    if( !CreateEnvironmentBlock( &pEnv, hToken, FALSE ) )
    {
        Log( _T("CreateEnvironmentBlock"), GetLastError( ) );

        CloseHandle( hToken );

        return;
    }
    
    // use CreateProcessAsUser with the user token and the created environment
    PROCESS_INFORMATION pi;
    ZeroMemory( &pi, sizeof( PROCESS_INFORMATION ) );

    if( !CreateProcessAsUser( hToken, NULL, cmdline, NULL, NULL, FALSE, 
        dwCreationFlag, pEnv, NULL, &si, &pi ) )
    {
        Log( _T("CreateProcessAsUser"), GetLastError( ) );
    }
    
    // release captured resources
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
    DestroyEnvironmentBlock( pEnv );
    CloseHandle( hToken );
}

BOOL CALLBACK EnumWindowsProc( HWND hWnd, LPARAM lParam )
{
    lParam;

    WINDOWPLACEMENT wp;
    ZeroMemory( &wp, sizeof( WINDOWPLACEMENT ) );

    GetWindowPlacement( hWnd, &wp );
    if( wp.showCmd == SW_MAXIMIZE )
    {
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
