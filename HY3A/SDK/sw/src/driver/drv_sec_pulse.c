/*
 * drv_sec_pulse.c
 *
 *  Created on: 2018Äê10ÔÂ23ÈÕ
 *      Author: 508
 */


#include "drv_sec_pulse.h"
#ifdef DRIVER_ENABLE_SECOND_PULSE
static void second_pulse_intr0_handler(void *InstancePtr)
{
	gSystemStatus |= SYSTEM_STS_TIMER_INTR;

	return;
}
static void second_pulse_intr_handler(void *InstancePtr)
{

	return ;
}

int init_sec_pulse(S_DEV_INFO * dev)
{
	int ret = SECOND_PULSE_SUCCESS;

	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device.
	 */
	if(dev->dev_conf->intr_num != DEV_NUM_INVALID)
	{
		switch(dev->dev_conf->intr_num)
		{
		case 		XPAR_CPU_AXI_INTC_0_CPU_SECOND_PULSE_INTR_RES_INTR:
			ret = XIntc_Connect(&InterruptController, dev->dev_conf->intr_num,
				(XInterruptHandler)second_pulse_intr0_handler,
			(void *)dev);
			break;
		case XPAR_CPU_AXI_INTC_0_LOW_SPEED_PERIPHERAL_FSYN_TIMER_CPSL_FSYN_O_INTR:
			ret = XIntc_Connect(&InterruptController, dev->dev_conf->intr_num,
							(XInterruptHandler)second_pulse_intr_handler,
						(void *)dev);
			break;
		}


		if (ret != XST_SUCCESS)
		{
			return SECOND_PULSE_FAIL;
		}

		/*
		* Enable the interrupt for the UartLite device.
		*/
		XIntc_Enable(&InterruptController, dev->dev_conf->intr_num);
	}

	return ret;
}

//zdren add for fsync timer stop function @ 0128
int close_sec_pulse(S_DEV_INFO * dev)
{
	int ret = SECOND_PULSE_SUCCESS;

	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device.
	 */
	if(dev->dev_conf->intr_num != DEV_NUM_INVALID)
	{
		XIntc_Disconnect(&InterruptController, dev->dev_conf->intr_num);
		/*
		* Enable the interrupt for the UartLite device.
		*/
		XIntc_Disable(&InterruptController, dev->dev_conf->intr_num);
	}

	return ret;
}

/* set second pulse period */
void set_sec_pulse_period(uint32_t regaddr, uint32_t period)
{
	WRITE_REG(regaddr + REG_SEC_PERIOD_OFFSET, period);
}

/* set second pulse valid pulse width */
void set_sec_pulse_width(uint32_t regaddr, uint32_t width)
{
	WRITE_REG(regaddr + REG_SEC_HIGH_WIDTH_OFFSET, width);
}

void start_sec_pulse(uint32_t regaddr)
{
	WRITE_REG(regaddr + REG_SEC_ON_OFF_OFFSET, SECOND_PULSE_ON);
}

void stop_sec_pulse(uint32_t regaddr)
{
	WRITE_REG(regaddr + REG_SEC_ON_OFF_OFFSET, SECOND_PULSE_ONCE);

	while(1)
	{
	if(READ_REG(regaddr + REG_SEC_STATUS_OFFSET) == SECOND_PULSE_OFF)
	{
		if(READ_REG(regaddr + REG_SEC_STATUS_OFFSET) == SECOND_PULSE_OFF)
		{
			break;
		}
	}
	}
}
#endif /* DRIVER_ENABLE_SECOND_PULSE */
