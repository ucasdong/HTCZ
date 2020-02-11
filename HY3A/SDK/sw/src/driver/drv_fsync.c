/*
 * drv_fsync.c
 *
 *  Created on: 2018Äê11ÔÂ2ÈÕ
 *      Author: 508
 */

#include "drv_fsync.h"

#ifdef DRIVER_ENABLE_FSYNC


int init_fsyn(S_DEV_INFO * dev)
{
	int ret = SECOND_PULSE_SUCCESS;

	return ret;
}

/* set second pulse period */
void set_fsync_period(uint32_t regaddr, uint32_t period)
{
	WRITE_REG(regaddr + REG_FSYN_PERIOD_OFFSET, period);
}

/* set second pulse valid pulse width */
void set_fsync_width(uint32_t regaddr, uint32_t width)
{
	WRITE_REG(regaddr + REG_FSYN_HIGH_WIDTH_OFFSET, width);
}

void start_fsync(uint32_t regaddr)
{
	WRITE_REG(regaddr + REG_SEC_ON_OFF_OFFSET, FSYN_ON);
}

void stop_fsync(uint32_t regaddr)
{
//	WRITE_REG(regaddr + REG_SEC_ON_OFF_OFFSET, FSYN_OFF);
	WRITE_REG(regaddr + REG_SEC_ON_OFF_OFFSET, FSYN_OFF);

	while(1)
	{
	if(READ_REG(regaddr + REG_FSYN_STATUS_OFFSET) == FSYN_OFF)
	{
		if(READ_REG(regaddr + REG_FSYN_STATUS_OFFSET) == FSYN_OFF)
		{
			break;
		}
	}
	}
}

#endif /* DRIVER_ENABLE_FSYNC */
