/*
 * drv_gpio.h
 *
 *  Created on: 2018Äê11ÔÂ2ÈÕ
 *      Author: 508
 */

#ifndef SRC_DRIVER_DRV_GPIO_H_
#define SRC_DRIVER_DRV_GPIO_H_

#include "driver.h"

#ifdef DRIVER_ENABLE_GPIO
#include "xgpio.h"

#define GPIO_CMD_SET_DIR 	0x00


int init_gpio(S_DEV_INFO * dev);
void write_gpio(uint32_t reg_addr, uint32_t val);
uint32_t read_gpio(uint32_t reg_addr);
void set_dir_gpio(uint32_t reg_addr, uint32_t dir);

#endif /* DRIVER_ENABLE_GPIO */

#endif /* SRC_DRIVER_DRV_GPIO_H_ */
