/*
 * drv_axi_switch.h
 *
 *  Created on: 2018Äê11ÔÂ14ÈÕ
 *      Author: 508
 */

#ifndef SRC_DRIVER_DRV_AXI_SWITCH_H_
#define SRC_DRIVER_DRV_AXI_SWITCH_H_

#include "driver.h"

#ifdef DRIVER_ENABLE_AXI_SWITCH

#define AXI_SWITCH_SUCCESS		0
#define AXI_SWITCH_FAIL 		1

#define AXI_SWITCH_CMD_M0_S0   0
#define AXI_SWITCH_CMD_M0_S1   1

int init_axi_switch(S_DEV_INFO * dev);
int write_axi_switch(uint32_t regaddr, uint8_t* arg, uint32_t offset);
int read_axi_switch(uint32_t regaddr, uint8_t* arg, uint32_t offset);
int ctrl_axi_switch(uint32_t regaddr, uint8_t* arg);


#endif /* DRIVER_ENABLE_AXI_SWITCH */

#endif /* SRC_DRIVER_DRV_AXI_SWITCH_H_ */
