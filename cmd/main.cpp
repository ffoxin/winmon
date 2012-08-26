#define _CRT_SECURE_NO_WARNINGS


#pragma comment(lib, "..\\Debug\\log.lib" )


#include <Windows.h>
#include <fstream>
#include <iostream>
#include <tchar.h>

#include "..\log\log.h"


int _tmain( int argc, TCHAR *argv[] )
{
	if( argc == 3 )
	{
		TCHAR					module[MAX_PATH];
		TCHAR					params[MAX_PATH];
		STARTUPINFO				si;
		PROCESS_INFORMATION		pi;

		GetModuleFileName( 0, module, MAX_PATH );
		wcscpy( params, module );
		wcscat( params, L" fix" );
		memset( &si, 0, sizeof( si ) );
		si.cb = sizeof( si );
		memset( &pi, 0, sizeof( pi ) );

		WriteLog( L"create new process" );
		CreateProcess( module, params, 0, 0, FALSE, NORMAL_PRIORITY_CLASS, 0, 0, &si, &pi );
	}
	else if( argc == 2 )
	{
		WriteLog( L"started" );
		Sleep( 50000 );
	}
	else
	{
		WriteLog( L"do nothing" );
	}

	return 0;
}
