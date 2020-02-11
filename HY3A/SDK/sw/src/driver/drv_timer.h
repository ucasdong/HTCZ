/*
 * drv_timer.h
 *
 *  Created on: 2018Äê11ÔÂ5ÈÕ
 *      Author: 508
 */

#ifndef SRC_DRIVER_DRV_TIMER_H_
#define SRC_DRIVER_DRV_TIMER_H_

#include "driver.h"

#ifdef DRIVER_ENABLE_TIMER
#include "xtmrctr.h"

#define TIMER_SUCCESS			0
#define TIMER_FAIL				1

#define TIMER_CMD_START_TIMER	0x00
#define TIMER_CMD_STOP_TIMER	0x01
#define TIMER_CMD_SET_TIMER     0x02
#define TIMER_CMD_DISABLE_INTR	0x03


XTmrCtr TimerCounterInst;   /* The instance of the Timer Counter */

int init_timer(S_DEV_INFO * dev);
int read_timer(S_DEV_INFO * dev);
void ctrl_timer(uint8_t devNo, uint8_t* arg);


#endif /* DRIVER_ENABLE_TIMER */

#endif /* SRC_DRIVER_DRV_TIMER_H_ */
