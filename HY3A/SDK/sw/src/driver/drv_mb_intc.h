/*
 * intc.h
 *
 *  Created on: 2018Äê9ÔÂ13ÈÕ
 *      Author: 508
 */

#ifndef SRC_DRV_MB_INTC_H_
#define SRC_DRV_MB_INTC_H_

#include "driver.h"
#include "xintc.h"
#include "xil_exception.h"

#define INTC_SUCCESS         0
#define INTC_FAIL            1

#define INTC_DEVICE_ID       XPAR_INTC_0_DEVICE_ID

#define CTRL_INTC_START		 0x01
#define CTRL_INTC_STOP		 0x02
#define CTRL_INTC_DISABLE	 0x04

#define RESET_DATA_PATH()
#if 0
\
{\
	*(volatile uint32_t*)(XPAR_M_REGLITE_CONFIG_BASEADDR + 4) = 1;\
	*(volatile uint32_t*)(XPAR_M_REGLITE_CONFIG_BASEADDR + 4) = 0;\
}
#endif

#define DATA_PATH_RESET()
#if 0
\
{\
	*(volatile uint32_t*)(XPAR_M_REGLITE_CONFIG_BASEADDR + 4) = 1;\
}
#endif

#define DATA_PATH_RESET_RELEASE()
#if 0
\
{\
	*(volatile uint32_t*)(XPAR_M_REGLITE_CONFIG_BASEADDR + 4) = 0;\
}
#endif

/*var define */
XIntc InterruptController;     /* The instance of the Interrupt Controller */



/*function define*/
uint8_t init_intc(void);
int ctrl_intc(uint8_t* arg);
#endif /* SRC_DRV_MB_INTC_H_ */
