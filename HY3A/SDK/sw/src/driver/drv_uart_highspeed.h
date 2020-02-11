/*
 * drv_uart_highspped.h
 *
 *  Created on: 2019Äê2ÔÂ26ÈÕ
 *      Author: 508
 */

#ifndef SRC_DRIVER_DRV_UART_HIGHSPEED_H_
#define SRC_DRIVER_DRV_UART_HIGHSPEED_H_

#include "driver.h"

#ifdef DRIVER_ENABLE_UART_2M

#define UART_HIGH_SPEED_SUCCESS    0
#define UART_HIGH_SPEED_FAIL      -1

#define UART_REG_DATA              0
#define UART_REG_LENGTH            4
#define UART_REG_INTERVAL          8
#define UART_REG_CNTL              0xc

#define UART_REG_READ_DATA         4

#define UART_FIFO_EMPTY            0x80000000
#define UART_START_TREANSFER       0x1

int dev_uart_highspeed_init(S_DEV_INFO * dev);
int dev_uart_high_speed_send(S_DEV_INFO * dev);
#endif /* DRIVER_ENABLE_UART_2M */

#endif /* SRC_DRIVER_DRV_UART_HIGHSPEED_H_ */
