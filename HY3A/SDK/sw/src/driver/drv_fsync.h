/*
 * drv_fsync.h
 *
 *  Created on: 2018Äê11ÔÂ2ÈÕ
 *      Author: 508
 */

#ifndef SRC_DRIVER_DRV_FSYNC_H_
#define SRC_DRIVER_DRV_FSYNC_H_

#include "driver.h"

#ifdef DRIVER_ENABLE_FSYNC


//registers offset declare
#define REG_FSYN_ON_OFF_OFFSET		 0x0	// bit[15:0]
#define REG_FSYN_PERIOD_OFFSET		 0x4	// bit[31:0]  10ns clk set to 8p/s  125ms  12500000
#define REG_FSYN_STATUS_OFFSET		 0x8	// bit[15:0]
#define REG_FSYN_GPS_SEC_OFFSET	     0xc	// bit[31:0]
#define REG_FSYN_GPS_MILSEC_OFFSET	 0x10// bit[27:0] millisecond bit[31:28] control on or off
#define REG_FSYN_HIGH_WIDTH_OFFSET   0x14// bit[31:0] high level width (clk count)

#define FSYN_SUCCESS                 0
#define FSYN_FAIL                    1

#define FSYN_ON	    			     0x0000AAAA
#define FSYN_ONCE    			     0x0000BBBB
#define FSYN_OFF			         0x00005555

#define FSYN_CMD_SET_PERIOD          0x01
#define FSYN_CMD_SET_WIDTH           0x02
#define FSYN_CMD_START               0x04
#define FSYN_CMD_STOP                0x08

int init_fsyn(S_DEV_INFO * dev);
void set_fsync_period(uint32_t regaddr, uint32_t period);
void set_fsync_width(uint32_t regaddr, uint32_t width);
void start_fsync(uint32_t regaddr);
void stop_fsync(uint32_t regaddr);

#endif /* DRIVER_ENABLE_FSYNC */

#endif /* SRC_DRIVER_DRV_FSYNC_H_ */
