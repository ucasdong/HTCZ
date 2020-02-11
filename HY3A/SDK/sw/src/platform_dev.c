/*
 * platform_dev.c
 *
 *  Created on: 2018��10��23��
 *      Author: 508
 */

#include "./driver/driver.h"
#include "platform_dev.h"

S_DEV_CONF gDevConfigTable[] =
{
#ifdef DRIVER_ENABLE_CAN
		{
				DEV_NUM_CAN_A,
				XPAR_CPU_AXI_INTC_0_CPU_UTIL_VECTOR_LOGIC_1_RES_INTR,
				XPAR_LOW_SPEED_PERIPHERAL_SAJ1000_V1_0_S00_AXI_0_BASEADDR,
				XPAR_LOW_SPEED_PERIPHERAL_SAJ1000_V1_0_S00_AXI_0_HIGHADDR
		},
		{
				DEV_NUM_CAN_B,
				XPAR_CPU_AXI_INTC_0_CPU_UTIL_VECTOR_LOGIC_0_RES_INTR,
				XPAR_LOW_SPEED_PERIPHERAL_SAJ1000_V1_0_S00_AXI_1_BASEADDR,
				XPAR_LOW_SPEED_PERIPHERAL_SAJ1000_V1_0_S00_AXI_1_HIGHADDR
		},
#endif /* DRIVER_ENABLE_CAN */
#ifdef DRIVER_ENABLE_UART
		{
				DEV_NUM_UART_A,
				XPAR_CPU_AXI_INTC_0_CPU_AXI_UARTLITE_HIGH_SPEED_INTERRUPT_INTR,
				XPAR_UARTLITE_0_BASEADDR,
				XPAR_UARTLITE_0_HIGHADDR
		},
		{
				DEV_NUM_UART_B,
				XPAR_CPU_AXI_INTC_0_CPU_AXI_UARTLITE_CAMLINK_INTERRUPT_INTR,//invalid interrupt ID
				XPAR_UARTLITE_1_BASEADDR,
				XPAR_UARTLITE_1_HIGHADDR
		},
#endif /* DRIVER_ENABLE_UART */
		{
				DEV_NUM_SEC_PULSE,
				XPAR_CPU_AXI_INTC_0_CPU_SECOND_PULSE_INTR_RES_INTR,
				XPAR_LOW_SPEED_PERIPHERAL_FSYN_SECONDPULSE_BASEADDR,
				XPAR_LOW_SPEED_PERIPHERAL_FSYN_SECONDPULSE_HIGHADDR
		},
		{
				DEV_NUM_FSYNC_TIMER,
				XPAR_CPU_AXI_INTC_0_LOW_SPEED_PERIPHERAL_FSYN_TIMER_CPSL_FSYN_O_INTR,
				XPAR_LOW_SPEED_PERIPHERAL_FSYN_TIMER_BASEADDR,
				XPAR_LOW_SPEED_PERIPHERAL_FSYN_TIMER_HIGHADDR
		},
		{
				DEV_NUM_ETH,
				INVALID_INTR_ID,  					//invalid interrupt ID
		},
		{
				DEV_NUM_DMA,
				INVALID_INTR_ID,  					//invalid interrupt ID
		},
#ifdef DRIVER_ENABLE_TIMER
		{
				DEV_NUM_TIMER,
				XPAR_CPU_AXI_INTC_0_CPU_AXI_TIMER_0_INTERRUPT_INTR,
				XPAR_CPU_AXI_TIMER_0_BASEADDR,
				XPAR_CPU_AXI_TIMER_0_HIGHADDR
		},
#endif /* DRIVER_ENABLE_TIMER */

#ifdef DRIVER_ENABLE_FSYNC
		{
				DEV_NUM_FSYNC,
				INVALID_INTR_ID,  					//invalid interrupt ID
				XPAR_HRC10000_SIM_FSYN_V1_1_S00_AXI_0_BASEADDR,
				XPAR_HRC10000_SIM_FSYN_V1_1_S00_AXI_0_HIGHADDR
		},
		{
				DEV_NUM_FSYNC_1,
				INVALID_INTR_ID,  					//invalid interrupt ID
				XPAR_LOW_SPEED_PERIPHERAL_ST_PULSE_FSYN_V1_1_S00_AXI_0_BASEADDR,
				XPAR_LOW_SPEED_PERIPHERAL_ST_PULSE_FSYN_V1_1_S00_AXI_0_HIGHADDR
		},
		{
				DEV_NUM_FSYNC_2,
				INVALID_INTR_ID,  					//invalid interrupt ID
				XPAR_LOW_SPEED_PERIPHERAL_UART_2M_FSYN_V1_1_S00_AXI_0_BASEADDR,
				XPAR_LOW_SPEED_PERIPHERAL_UART_2M_FSYN_V1_1_S00_AXI_0_HIGHADDR
		},
#endif /* DRIVER_ENABLE_FSYNC */

#ifdef DRIVER_ENABLE_IIC
		{
				DEV_NUM_IIC,
				XPAR_CPU_AXI_INTC_0_CPU_AXI_IIC_0_IIC2INTC_IRPT_INTR,
				XPAR_CPU_AXI_IIC_0_BASEADDR,
				XPAR_CPU_AXI_IIC_0_HIGHADDR
		},
#endif /* DRIVER_ENABLE_IIC */
		{
				DEV_NUM_INTR,
				INVALID_INTR_ID,  					//invalid interrupt ID
				XPAR_INTC_0_BASEADDR,
				XPAR_INTC_0_HIGHADDR
		},
#ifdef DRIVER_ENABLE_AXI_SWITCH
		{
				DEV_NUM_AXI_SWITCH,
				INVALID_INTR_ID,  					//invalid interrupt ID
				XPAR_AXIS_SWITCH_0_BASEADDR,     //
				XPAR_AXIS_SWITCH_0_HIGHADDR,
		},
#endif /* DRIVER_ENABLE_AXI_SWITCH */
#ifdef DRIVER_ENABLE_SPI
		{
				DEV_NUM_SPI_SEND,
				INVALID_INTR_ID,  					//invalid interrupt ID
				XPAR_SPI_SEND_BASEADDR,
				XPAR_SPI_SEND_HIGHADDR,
		},
		{
				DEV_NUM_SPI_RECV,
				INVALID_INTR_ID,  					//invalid interrupt ID
				XPAR_SPI_REC_BASEADDR,
				XPAR_SPI_REC_HIGHADDR,
		},
#endif /* DRIVER_ENABLE_SPI */
#ifdef DRIVER_ENABLE_SPI_NBYTE
		{
				DEV_NUM_SPI_NBYTE_SEND,
				INVALID_INTR_ID,  					//invalid interrupt ID
				SPI_NBYTE_SEND_BASEADDR,
				SPI_NBYTE_SEND_HIGHADDR
		},
		{
				DEV_NUM_SPI_NBYTE_RECV,
				INVALID_INTR_ID,  					//invalid interrupt ID
				SPI_NBYTE_REC_BASEADDR,
				SPI_NBYTE_REC_HIGHADDR
		},
#endif /* DRIVER_ENABLE_SPI_NBYTE */
#ifdef DRIVER_ENABLE_UART_2M
		{
				DEV_NUM_UART_1M_MST2MST,   // UART 1M master 2 master channel
				XPAR_CPU_AXI_INTC_0_LOW_SPEED_PERIPHERAL_UART_RXD_MM_TXD_DET_LITETOFIFO_V1_0_S00_AXI_0_INTRUPT_O_INTR,
				XPAR_LOW_SPEED_PERIPHERAL_UART_RXD_MM_TXD_DET_LITETOFIFO_V1_0_S00_AXI_0_BASEADDR,
				XPAR_LOW_SPEED_PERIPHERAL_UART_RXD_MM_TXD_DET_LITETOFIFO_V1_0_S00_AXI_0_HIGHADDR
		},
		{
				DEV_NUM_UART_1M_MST2SLV,   // UART 1M master 2 slave channel
				XPAR_CPU_AXI_INTC_0_LOW_SPEED_PERIPHERAL_UART_RXD_MS_TXD_REM_LITETOFIFO_V1_0_S00_AXI_0_INTRUPT_O_INTR,
				XPAR_LOW_SPEED_PERIPHERAL_UART_RXD_MS_TXD_REM_LITETOFIFO_V1_0_S00_AXI_0_BASEADDR,
				XPAR_LOW_SPEED_PERIPHERAL_UART_RXD_MS_TXD_REM_LITETOFIFO_V1_0_S00_AXI_0_HIGHADDR
		},
		{
				DEV_NUM_UART_1M_SLV2MST,   // UART 1M slave 2 master channel
				XPAR_CPU_AXI_INTC_0_LOW_SPEED_PERIPHERAL_UART_RXD_S_M_LITETOFIFO_V1_0_S00_AXI_0_INTRUPT_O_INTR,
				XPAR_LOW_SPEED_PERIPHERAL_UART_RXD_S_M_LITETOFIFO_V1_0_S00_AXI_0_BASEADDR,
				XPAR_LOW_SPEED_PERIPHERAL_UART_RXD_S_M_LITETOFIFO_V1_0_S00_AXI_0_HIGHADDR
		},
		{
				DEV_NUM_UART_1M_SLV2SLV,   // UART 1M slave 3 slave channel
				XPAR_CPU_AXI_INTC_0_LOW_SPEED_PERIPHERAL_UART_RXD_S_S_LITETOFIFO_V1_0_S00_AXI_0_INTRUPT_O_INTR,
				XPAR_LOW_SPEED_PERIPHERAL_UART_RXD_S_S_LITETOFIFO_V1_0_S00_AXI_0_BASEADDR,
				XPAR_LOW_SPEED_PERIPHERAL_UART_RXD_S_S_LITETOFIFO_V1_0_S00_AXI_0_HIGHADDR
		},
		{
				DEV_NUM_UART_2M_MST,   // UART 2M master send device
				XPAR_CPU_AXI_INTC_0_LOW_SPEED_PERIPHERAL_UART_2M_LITETOFIFO_V1_0_S00_AXI_0_INTRUPT_O_INTR,
				XPAR_LOW_SPEED_PERIPHERAL_UART_2M_LITETOFIFO_V1_0_S00_AXI_0_BASEADDR,
				XPAR_LOW_SPEED_PERIPHERAL_UART_2M_LITETOFIFO_V1_0_S00_AXI_0_HIGHADDR
		},

#endif /* DRIVER_ENABLE_UART_2M */

#ifdef DRIVER_ENABLE_GPIO
		{
				DEV_NUM_GPIO_0,
				INVALID_INTR_ID,  					//invalid interrupt ID
				XPAR_LOW_SPEED_PERIPHERAL_AXI_GPIO_0_BASEADDR,
				XPAR_LOW_SPEED_PERIPHERAL_AXI_GPIO_0_HIGHADDR,
		},
		{
				DEV_NUM_GPIO_1,
				INVALID_INTR_ID,  					//invalid interrupt ID
//				XPAR_AXI_GPIO_1_BASEADDR,
//				XPAR_AXI_GPIO_1_HIGHADDR,
		},
#endif /* DRIVER_ENABLE_GPIO */

#ifdef DRIVER_ENABLE_USER_IP
		{
				DEV_NUM_USER_0,
				INVALID_INTR_ID,  					//invalid interrupt ID  Fsync
				XPAR_M_REGLITE_CONFIG_1_BASEADDR,
				XPAR_M_REGLITE_CONFIG_1_HIGHADDR,
		},
#endif
		{
				DEV_NUM_USER_1,
				INVALID_INTR_ID,  					//invalid interrupt ID
				XPAR_R1_R2_DATAMOVER_M_REGLITE_CONFIG_0_BASEADDR,
				XPAR_R1_R2_DATAMOVER_M_REGLITE_CONFIG_0_HIGHADDR
		},
		{
				DEV_NUM_USER_2,
				INVALID_INTR_ID,  					//invalid interrupt ID
		},
		{
				DEV_NUM_USER_3,
				INVALID_INTR_ID,  					//invalid interrupt ID
		},
};