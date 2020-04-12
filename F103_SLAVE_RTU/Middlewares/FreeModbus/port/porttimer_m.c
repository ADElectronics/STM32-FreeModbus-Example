#include "port.h"
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

/* ----------------------- User defenitions ---------------------------------*/
TIM_HandleTypeDef *tim;
static uint16_t timeout = 0;
volatile uint16_t counter = 0;

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortTimersInit( USHORT usTimeOut50us, void *dHTIM )
{
	tim = (TIM_HandleTypeDef *)dHTIM;
	timeout = usTimeOut50us;
    return TRUE;
}

void vMBMasterPortTimersT35Enable()
{
    vMBMasterSetCurTimerMode(MB_TMODE_T35);
	counter=0;
	HAL_TIM_Base_Start_IT(tim);
}

void vMBMasterPortTimersConvertDelayEnable()
{
	vMBMasterSetCurTimerMode(MB_TMODE_CONVERT_DELAY);

}

void vMBMasterPortTimersRespondTimeoutEnable()
{
	vMBMasterSetCurTimerMode(MB_TMODE_RESPOND_TIMEOUT);

}

void vMBMasterPortTimersDisable()
{
	HAL_TIM_Base_Stop_IT(tim);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == tim->Instance)
	{
		if((++counter) >= timeout)
			pxMBMasterPortCBTimerExpired();
	}
}

#endif
