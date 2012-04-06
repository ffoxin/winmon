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
	HMODULE hModule = LoadLibrary( L"unknown.dll" );
	WriteLogE( L"LoadLibrary" );
	WriteConsoleLogE( L"LoadLibrary" );

	// get current station and desktop

	HWINSTA	hWinStaNow		= GetProcessWindowStation( );
	WriteLogE( L"GetProcessWindowStation" );

	HDESK	hDesktopNow		= GetThreadDesktop( GetCurrentThreadId( ) );
	if( !hDesktopNow )
		WriteLogE( L"GetThreadDesktop" );

	// get target station and desktop

	HWINSTA hWinStaUser		= OpenWindowStation( L"WinSta0", FALSE, GENERIC_ALL );
	if( !hWinStaUser )
		WriteLogE( L"OpenWindowStation" );

	if( !SetProcessWindowStation( hWinStaUser ) )
		WriteLogE( L"SetProcessWindowStation" );

	HDESK	hDesktopUser	= OpenInputDesktop( 0, FALSE, GENERIC_ALL );
	if( !hDesktopUser )
		WriteLogE( L"OpenInputDesktop" );

	if( !SetThreadDesktop( hDesktopUser ) )
		WriteLogE( L"SetThreadDesktop" );

	// process windows on target desktop

	EnumWindows( EnumWindowsProc, 0 );

	// restore station and desktop

	SetProcessWindowStation( hWinStaNow );
	SetThreadDesktop( hDesktopNow );

	// finilize

	CloseDesktop( hDesktopUser );
	CloseWindowStation( hWinStaUser );

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
