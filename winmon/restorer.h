#ifndef restorer_h__
#define restorer_h__


const TCHAR	const detect_token[]	= L"DISPLAY";


void RegisterDeviceInterfaceNotification( SERVICE_STATUS_HANDLE hServiceStatusHandle );
void UnregisterDeviceInterfaceNotification( );
void DeviceEventProc( DWORD dwEventType, LPVOID lpEventData );
void FixWindows( );


#endif // restorer_h__
