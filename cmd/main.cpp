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


size_t max = 0;
size_t all = 0;

int _tmain( int argc, TCHAR *argv[] )
{
	InitLog( true );

	// get current station and desktop

	HWINSTA	hWinStaNow		= GetProcessWindowStation( );
	if( !hWinStaNow )
		WriteLog( L"_tmain::hWinStaNow", GetLastError( ) );

	HDESK	hDesktopNow		= GetThreadDesktop( GetCurrentThreadId( ) );
	if( !hDesktopNow )
		WriteLog( L"_tmain::hDesktopNow", GetLastError( ) );

	// get target station and desktop

	HWINSTA hWinStaUser		= OpenWindowStation( L"WinSta0", FALSE, GENERIC_ALL );
	if( !hWinStaUser )
		WriteLog( L"_tmain::hWinStaUser", GetLastError( ) );

	if( !SetProcessWindowStation( hWinStaUser ) )
		WriteLog( L"_tmain::SetProcessWindowStation", GetLastError( ) );

	HDESK	hDesktopUser	= OpenInputDesktop( 0, FALSE, GENERIC_ALL );
	if( !hDesktopUser )
		WriteLog( L"_tmain::hDesktopUser", GetLastError( ) );

	if( !SetThreadDesktop( hDesktopUser ) )
		WriteLog( L"_tmain::SetThreadDesktop", GetLastError( ) );

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
