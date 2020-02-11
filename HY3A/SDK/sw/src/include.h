#ifndef __PLATFORM_CONFIG_H_
#define __PLATFORM_CONFIG_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "xparameters.h"
#include "xil_printf.h"
#include "xil_types.h"

#define MACHINE_TYPE_2711
//#define MACHINE_TYPE_LVDS

#define PROTOCOL_TYPE_CAN
#define PROTOCOL_TYPE_UART_2M
//#define PROTOCOL_TYPE_SPI
//#define PROTOCOL_TYPE_SPI_NBYTE


/* common device */
#define DRIVER_ENABLE_MB_INTC
#define DRIVER_ENABLE_UART
#define DRIVER_ENABLE_SECOND_PULSE
#define DRIVER_ENABLE_FSYNC
#define DRIVER_ENABLE_GPIO
//#define DRIVER_ENABLE_TIMER
//#define DRIVER_ENABLE_DMA
//#define DRIVER_ENABLE_ETH
//#define DRIVER_ENABLE_AXI_SWITCH
#define DRIVER_ENABLE_USER_IP


#ifdef MACHINE_TYPE_2711
#define DRIVER_ENABLE_IIC   // only config 2711
#else
#undef DRIVER_ENABLE_IIC
#endif /* MACHINE_TYPE_2711 */

#ifdef MACHINE_TYPE_LVDS
#else
#endif /* MACHINE_TYPE_LVDS */

#ifdef PROTOCOL_TYPE_CAN
#define DRIVER_ENABLE_CAN
#else
#undef DRIVER_ENABLE_CAN
#endif /* PROTOCOL_TYPE_CAN */


#ifdef PROTOCOL_TYPE_SPI
#define DRIVER_ENABLE_SPI
#else
#undef DRIVER_ENABLE_SPI
#endif

#ifdef PROTOCOL_TYPE_SPI_NBYTE
#define DRIVER_ENABLE_SPI_NBYTE
#else
#undef DRIVER_ENABLE_SPI_NBYTE
#endif /* PROTOCOL_TYPE_SPI_NBYTE */

#ifdef PROTOCOL_TYPE_UART_2M
#define DRIVER_ENABLE_UART_2M
#else
#undef DRIVER_ENABLE_UART_2M
#endif /* PROTOCOL_TYPE_UART_2M */

#define SYSTEM_STS_TIMER_INTR           0x01
#define SYSTEM_STS_UART_INTR            0x02
uint32_t gSystemStatus;

#define writel(addr, val) (*(volatile uint32_t*)(addr) = val);
#define readl(addr)       (*(volatile uint32_t*)(addr))
#endif
