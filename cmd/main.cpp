#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
#endif


#include <Windows.h>
#include <fstream>
#include <iostream>
#include <tchar.h>

#include "..\common\log.h"


using std::wcout;
using std::wcin;
using std::endl;


BOOL CALLBACK EnumWindowsProc( HWND hWnd, LPARAM lParam );
BOOL CALLBACK EnumDesktopProc( LPTSTR lpszDesktop, LPARAM lParam );
BOOL CALLBACK EnumWindowStationProc( LPTSTR lpszWindowStation, LPARAM lParam );


size_t		g_Stations	= 0;
size_t		g_Desktops	= 0;
size_t		g_Windows	= 0;

HWINSTA		hWinSta		= 0;
HDESK		hDesktop	= 0;
HWINSTA		hDefSta		= 0;
HDESK		hDefDesktop	= 0;


int _tmain( int argc, TCHAR *argv[] )
{
	InitLog( true );

	HWINSTA	hDefSta		= GetProcessWindowStation( );
	HDESK	hDefDesktop	= GetThreadDesktop( GetCurrentThreadId( ) );

	if( !EnumWindowStations( EnumWindowStationProc, 0 ) )
	{
		WriteLog( L"_tmain::EnumWindowStations", GetLastError( ) );
	}

	//SetProcessWindowStation( hDefSta );
	//SetThreadDesktop( hDefDesktop );

	wcout << L"Stations:\t" << g_Stations << endl;
	wcout << L"Desktops:\t" << g_Desktops << endl;
	wcout << L"Windows:\t" << g_Windows << endl;

	return 0;
}

BOOL CALLBACK EnumWindowStationProc( LPTSTR lpszWindowStation, LPARAM lParam )
{
	g_Stations++;

	TCHAR buf[MAX_PATH];
	wcscpy( buf, L"Station name:\t" );
	wcscat( buf, lpszWindowStation );
	WriteLog( buf );

	hWinSta = OpenWindowStation( lpszWindowStation, FALSE, MAXIMUM_ALLOWED
		/*WINSTA_ACCESSCLIPBOARD   | 
		WINSTA_ACCESSGLOBALATOMS | 
		WINSTA_CREATEDESKTOP     | 
		WINSTA_ENUMDESKTOPS      | 
		WINSTA_ENUMERATE         | 
		WINSTA_EXITWINDOWS       | 
		WINSTA_READATTRIBUTES    | 
		WINSTA_READSCREEN        | 
		WINSTA_WRITEATTRIBUTES*/ );
	if( !hWinSta )
	{
		WriteLog( L"EnumWindowStationProc::OpenWindowStation", GetLastError( ) );

		return FALSE;
	}

	if( !SetProcessWindowStation( hWinSta ) )
	{
		WriteLog( L"EnumWindowStationProc::SetProcessWindowStation", GetLastError( ) );
		CloseWindowStation( hWinSta );

		return FALSE;
	}

	if( !EnumDesktops( hWinSta, EnumDesktopProc, 0 ) )
	{
		WriteLog( L"EnumWindowStationProc::EnumDesktops", GetLastError( ) );
		CloseWindowStation( hWinSta );

		return FALSE;
	}

	CloseWindowStation( hWinSta );

	return TRUE;
}

BOOL CALLBACK EnumDesktopProc( LPTSTR lpszDesktop, LPARAM lParam )
{
	g_Desktops++;

	TCHAR buf[MAX_PATH];
	wcscpy( buf, L"Desktop name:\t" );
	wcscat( buf, lpszDesktop );
	WriteLog( buf );

	hDesktop = OpenDesktop( lpszDesktop, DF_ALLOWOTHERACCOUNTHOOK, FALSE, MAXIMUM_ALLOWED
		/*DESKTOP_CREATEMENU		| 
		DESKTOP_CREATEWINDOW	| 
		DESKTOP_ENUMERATE		| 
		DESKTOP_HOOKCONTROL		| 
		DESKTOP_JOURNALPLAYBACK	| 
		DESKTOP_JOURNALRECORD	| 
		DESKTOP_READOBJECTS		| 
		DESKTOP_SWITCHDESKTOP	| 
		DESKTOP_WRITEOBJECTS*/ );
	if( !hDesktop )
	{
		WriteLog( L"EnumDesktopProc::OpenDesktop", GetLastError( ) );

		return FALSE;
	}

	if( !SetThreadDesktop( hDesktop ) )
	{
		WriteLog( L"EnumDesktopProc::SetThreadDesktop", GetLastError( ) );
		CloseDesktop( hDesktop );

		return FALSE;
	}

	if( !EnumDesktopWindows( hDesktop, EnumWindowsProc, 0 ) )
	{
		WriteLog( L"EnumDesktopProc::EnumDesktopWindows", GetLastError( ) );
		CloseDesktop( hDesktop );

		return FALSE;
	}

	CloseDesktop( hDesktop );

	return TRUE;
}

BOOL CALLBACK EnumWindowsProc( HWND hWnd, LPARAM lParam )
{
	WINDOWPLACEMENT		wp;
	TCHAR				title[MAX_PATH];

	g_Windows++;

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


#ifdef _MSC_VER
	#undef _CRT_SECURE_NO_WARNINGS
#endif
