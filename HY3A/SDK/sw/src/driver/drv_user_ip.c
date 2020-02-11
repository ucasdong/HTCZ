/*
 * drv_user_ip.c
 *
 *  Created on: 2018Äê11ÔÂ15ÈÕ
 *      Author: 508
 */


#include "drv_user_ip.h"

#ifdef DRIVER_ENABLE_USER_IP

int init_user_ip(S_DEV_INFO * dev)
{
	int ret = USER_IP_SUCCESS;

	return ret;
}

int write_user_ip(uint32_t regaddr, uint8_t* arg, uint32_t offset)
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

int read_user_ip(uint32_t regaddr, uint8_t* arg, uint32_t offset)
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


#endif /* DRIVER_ENABLE_USER_IP */
