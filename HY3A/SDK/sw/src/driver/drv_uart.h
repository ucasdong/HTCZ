/*
 * drv_uart.h
 *
 *  Created on: 2018Äê10ÔÂ24ÈÕ
 *      Author: 508
 */

#ifndef SRC_DRV_UART_H_
#define SRC_DRV_UART_H_

#include "driver.h"

#ifdef DRIVER_ENABLE_UART
#include "xuartlite.h"
#include "xuartlite_l.h"

#define UART_SUCCESS        0
#define UART_FAIL           1


#define XUART_BUFFER_SIZE  	16

#define MAX_SERIAL_DATA_LEN	13

#define UART_DEV_ID_0           XPAR_UARTLITE_0_DEVICE_ID
#define UART_DEV_ID_1           XPAR_UARTLITE_1_DEVICE_ID


/**************************** Protocol Definitions ******************************/
typedef struct serial_cmd_pro{
	uint8_t cmd;
	uint8_t canSel;
	uint8_t validLen;
	uint8_t data[MAX_SERIAL_DATA_LEN];
}S_SERIAL_CMD_PRO;

XUartLite UartLite[2];		    /* Instance of the UartLite Device */
uint8_t g8UartRecvCntTmpA;
uint8_t g8UartRecvCntTmpB;
extern unsigned int TotalReceivedCount;

int dev_uart_init(S_DEV_INFO * dev);
int dev_uart_transfer(S_DEV_INFO * dev);
#endif /* DRIVER_ENABLE_UART */
#endif /* SRC_DRV_UART_H_ */
