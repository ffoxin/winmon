#ifndef scm_h__
#define scm_h__


/************************************************************************/
/* Min, max and total time (in milliseconds) waiting for response
 * from service while trying to obtain current service status */
/************************************************************************/
const ULONGLONG MIN_WAIT_TIME = 1000;
const ULONGLONG MAX_WAIT_TIME = 10000;
const ULONGLONG SVC_WAIT_TIMEOUT = 30000;


/************************************************************************/
/* Install service in SCM database                                                                     */
/************************************************************************/
void ServiceInstall( );

/************************************************************************/
/* Delete service from SCM database                                                                     */
/************************************************************************/
void ServiceRemove( );

/************************************************************************/
/* Start installed service                                                                     */
/************************************************************************/
void ServiceStart( );

/************************************************************************/
/* Stop service                                                                     */
/************************************************************************/
void ServiceStop( );


#endif // scm_h__
