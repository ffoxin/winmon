#include <Windows.h>

#include "..\log\log.h"
#include "scm.h"


extern TCHAR			service_name[];


int InstallService( )
{
	TCHAR module[MAX_PATH];
	GetModuleFileName( 0, module, MAX_PATH );

	SC_HANDLE hSCManager = OpenSCManager( 0, 0, SC_MANAGER_CREATE_SERVICE );
	if( !hSCManager )
	{
		WriteLogE( L"InstallService::OpenSCManager" );

		return -1;
	}

	SC_HANDLE hService = CreateService( hSCManager, service_name, service_name, 
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, 
		SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, module, 
		0, 0, 0, 0, 0 );

	if( !hService )
	{
		WriteLogE( L"InstallService::CreateService" );
		CloseServiceHandle( hSCManager );

		return -1;
	}

	CloseServiceHandle( hService );
	CloseServiceHandle( hSCManager );

	WriteLog( L"Service installed successfully" );
	WriteConsoleLog( L"Service installed successfully" );

	return 0;
}

int RemoveService( )
{
	SC_HANDLE hSCManager = OpenSCManager( 0, 0, SC_MANAGER_ALL_ACCESS );
	if( !hSCManager )
	{
		WriteLogE( L"RemoveService::OpenSCManager" );

		return -1;
	}

	SC_HANDLE hService = OpenService( hSCManager, service_name, SERVICE_STOP | DELETE );

	if( !hService )
	{
		WriteLogE( L"RemoveService::OpenService" );
		CloseServiceHandle( hSCManager );

		return -1;
	}

	DeleteService( hService );

	CloseServiceHandle( hService );
	CloseServiceHandle( hSCManager );

	WriteLog( L"Service removed successfully" );
	WriteConsoleLog( L"Service removed successfully" );

	return 0;
}

int StartService( )
{
	SC_HANDLE hSCManager = OpenSCManager( 0, 0, SC_MANAGER_ENUMERATE_SERVICE );
	if( !hSCManager )
	{
		WriteLogE( L"StartService::OpenSCManager" );

		return -1;
	}

	SC_HANDLE hService = OpenService( hSCManager, service_name, SERVICE_START );

	if( !hService )
	{
		WriteLogE( L"StartService::OpenService" );
		CloseServiceHandle( hSCManager );

		return -1;
	}

	if( !StartService( hService, 0, 0 ) )
	{
		WriteLogE( L"StartService::StartService" );
		CloseServiceHandle( hService );
		CloseServiceHandle( hSCManager );

		return -1;
	}

	CloseServiceHandle( hService );
	CloseServiceHandle( hSCManager );

	WriteLog( L"Service started" );
	WriteConsoleLog( L"Service started" );

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
		WriteLogE( L"StopService::OpenSCManager" );

		return -1;
	}

	hService = OpenService( hSCManager, service_name, SERVICE_STOP | SERVICE_QUERY_STATUS );
	if( !hService )
	{
		WriteLogE( L"StopService::OpenService" );
		CloseServiceHandle( hSCManager );

		return -1;
	}

	if( !ControlService( hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS) &ssp ) )
		WriteLogE( L"StopService::ControlService" );

	if( !QueryServiceStatusEx( hService, SC_STATUS_PROCESS_INFO, (LPBYTE) &ssp, 
		sizeof( SERVICE_STATUS_PROCESS ), &dwBytesNeeded ) )
	{
		WriteLogE( L"StopService::QueryServiceStatusEx1" );
		CloseServiceHandle( hService );
		CloseServiceHandle( hSCManager );

		return -1;
	}

	if( ssp.dwCurrentState == SERVICE_STOPPED )
	{
		WriteLog( L"Service stopped" );
		WriteConsoleLog( L"Service stopped" );
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
			WriteLogE( L"StopService::QueryServiceStatusEx2" );
			CloseServiceHandle( hService );
			CloseServiceHandle( hSCManager );

			return -1;
		}

		if( ssp.dwCurrentState == SERVICE_STOPPED )
		{
			WriteLog( L"Service stopped" );
			WriteConsoleLog( L"Service stopped" );
		}

		if( GetTickCount( ) - dwStartTime > dwTimeout )
		{
			WriteLog( L"Service stop timeout" );
			WriteConsoleLog( L"Service stop timeout" );
			break;
		}
	}

	CloseServiceHandle( hService );
	CloseServiceHandle( hSCManager );

	return 0;
}
