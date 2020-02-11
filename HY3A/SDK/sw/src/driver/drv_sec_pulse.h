/*
 * drv_sec_pulse.h
 *
 *  Created on: 2018Äê10ÔÂ23ÈÕ
 *      Author: 508
 */

#ifndef SRC_DRV_SEC_PULSE_H_
#define SRC_DRV_SEC_PULSE_H_

#include "driver.h"

#ifdef DRIVER_ENABLE_SECOND_PULSE

//registers offset declare
#define REG_SEC_ON_OFF_OFFSET		0x0	// bit[15:0]
#define REG_SEC_PERIOD_OFFSET		0x4	// bit[31:0]  10ns clk set to 8p/s  125ms  12500000
#define REG_SEC_STATUS_OFFSET		0x8	// bit[15:0]
#define REG_SEC_GPS_SEC_OFFSET	    0xc	// bit[31:0]
#define REG_SEC_GPS_MILSEC_OFFSET	0x10// bit[27:0] millisecond bit[31:28] control on or off
#define REG_SEC_HIGH_WIDTH_OFFSET   0x14// bit[31:0] high level width (clk count)

#define SECOND_PULSE_SUCCESS        0
#define SECOND_PULSE_FAIL           1

#define SECOND_PULSE_ON				0x0000AAAA
#define SECOND_PULSE_ONCE			0x0000BBBB
#define SECOND_PULSE_OFF			0x00005555

#define SEC_PULSE_CMD_SET_PERIOD    0x01
#define SEC_PULSE_CMD_SET_WIDTH     0x02
#define SEC_PULSE_CMD_START         0x04
#define SEC_PULSE_CMD_STOP          0x08

u32 g32SecondCnt;
u32 g32SecondCntTmp;


int init_sec_pulse(S_DEV_INFO * dev);
int close_sec_pulse(S_DEV_INFO * dev);
void set_sec_pulse_period(uint32_t regaddr, uint32_t period);
void set_sec_pulse_width(uint32_t regaddr, uint32_t width);
void start_sec_pulse(uint32_t regaddr);
void stop_sec_pulse(uint32_t regaddr);

#endif /* DRIVER_ENABLE_SECOND_PULSE */

#endif /* SRC_DRV_SEC_PULSE_H_ */
