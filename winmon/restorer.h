#ifndef RESTORER_H_1824_01042012
#define RESTORER_H_1824_01042012


//==============================================================================
//	CONSTANTS
//==============================================================================

const TCHAR	detect_token[]	= L"DISPLAY";


//==============================================================================
//	Prosessor
//==============================================================================

void RegisterDeviceInterfaceNotification( SERVICE_STATUS_HANDLE hServiceStatusHandle );
void UnregisterDeviceInterfaceNotification( );
void DeviceEventProc( DWORD dwEventType, LPVOID lpEventData );


#endif // RESTORER_H_1824_01042012
