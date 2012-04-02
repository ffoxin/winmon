#ifndef LOG_H_1522_01042012
#define LOG_H_1522_01042012


//==============================================================================
//	LOG
//==============================================================================

int InitLog( bool clear = false );
int WriteLog( TCHAR *msg );
int WriteLog( TCHAR *msg, DWORD msg_id );

int WriteConsoleLog( TCHAR *msg );
int WriteConsoleLog( TCHAR *msg, DWORD msg_id );


#endif // LOG_H_1522_01042012
