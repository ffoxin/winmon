#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
#endif


#pragma comment(lib, "..\\Debug\\log.lib" )


#include <Windows.h>
#include <fstream>
#include <iostream>
#include <tchar.h>

#include "..\log\log.h"


using std::wcout;
using std::wcin;
using std::endl;


BOOL CALLBACK EnumWindowsProc( HWND hWnd, LPARAM lParam );
BOOL CALLBACK EnumDesktopProc( LPTSTR lpszDesktop, LPARAM lParam );
BOOL CALLBACK EnumWindowStationProc( LPTSTR lpszWindowStation, LPARAM lParam );


size_t max = 0;
size_t all = 0;

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

BOOL CALLBACK EnumWindowsProc( HWND hWnd, LPARAM lParam )
{
	WINDOWPLACEMENT		wp;
	TCHAR				title[MAX_PATH];

	all++;

	GetWindowText( hWnd, title, MAX_PATH );
	GetWindowPlacement( hWnd, &wp );

	if( wp.showCmd == SW_MAXIMIZE )
	{
		max++;
		WriteLog( title );
		WriteConsoleLog( title );
		/*wp.showCmd = SW_SHOWNORMAL;
		SetWindowPlacement( hWnd, &wp );
		wp.showCmd = SW_MAXIMIZE;
		SetWindowPlacement( hWnd, &wp );*/
	}

	return TRUE;
}


#ifdef _MSC_VER
	#undef _CRT_SECURE_NO_WARNINGS
#endif
