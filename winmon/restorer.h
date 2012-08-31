#ifndef restorer_h__
#define restorer_h__


//const TCHAR	detect_token[]	= _T("DISPLAY");
const TCHAR	detect_token[]	= _T("{6ac27878-a6fa-4155-ba85-f98f491d4f33}");


void RegisterDeviceInterfaceNotification( SERVICE_STATUS_HANDLE hServiceStatusHandle );
void UnregisterDeviceInterfaceNotification( );
void DeviceEventProc( DWORD dwEventType, LPVOID lpEventData );
void FixWindows( );


#endif // restorer_h__
