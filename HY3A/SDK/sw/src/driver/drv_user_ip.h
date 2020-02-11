/*
 * drv_user_ip.h
 *
 *  Created on: 2018Äê11ÔÂ15ÈÕ
 *      Author: 508
 */

#ifndef SRC_DRIVER_DRV_USER_IP_H_
#define SRC_DRIVER_DRV_USER_IP_H_

#include "driver.h"

#ifdef DRIVER_ENABLE_USER_IP

#define USER_IP_SUCCESS		0
#define USER_IP_FAIL 		1


int init_user_ip(S_DEV_INFO * dev);
int write_user_ip(uint32_t regaddr, uint8_t* arg, uint32_t offset);
int read_user_ip(uint32_t regaddr, uint8_t* arg, uint32_t offset);


#endif /* DRIVER_ENABLE_USER_IP */

#endif /* SRC_DRIVER_DRV_USER_IP_H_ */
