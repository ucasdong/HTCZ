/*
 * drv_axi_switch.c
 *
 *  Created on: 2018Äê11ÔÂ14ÈÕ
 *      Author: 508
 */


#include "drv_axi_switch.h"

#ifdef DRIVER_ENABLE_AXI_SWITCH


int init_axi_switch(S_DEV_INFO * dev)
{
	int ret = AXI_SWITCH_SUCCESS;

	return ret;
}

int write_axi_switch(uint32_t regaddr, uint8_t* arg, uint32_t offset)
{
	int ret = USER_IP_SUCCESS;
	int writedata = 0;

	writedata = arg[0];
	writedata = writedata << 8 | arg[1];
	writedata = writedata << 8 | arg[2];
	writedata = writedata << 8 | arg[3];

	WRITE_REG(regaddr + offset, writedata);

	return ret;
}

int read_axi_switch(uint32_t regaddr, uint8_t* arg, uint32_t offset)
{
	int ret = USER_IP_SUCCESS;
	int readdata = 0;

	readdata = READ_REG(regaddr + offset);

	arg[0] = (readdata >> 24) & 0xff;
	arg[1] = (readdata >> 16) & 0xff;
	arg[2] = (readdata >>  8) & 0xff;
	arg[3] = (readdata      ) & 0xff;

	return ret;
}


int ctrl_axi_switch(uint32_t regaddr, uint8_t* arg)
{
	int ret = USER_IP_SUCCESS;

	switch(arg[0])
	{
	case AXI_SWITCH_CMD_M0_S0:
		WRITE_REG(regaddr + 0x40, 0x00);
		break;
	case AXI_SWITCH_CMD_M0_S1:
		WRITE_REG(regaddr + 0x40, 0x01);
		break;
	default:
		break;
	}

	WRITE_REG(regaddr, 0x02);
	return ret;
}

#endif /* DRIVER_ENABLE_AXI_SWITCH */
