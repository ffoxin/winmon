#include <Windows.h>
#include <iostream>

#include "log.h"
#include "scm.h"


extern TCHAR        service_name[];


void CloseServiceHandles( SC_HANDLE hSCManager, SC_HANDLE hService )
{
    CloseServiceHandle( hService );
    CloseServiceHandle( hSCManager );
}

void ServiceInstall( )
{
    TCHAR module[MAX_PATH];
    GetModuleFileName( 0, module, MAX_PATH );

    SC_HANDLE hSCManager = OpenSCManager( 0, 0, SC_MANAGER_CREATE_SERVICE );
    if( !hSCManager )
    {
        Log( _T("InstallService::OpenSCManager"), GetLastError( ) );

        return;
    }

    SC_HANDLE hService = CreateService( hSCManager, service_name, service_name, 
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, 
        SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, module, 
        0, 0, 0, 0, 0 );

    if( !hService )
    {
        Log( _T("InstallService::CreateService"), GetLastError( ) );

        CloseServiceHandle( hSCManager );

        return;
    }

    CloseServiceHandles( hSCManager, hService );

    Log( _T("Service installed successfully") );
    std::wcout << _T("Service installed successfully") << std::endl;
}

void ServiceRemove( )
{
    SC_HANDLE hSCManager = OpenSCManager( 0, 0, SC_MANAGER_ALL_ACCESS );
    if( !hSCManager )
    {
        Log( _T("RemoveService::OpenSCManager"), GetLastError( ) );

        return;
    }

    SC_HANDLE hService = OpenService( hSCManager, service_name, SERVICE_STOP | DELETE );

    if( !hService )
    {
        Log( _T("RemoveService::OpenService"), GetLastError( ) );

        CloseServiceHandles( hSCManager, hService );

        return;
    }

    DeleteService( hService );

    CloseServiceHandles( hSCManager, hService );

    Log( _T("Service removed successfully"), GetLastError( ) );
    std::wcout << _T("Service removed successfully") << std::endl;
}

void ServiceStart( )
{
    // get handle to the SCM database
    SC_HANDLE hSCManager = OpenSCManager( 0, 0, SC_MANAGER_ENUMERATE_SERVICE );
    if( !hSCManager )
    {
        Log( _T("StartService::OpenSCManager"), GetLastError( ) );

        return;
    }

    // get handle to the service
    SC_HANDLE hService = OpenService( hSCManager, service_name, SERVICE_START );
    if( !hService )
    {
        Log( _T("StartService::OpenService"), GetLastError( ) );

        CloseServiceHandle( hSCManager );

        return;
    }

    // check if service is not stopped
    SERVICE_STATUS_PROCESS  service_status;
    DWORD                   dwBytesNeeded;

    if( !QueryServiceStatusEx( hService, SC_STATUS_PROCESS_INFO, 
        (LPBYTE) &service_status, sizeof( SERVICE_STATUS_PROCESS ), &dwBytesNeeded) )
    {
        Log( _T("StartService::QueryServiceStatusEx"), GetLastError( ) );

        CloseServiceHandles( hSCManager, hService );

        return;
    }

    // check if service already running
    if( service_status.dwCurrentState != SERVICE_STOPPED && 
        service_status.dwCurrentState != SERVICE_STOP_PENDING )
    {
        Log( _T("Attempt to start service failed: service is already running") );

        CloseServiceHandles( hSCManager, hService );

        return;
    }

    // wait until service finish stopping
    DWORD       dwWaitTime;
    ULONGLONG   ulStartTime = GetTickCount64( );

    while( service_status.dwCurrentState == SERVICE_STOP_PENDING )
    {

    }

    if( !StartService( hService, 0, 0 ) )
    {
        Log( _T("StartService::StartService"), GetLastError( ) );

        CloseServiceHandles( hSCManager, hService );

        return;
    }

    CloseServiceHandles( hSCManager, hService );

    Log( _T("Service started") );
    std::wcout << _T("Service started") << std::endl;
}

void ServiceStop( )
{
    SC_HANDLE hSCManager = OpenSCManager( 0, 0, SC_MANAGER_ENUMERATE_SERVICE );
    if( !hSCManager )
    {
        Log( _T("StopService::OpenSCManager"), GetLastError( ) );

        return;
    }

    SC_HANDLE hService = OpenService( hSCManager, service_name, 
        SERVICE_STOP | SERVICE_QUERY_STATUS );
    if( !hService )
    {
        Log( _T("StopService::OpenService"), GetLastError( ) );

        CloseServiceHandle( hSCManager );

        return;
    }

    SERVICE_STATUS_PROCESS ssp;

    if( !ControlService( hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS) &ssp ) )
    {
        Log( _T("StopService::ControlService"), GetLastError( ) );

        CloseServiceHandles( hSCManager, hService );

        return;
    }

    DWORD dwBytesNeeded;

    if( !QueryServiceStatusEx( hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &ssp, 
        sizeof( SERVICE_STATUS_PROCESS ), &dwBytesNeeded ) )
    {
        Log( _T("StopService::QueryServiceStatusEx"), GetLastError( ) );

        CloseServiceHandles( hSCManager, hService );

        return;
    }

    if( ssp.dwCurrentState == SERVICE_STOPPED )
    {
        Log( _T("Service stopped") );
        std::wcout << _T("Service stopped") << std::endl;

        CloseServiceHandles( hSCManager, hService );

        return;
    }

    DWORD       dwWaitTime;
    ULONGLONG   ulStartTime = GetTickCount64( );

    while( ssp.dwCurrentState != SERVICE_STOPPED )
    {
        dwWaitTime = ssp.dwWaitHint / 10;
        if( dwWaitTime < MIN_WAIT_TIME )
        {
            dwWaitTime = MIN_WAIT_TIME;
        }
        else if( dwWaitTime > MAX_WAIT_TIME )
        {
            dwWaitTime = MAX_WAIT_TIME;
        }

        Sleep( dwWaitTime );

        if( !QueryServiceStatusEx( hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &ssp, 
            sizeof( SERVICE_STATUS_PROCESS ), &dwBytesNeeded ) )
        {
            Log( _T("StopService::QueryServiceStatusEx"), GetLastError( ) );

            CloseServiceHandles( hSCManager, hService );

            return;
        }

        if( ssp.dwCurrentState == SERVICE_STOPPED )
        {
            Log( _T("Service stopped") );
            std::wcout << _T("Service stopped") << std::endl;
        }

        if( GetTickCount64( ) - ulStartTime > SVC_WAIT_TIMEOUT )
        {
            Log( _T("Service stop timeout") );
            std::wcout << _T("Service stop timeout") << std::endl;

            break;
        }
    }

    CloseServiceHandles( hSCManager, hService );
}
