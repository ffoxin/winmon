#ifndef restorer_h__
#define restorer_h__


/************************************************************************/
/* Device signature
 *  notification will be processed if device name contains signature below */
/************************************************************************/
//const TCHAR	detect_token[]	= _T("DISPLAY");
const TCHAR	detect_token[]	= _T("{6ac27878-a6fa-4155-ba85-f98f491d4f33}");

/************************************************************************/
/* Subscribe service on receiving device notifications                                                                     */
/************************************************************************/
void RegisterDeviceInterfaceNotification( SERVICE_STATUS_HANDLE hServiceStatusHandle );

/************************************************************************/
/* Release subscription                                                                     */
/************************************************************************/
void UnregisterDeviceInterfaceNotification( );

/************************************************************************/
/* Device notification handler                                                                     */
/************************************************************************/
void DeviceEventProc( DWORD dwEventType, LPVOID lpEventData );

/************************************************************************/
/* Restore corrupted windows                                                                     */
/************************************************************************/
void FixWindows( );


#endif // restorer_h__
