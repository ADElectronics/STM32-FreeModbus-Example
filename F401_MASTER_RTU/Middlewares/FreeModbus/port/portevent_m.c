#include "port.h"
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- Variables ----------------------------------------*/
static eMBMasterEventType eQueuedEvent;
static BOOL     xEventInQueue;
BOOL xNeedPoll;
/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortEventInit( void )
{
	xEventInQueue = FALSE;
	return TRUE;
}

BOOL xMBMasterPortEventPost( eMBMasterEventType eEvent )
{
	xEventInQueue = TRUE;
	eQueuedEvent = (eMBMasterEventType)eEvent;
	return TRUE;
}

BOOL xMBMasterPortEventGet( eMBMasterEventType * eEvent )
{
	BOOL xEventHappened = FALSE;

	if( xEventInQueue )
	{
		*eEvent = (eMBMasterEventType)eQueuedEvent;
		xEventInQueue = FALSE;
		xEventHappened = TRUE;
	}
	return xEventHappened;
}
/**
 * This function is initialize the OS resource for modbus master.
 * Note:The resource is define by OS.If you not use OS this function can be empty.
 *
 */
void vMBMasterOsResInit( void )
{

}

/**
 * This function is take Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use OS this function can be just return TRUE.
 *
 * @param lTimeOut the waiting time.
 *
 * @return resource taked result
 */
BOOL xMBMasterRunResTake( LONG lTimeOut )
{
	//HAL_Delay(lTimeOut);
	return TRUE;
}

/**
 * This function is release Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use OS this function can be empty.
 *
 */
void vMBMasterRunResRelease( void )
{
	xNeedPoll = TRUE;
}

/**
 * This is modbus master respond timeout error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBRespondTimeout(UCHAR ucDestAddress, const UCHAR* pucPDUData, USHORT ucPDULength)
{
	xMBMasterPortEventPost(EV_MASTER_ERROR_RESPOND_TIMEOUT);
}

/**
 * This is modbus master receive data error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBReceiveData(UCHAR ucDestAddress, const UCHAR* pucPDUData, USHORT ucPDULength)
{
	xMBMasterPortEventPost(EV_MASTER_ERROR_RECEIVE_DATA);
}

/**
 * This is modbus master execute function error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBExecuteFunction(UCHAR ucDestAddress, const UCHAR* pucPDUData, USHORT ucPDULength)
{
	xMBMasterPortEventPost(EV_MASTER_ERROR_EXECUTE_FUNCTION);
}

/**
 * This is modbus master request process success callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 */
void vMBMasterCBRequestScuuess( void )
{
	xMBMasterPortEventPost(EV_MASTER_PROCESS_SUCESS);
}

/**
 * This function is wait for modbus master request finish and return result.
 * Waiting result include request process success, request respond timeout,
 * receive data error and execute function error.You can use the above callback function.
 * @note If you are use OS, you can use OS's event mechanism. Otherwise you have to run
 * much user custom delay for waiting.
 *
 * @return request error code
 */
eMBMasterReqErrCode eMBMasterWaitRequestFinish( void )
{
	eMBMasterReqErrCode eErrStatus = MB_MRE_NO_ERR;
	//eMBMasterEventType recvedEvent;

	//xMBMasterPortEventGet(&recvedEvent);

	switch (eQueuedEvent)
	{
		case EV_MASTER_ERROR_RESPOND_TIMEOUT:
			eErrStatus = MB_MRE_TIMEDOUT;
		break;

		case EV_MASTER_ERROR_RECEIVE_DATA:
			eErrStatus = MB_MRE_REV_DATA;
		break;

		case EV_MASTER_ERROR_EXECUTE_FUNCTION:
			eErrStatus = MB_MRE_EXE_FUN;
		break;

		default:
			eErrStatus = MB_MRE_NO_ERR;
		break;
	}
	return eErrStatus;
}

#endif
