#ifndef LOG_H_1522_01042012
#define LOG_H_1522_01042012


//==============================================================================
//	Logging to file
//==============================================================================

int InitLog( bool clear = false );			// init log path
int WriteLog( TCHAR *msg );					// log message
int WriteLog( TCHAR *msg, DWORD msg_id );	// log message and FormatMessage
											// from error code


//==============================================================================
//	Logging to console
//==============================================================================

int WriteConsoleLog( TCHAR *msg );					// log message
int WriteConsoleLog( TCHAR *msg, DWORD msg_id );	// log message and FormatMessage
													// from error code


#endif // LOG_H_1522_01042012
