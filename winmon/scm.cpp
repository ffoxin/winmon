#include <Windows.h>
#include <iostream>

#include "log.h"
#include "scm.h"


extern TCHAR			service_name[];


int InstallService( )
{
    TCHAR module[MAX_PATH];
    GetModuleFileName( 0, module, MAX_PATH );

    SC_HANDLE hSCManager = OpenSCManager( 0, 0, SC_MANAGER_CREATE_SERVICE );
    if( !hSCManager )
    {
        Log( L"InstallService::OpenSCManager", GetLastError( ) );

        return -1;
    }

    SC_HANDLE hService = CreateService( hSCManager, service_name, service_name, 
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, 
        SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, module, 
        0, 0, 0, 0, 0 );

    if( !hService )
    {
        Log( L"InstallService::CreateService", GetLastError( ) );
        CloseServiceHandle( hSCManager );

        return -1;
    }

    CloseServiceHandle( hService );
    CloseServiceHandle( hSCManager );

    Log( _T("Service installed successfully") );
    std::wcout << _T("Service installed successfully") << std::endl;

    return 0;
}

int RemoveService( )
{
    SC_HANDLE hSCManager = OpenSCManager( 0, 0, SC_MANAGER_ALL_ACCESS );
    if( !hSCManager )
    {
        Log( _T("RemoveService::OpenSCManager"), GetLastError( ) );
        return -1;
    }

    SC_HANDLE hService = OpenService( hSCManager, service_name, SERVICE_STOP | DELETE );

    if( !hService )
    {
        Log( _T("RemoveService::OpenService"), GetLastError( ) );
        CloseServiceHandle( hSCManager );

        return -1;
    }

    DeleteService( hService );

    CloseServiceHandle( hService );
    CloseServiceHandle( hSCManager );

    Log( _T("Service removed successfully"), GetLastError( ) );
    std::wcout << _T("Service removed successfully") << std::endl;

    return 0;
}

int StartService( )
{
    SC_HANDLE hSCManager = OpenSCManager( 0, 0, SC_MANAGER_ENUMERATE_SERVICE );
    if( !hSCManager )
    {
        Log( _T("StartService::OpenSCManager"), GetLastError( ) );

        return -1;
    }

    SC_HANDLE hService = OpenService( hSCManager, service_name, SERVICE_START );

    if( !hService )
    {
        Log( _T("StartService::OpenService"), GetLastError( ) );
        CloseServiceHandle( hSCManager );

        return -1;
    }

    if( !StartService( hService, 0, 0 ) )
    {
        Log( _T("StartService::StartService"), GetLastError( ) );
        CloseServiceHandle( hService );
        CloseServiceHandle( hSCManager );

        return -1;
    }

    CloseServiceHandle( hService );
    CloseServiceHandle( hSCManager );

    Log( _T("Service started") );
    std::wcout << _T("Service started") << std::endl;

    return 0;
}

int StopService( )
{
    SERVICE_STATUS_PROCESS	ssp;
    SC_HANDLE				hSCManager;
    SC_HANDLE				hService;
    DWORD					dwWaitTime;
    DWORD					dwBytesNeeded;
    DWORD					dwTimeout		= 30000;
    DWORD					dwStartTime		= GetTickCount( );

    hSCManager = OpenSCManager( 0, 0, SC_MANAGER_ENUMERATE_SERVICE );
    if( !hSCManager )
    {
        Log( _T("StopService::OpenSCManager"), GetLastError( ) );
        return -1;
    }

    hService = OpenService( hSCManager, service_name, SERVICE_STOP | SERVICE_QUERY_STATUS );
    if( !hService )
    {
		Log( _T("StopService::OpenService"), GetLastError( ) );

		CloseServiceHandle( hSCManager );

		return -1;
    }

    if( !ControlService( hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS) &ssp ) )
	{
        Log( _T("StopService::ControlService"), GetLastError( ) );
	}

    if( !QueryServiceStatusEx( hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &ssp, 
        sizeof( SERVICE_STATUS_PROCESS ), &dwBytesNeeded ) )
    {
        Log( _T("StopService::QueryServiceStatusEx1"), GetLastError( ) );

        CloseServiceHandle( hService );
        CloseServiceHandle( hSCManager );

        return -1;
    }

    if( ssp.dwCurrentState == SERVICE_STOPPED )
    {
        Log( _T("Service stopped") );
        std::wcout << _T("Service stopped") << std::endl;
    }

    while( ssp.dwCurrentState != SERVICE_STOPPED )
    {
        dwWaitTime = ssp.dwWaitHint / 10;
        if( dwWaitTime < 1000 )
            dwWaitTime = 1000;
        else if( dwWaitTime > 10000 )
            dwWaitTime = 10000;

        Sleep( dwWaitTime );

        if( !QueryServiceStatusEx( hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &ssp, 
            sizeof( SERVICE_STATUS_PROCESS ), &dwBytesNeeded ) )
        {
            Log( _T("StopService::QueryServiceStatusEx2"), GetLastError( ) );

            CloseServiceHandle( hService );
            CloseServiceHandle( hSCManager );

            return -1;
        }

        if( ssp.dwCurrentState == SERVICE_STOPPED )
        {
            Log( _T("Service stopped") );
            std::wcout << _T("Service stopped") << std::endl;
        }

        if( GetTickCount( ) - dwStartTime > dwTimeout )
        {
            Log( _T("Service stop timeout") );
            std::wcout << _T("Service stop timeout") << std::endl;
            break;
        }
    }

    CloseServiceHandle( hService );
    CloseServiceHandle( hSCManager );

    return 0;
}
