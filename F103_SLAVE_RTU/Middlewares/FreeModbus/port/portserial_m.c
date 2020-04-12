#include "port.h"
#include "mb.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

/* ----------------------- Static variables ---------------------------------*/
UART_HandleTypeDef *uart;
static uint8_t singlechar;

/* ----------------------- User defenitions ---------------------------------*/
#define RS485_MASTER_RTS_LOW	HAL_GPIO_WritePin(RS485_RTS_GPIO_Port, RS485_RTS_Pin, GPIO_PIN_RESET)
#define RS485_MASTER_RTS_HIGH 	HAL_GPIO_WritePin(RS485_RTS_GPIO_Port, RS485_RTS_Pin, GPIO_PIN_SET)

/* ----------------------- Start implementation -----------------------------*/

BOOL xMBMasterPortSerialInit(void *dHUART, ULONG ulBaudRate, void *dHTIM)
{
	uart = (UART_HandleTypeDef *)dHUART;

	return TRUE;
}

void vMBMasterPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	if(xRxEnable)
	{
		RS485_MASTER_RTS_LOW;
		HAL_UART_Receive_IT(uart, &singlechar, 1);
	}	
	else
	{
		HAL_UART_AbortReceive_IT(uart);
	}

	if(xTxEnable)
	{
		RS485_MASTER_RTS_HIGH;
		pxMBMasterFrameCBTransmitterEmpty();
	}
	else
	{
		HAL_UART_AbortTransmit_IT(uart);
	}
}

void vMBMasterPortClose(void)
{
	HAL_UART_AbortReceive_IT(uart);
	HAL_UART_AbortTransmit_IT(uart);
}

BOOL xMBMasterPortSerialPutBytes(volatile UCHAR *ucByte, USHORT usSize)
{
	HAL_UART_Transmit_IT(uart, (uint8_t *)ucByte, usSize);
	return TRUE;
}

BOOL xMBMasterPortSerialPutByte(CHAR ucByte)
{
	HAL_UART_Transmit_IT(uart, (uint8_t*)&ucByte, 1);
	return TRUE;
}

BOOL xMBMasterPortSerialGetByte(CHAR * pucByte)
{
	*pucByte = (uint8_t)(singlechar);
	return TRUE;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == uart->Instance)
	{
		pxMBMasterFrameCBByteReceived();
		HAL_UART_Receive_IT(uart, &singlechar, 1);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == uart->Instance)
	{
		pxMBMasterFrameCBTransmitterEmpty();
	}
}

#endif
