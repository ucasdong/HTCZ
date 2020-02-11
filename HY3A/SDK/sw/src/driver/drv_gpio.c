/*
 * drv_gpio.c
 *
 *  Created on: 2018Äê11ÔÂ2ÈÕ
 *      Author: 508
 */


#include "drv_gpio.h"

#ifdef DRIVER_ENABLE_GPIO

int init_gpio(S_DEV_INFO * dev)
{
	int ret = SECOND_PULSE_SUCCESS;

	return ret;
}

void write_gpio(uint32_t reg_addr, uint32_t val)
{
	WRITE_REG(reg_addr + XGPIO_DATA_OFFSET, val);
}

uint32_t read_gpio(uint32_t reg_addr)
{
	return READ_REG(reg_addr + XGPIO_DATA_OFFSET);
}

void set_dir_gpio(uint32_t reg_addr, uint32_t dir)
{
	/* Set the direction for all signals as user specified */
	WRITE_REG(reg_addr + XGPIO_TRI_OFFSET, dir);
}


#endif /* DRIVER_ENABLE_GPIO */
