/*
 * platform_dev.h
 *
 *  Created on: 2018Äê10ÔÂ23ÈÕ
 *      Author: 508
 */

#ifndef SRC_PLATFORM_DEV_H_
#define SRC_PLATFORM_DEV_H_


#define MAX_DEV_NUM				255
#define INVALID_INTR_ID			0xff

typedef enum
{
	DEV_NUM_CAN_A = 0,		// CAN A device ID
	DEV_NUM_CAN_B,          // CAN B device ID

	DEV_NUM_UART_A,         // UART port receive command
	DEV_NUM_UART_B,         // UART port receive high speed data

	DEV_NUM_TIMER,          // timer device ID
	DEV_NUM_SEC_PULSE,      // second pulse device ID
	DEV_NUM_FSYNC_TIMER,    // timer with fsyn module
	DEV_NUM_FSYNC,          // fsyn module for data out
	DEV_NUM_FSYNC_1,        // fsyn module for data out
	DEV_NUM_FSYNC_2,        // fsyn module for data out

	DEV_NUM_ETH,			// Ethernet device ID
	DEV_NUM_DMA,            // DMA device ID

	DEV_NUM_IIC,            // IIC controller device ID
	DEV_NUM_INTR,           // interrupt controller device ID

	DEV_NUM_SPI_SEND,       // spi send device
	DEV_NUM_SPI_RECV,       // spi receive device

	DEV_NUM_SPI_NBYTE_SEND, // spi n-byte send device
	DEV_NUM_SPI_NBYTE_RECV, // spi n-byte receive device

	DEV_NUM_AXI_SWITCH,     // axi switch device, select data source

	DEV_NUM_UART_1M_MST2MST,   // uart 1M master 2 master channel
	DEV_NUM_UART_1M_MST2SLV,   // uart 1M master 2 slave channel

	DEV_NUM_UART_1M_SLV2MST,   // uart 1M slave 2 master channel
	DEV_NUM_UART_1M_SLV2SLV,   // uart 1M slave 3 slave channel

	DEV_NUM_UART_2M_MST,   // uart 2M master send device

	DEV_NUM_GPIO_0,         // GPIO device
	DEV_NUM_GPIO_1,         // GPIO device


	DEV_NUM_USER_0 = 0xe0,  // user define device0 ID
	DEV_NUM_USER_1,         // user define device1 ID
	DEV_NUM_USER_2,         // user define device2 ID
	DEV_NUM_USER_3,         // user define device3 ID

	DEV_NUM_INVALID = 0xff
}T_DEV_NUM;


/**
 * This typedef contains configuration information for the device.
 */
typedef struct dev_config{
	uint8_t  dev_id;		/* Unique ID  of device */
	uint8_t  intr_num;      /* Device interrupt index */
	uint32_t base_addr;	    /* Device base address */
	uint32_t high_addr;	    /* Device high address */
}S_DEV_CONF;

typedef struct dev_iic_arg
{
	uint8_t  iicNo;  // slave iic device identification
	uint32_t addr;   // operate base register address
	uint32_t len;    // valid data length
	uint8_t* src;    // source data ptr
}S_DEV_IIC_ARG;

typedef struct dev_com_arg
{
	uint32_t len;    // valid data length
	uint8_t* src;    // source data ptr
}S_DEV_COM_ARG;

typedef struct dev_can_arg
{
	uint32_t len;    // valid data length
	uint8_t* src;    // source data ptr
}S_DEV_CAN_ARG;

typedef struct dev_dma_arg
{
	uint8_t  dir;    // transfer direction
	uint32_t len;    // valid data length
	uint8_t* src;    // source data ptr
}S_DEV_DMA_ARG;

typedef struct dev_uart_arg
{
	uint32_t len;    // valid data length
	uint8_t* src;    // source data ptr
}S_DEV_UART_ARG;

typedef struct dev_spi_arg
{
	uint32_t len;    // valid data length
	uint8_t* src;    // source data ptr
}S_DEV_SPI_ARG;


typedef struct dev_info{
	S_DEV_CONF*           dev_conf;  // control register base address
	union  // device configuration list
	{
		S_DEV_IIC_ARG  * dev_iic_arg;
		S_DEV_CAN_ARG  * dev_can_arg;
		S_DEV_DMA_ARG  * dev_dma_arg;
		S_DEV_UART_ARG * dev_uart_arg;
		S_DEV_SPI_ARG  * dev_spi_arg;
		S_DEV_COM_ARG  * dev_com_arg;
	};
	struct platfrom_dev * platform_dev;
}S_DEV_INFO;



extern S_DEV_CONF gDevConfigTable[MAX_DEV_NUM];

#endif /* SRC_PLATFORM_DEV_H_ */

