#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <Windows.h>

#include "..\common\log.h"
#include "restorer.h"


extern size_t g_WinCount;
extern size_t g_WinMaxCount;


BOOL CALLBACK EnumWindowsProc( HWND hWnd, LPARAM lParam )
{
	WINDOWPLACEMENT		wp;
	TCHAR				title[MAX_PATH];

	GetWindowText( hWnd, title, MAX_PATH );
	GetWindowPlacement( hWnd, &wp );
	//WriteLog( title );
	g_WinCount++;

	if( wp.showCmd == SW_MAXIMIZE )
	{
		g_WinMaxCount++;
		//WriteLog( title );
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
