/*
 * driver.h
 *
 *  Created on: 2018Äê10ÔÂ22ÈÕ
 *      Author: 508
 */

#ifndef SRC_DRIVER_H_
#define SRC_DRIVER_H_

#include "../platform.h"

#include "../platform_dev.h"

#include "drv_spi.h"
#include "drv_sec_pulse.h"
#include "drv_dma.h"
#include "drv_rec.h"
#include "drv_mb_intc.h"
#include "drv_iic.h"
#include "drv_can.h"
#include "drv_uart.h"
#include "drv_fsync.h"
#include "drv_gpio.h"
#include "drv_timer.h"
#include "drv_axi_switch.h"
#include "drv_spi_nbyte.h"
#include "drv_user_ip.h"
#include "drv_uart_highspeed.h"


/**************************** Macro Definitions ******************************/
#define IO_STS_SUCCESS          (0)
#define IO_INVALID_DEV_NO       (-1)
#define IO_INVALID_PARA         (-2)
#define IO_WRITE_FAIL           (-3)
#define IO_READ_FAIL            (-4)
#define IO_CTRL_FAIL            (-5)
#define IO_OPEN_FAIL            (-6)


/**************************** Type Definitions ******************************/

/* platform devices type structure define */
typedef struct platfrom_dev{
	S_DEV_INFO * dev_info[MAX_DEV_NUM];
#ifdef DRIVER_ENABLE_CAN
	const struct dev_can_ops	*can_ops;  // CAN device operation function
#endif /* DRIVER_ENABLE_CAN */
#ifdef DRIVER_ENABLE_IIC
	const struct dev_iic_ops	*iic_ops;  // IIC device operation function
#endif /* DRIVER_ENABLE_IIC */
#ifdef DRIVER_ENABLE_SPI
	const struct dev_spi_ops    *spi_ops;  // SPI device operation function
#endif /* DRIVER_ENABLE_SPI */
#ifdef DRIVER_ENABLE_DMA
	const struct dev_dma_ops	*dma_ops;  // DMA device operation function
#endif /* DRIVER_ENABLE_DMA */
}S_PLATFORM_DEV;

S_PLATFORM_DEV gPlatformDev;
S_DEV_INFO     gDeviceInfo[MAX_DEV_NUM];


/* global function define  */
void platform_dev_init(void);

int ioDevOpen(uint8_t devNo);
int ioDevWrite(uint8_t devNo, uint8_t * arg, uint32_t len);
int ioDevRead(uint8_t devNo, uint8_t * arg, uint32_t len);
int ioDevCtrl(uint8_t devNo, uint8_t * arg);

void ioDevTest(void);

#endif /* SRC_DRIVER_H_ */
