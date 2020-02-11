/*
 * drv_timer.c
 *
 *  Created on: 2018Äê11ÔÂ5ÈÕ
 *      Author: 508
 */


#include "drv_timer.h"

#ifdef DRIVER_ENABLE_TIMER

static void timer_intr_handler(void *CallBackRef, u8 TmrCtrNumber)
{

}

int init_timer(S_DEV_INFO * dev)
{
	int ret = TIMER_SUCCESS;
	uint16_t DeviceId = 0xff;

	switch(dev->dev_conf->dev_id)
	{
	case DEV_NUM_TIMER:
		DeviceId = XPAR_TMRCTR_0_DEVICE_ID;
		break;
	default:
		break;
	}

	/*
	 * Initialize the timer counter so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	ret = XTmrCtr_Initialize(&TimerCounterInst, DeviceId);
	if (ret != XST_SUCCESS) {
		return TIMER_FAIL;
	}

	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device.
	 */
	if(dev->dev_conf->intr_num != DEV_NUM_INVALID)
	{
		ret = XIntc_Connect(&InterruptController, dev->dev_conf->intr_num,
					(XInterruptHandler)XTmrCtr_InterruptHandler,
					(void *)&TimerCounterInst);
		if (ret != XST_SUCCESS)
		{
			return TIMER_FAIL;
		}

		/*
		* Enable the interrupt for the UartLite device.
		*/
		XIntc_Enable(&InterruptController, dev->dev_conf->intr_num);

		/*
		 * Setup the handler for the timer counter that will be called from the
		 * interrupt context when the timer expires, specify a pointer to the
		 * timer counter driver instance as the callback reference so the handler
		 * is able to access the instance data
		 */
		XTmrCtr_SetHandler((XTmrCtr *)&TimerCounterInst, timer_intr_handler,
						   (void*)&TimerCounterInst);
	}


	return ret;
}

int read_timer(S_DEV_INFO * dev)
{
	uint32_t m32value = 0;

	if(dev->dev_conf->dev_id != DEV_NUM_INVALID)
	{
		m32value = READ_REG(dev->dev_conf->base_addr + 8);

		dev->dev_com_arg->src[0] = (m32value >> 24) & 0xff;
		dev->dev_com_arg->src[1] = (m32value >> 16) & 0xff;
		dev->dev_com_arg->src[2] = (m32value >>  8) & 0xff;
		dev->dev_com_arg->src[3] = (m32value      ) & 0xff;
	}

	return m32value;
}

void ctrl_timer(uint8_t devNo, uint8_t* arg)
{
	uint32_t m32value = 0;

	switch(arg[0])
	{
	case TIMER_CMD_START_TIMER:
		XTmrCtr_Start(&TimerCounterInst, 0);
		break;
	case TIMER_CMD_STOP_TIMER:
		XTmrCtr_Stop(&TimerCounterInst, 0);
		break;
	case TIMER_CMD_SET_TIMER:
		m32value = 				   arg[1];
		m32value = m32value << 8 | arg[2];
		m32value = m32value << 8 | arg[3];
		m32value = m32value << 8 | arg[4];

		XTmrCtr_SetResetValue(&TimerCounterInst, 0, m32value);
		break;
	case TIMER_CMD_DISABLE_INTR:
		XIntc_Disable(&InterruptController, XPAR_CPU_AXI_INTC_0_CPU_AXI_TIMER_0_INTERRUPT_INTR);
		break;
	default:
		break;
	}
}

#endif /* DRIVER_ENABLE_TIMER */
