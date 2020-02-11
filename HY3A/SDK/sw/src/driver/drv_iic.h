/*
 * drv_iic.h
 *
 *  Created on: 2018Äê9ÔÂ14ÈÕ
 *      Author: 508
 */

#ifndef SRC_DRV_IIC_H_
#define SRC_DRV_IIC_H_

//if use iic

#include "driver.h"

#ifdef DRIVER_ENABLE_IIC

#include "xiic.h"

/*macro define*/
#define IIC_SUCCESS                 0
#define IIC_FAIL                    1

#define EEPROM_TEST_START_ADDRESS   128

#define IIC_SWITCH_ADDRESS 			0x74    // first switch mux address   8 device (dev7 connect second switch)
#define IIC_SWITCH_SECOND_ADDRESS   0x75    // second switch mux address  4 device (all connect b203 device)

#define IIC_FORBID_ADDRESS 			0x02    //on this board no DEVICE CAN USE this ADDR
#define IIC_SI570433_ADDRESS 		0x00

#define IIC_SI5324_ADDRESS 			0x68

#define IIC_DM100MB203A_ADDRESS     0x64    // dm100MB203 A device address
#define IIC_DM100MB203B_ADDRESS     0x65    // dm100MB203 B device address
#define IIC_DM100MB203C_ADDRESS     0x66    // dm100MB203 C device address
#define IIC_DM100MB203D_ADDRESS     0x67    // dm100MB203 D device address

#define IIC_BUS_0 					0x01    // EEPROM
#define IIC_BUS_1 					0x02    // SI570 433  iic addr is 0
#define IIC_BUS_2 					0x04    // SI570 433  iic addr is 0
#define IIC_BUS_3 					0x08    // ddr3  ??
#define IIC_BUS_4 					0x10    // SI 5324   iic addr is 0x68
#define IIC_BUS_5 					0x20    // QSFP     ???
#define IIC_BUS_6 					0x40    // QSFP     ???
#define IIC_BUS_7 					0x80    // pc9546    IIC ADDR is 0x75

#define IIC_BUS2_0 					0x01    // B203A
#define IIC_BUS2_1 					0x02    // B203B
#define IIC_BUS2_2 					0x04    // B203C
#define IIC_BUS2_3 					0x08    // B203D

#define IIC_DEVICE_ID               XPAR_IIC_0_DEVICE_ID
#define INTC_DEVICE_ID              XPAR_INTC_0_DEVICE_ID
#define IIC_INTR_ID                 XPAR_INTC_0_IIC_0_VEC_ID

#define PAGE_SIZE   				16

#define IIC_CTRL_PARM_CONFIG        0x01
#define IIC_CTRL_MASTER_SELECT      0x02
#define IIC_CTRL_SLAVE_SELECT       0x03



#define IIC_OUTPUT_CLK_100M				0x01
#define IIC_OUTPUT_CLK_140M	    		0x02
#define IIC_OUTPUT_CLK_156_25M			0x03

/* device IIC operation functions define */
typedef struct dev_iic_ops
{
	/* IIC OPS */
	int (*init)( S_DEV_INFO * dev );
	int (*write)( S_DEV_INFO * dev );
	int (*read)( S_DEV_INFO * dev );
	int (*ctrl)( S_DEV_INFO * dev );
}S_DEV_IIC_OPS;


/* structure define */
typedef u8 AddressType;

typedef enum dev_num{
	DEV_IIC_SWITCH = 0,
	DEV_IIC_SI570433A ,
	DEV_IIC_SI570433B ,
	DEV_IIC_SI5324	  ,
	DEV_IIC_B203A     ,
	DEV_IIC_B203B     ,
	DEV_IIC_B203C     ,
	DEV_IIC_B203D     ,

	DEV_INVALID = 0xff
}S_DEV_NUM;

extern S_DEV_IIC_OPS platform_dev_iic_ops;

XIic IicInstance;		/* The instance of the IIC device. */

u8 WriteBuffer[sizeof(AddressType) + PAGE_SIZE];
u8 UpdateBuffer[sizeof(AddressType) + PAGE_SIZE];

u8 ReadBuffer[PAGE_SIZE];	/* Read buffer for reading a page. */

volatile u8 TransmitComplete;	/* Flag to check completion of Transmission */
volatile u8 ReceiveComplete;	/* Flag to check completion of Reception */


int EepromWriteData(u16 ByteCount);
int EepromReadData(u8 *BufferPtr, u16 ByteCount);
int EepromReadData2(AddressType addr, u8 *BufferPtr, u16 ByteCount);

uint8_t iic_select_dev(S_DEV_NUM devNo);
uint8_t iic_write_data(uint8_t* dataBuf, uint8_t startAddr, uint8_t len);
uint8_t iic_read_data(uint8_t* dataBuf, uint8_t startAddr, uint8_t len);
void config_b203(uint8_t iicNo);
void config_si570(uint8_t mode);
void config_si5324(uint8_t mode);

#endif /* DRIVER_ENABLE_IIC */

#endif /* SRC_DRV_IIC_H_ */
