/*
 * drv_can.h
 *
 *  Created on: 2018年9月13日
 *      Author: 508
 */

#ifndef SRC_DRV_CAN_H_
#define SRC_DRV_CAN_H_

#include "driver.h"

#ifdef DRIVER_ENABLE_CAN

#define DEV_CAN_A           0
#define DEV_CAN_B           1

#define CAN_A_CTRL_BASEADDR XPAR_MBLAZE_SAJ1000_V1_0_S00_AXI_0_BASEADDR   //this should be changed with actually device control address
#define CAN_B_CTRL_BASEADDR 0x42100000   //this should be changed with actually device control address

#define CAN_SUCCESS         0
#define CAN_FAIL            1

#define MAX_TIMEOUT            		0x0fffffff

#define CAN_REG_WR					0x00					/*CAN总线写控制寄存器偏移地址*/
#define CAN_REG_RD_ADDR				0x04					/*CAN总线读地址寄存器偏移地址*/
#define CAN_REG_RD_DATA				0x08					/*CAN总线读数据寄存器偏移地址*/
//#define CAN_ADDR_DIFF				0x10000					/*AB总线基址差*/
#define CANA                        0
#define CANB                        1
/*CAN地址*/
#define CAN_MOD						0x00					/*模式控制寄存器地址*/
#define CAN_CMR						0x01					/*命令寄存器地址*/
#define CAN_SR						0x02					/*报文缓冲区状态寄存器地址*/
#define CAN_IR						0x03					/*中断标志寄存器地址*/
#define CAN_IER						0x04					/*中断使能寄存器地址*/
#define CAN_BTR0					0x06					/*总线定时寄存器地址0*/
#define CAN_BTR1					0x07					/*总线定时寄存器地址1*/
#define CAN_OCR						0x08					/*输出控制寄存器地址*/
#define CAN_ACR0					0x10					/*验收码寄存器地址0*/
#define CAN_ACR1					0x11					/*验收码寄存器地址1*/
#define CAN_ACR2					0x12					/*验收码寄存器地址2*/
#define CAN_ACR3					0x13					/*验收码寄存器地址3*/
#define CAN_AMR0					0x14					/*验收码屏蔽寄存器地址0*/
#define CAN_AMR1					0x15					/*验收码屏蔽寄存器地址1*/
#define CAN_AMR2					0x16					/*验收码屏蔽寄存器地址2*/
#define CAN_AMR3					0x17					/*验收码屏蔽寄存器地址3*/
#define CAN_RXFIFO					0x1E					/*接收缓冲器FIFO起始地址寄存器地址*/
#define CAN_CDR						0x1F					/*时钟分频寄存器地址*/
#define CAN_RXBUF					0x10					/*接收缓冲器起始地址寄存器地址*/
#define CAN_TXBUF					0x10					/*发送缓冲器起始地址寄存器地址*/



/*CAN常量*/
#define C_CAN_MOD_RESET				0x01					/*设置为复位模式MOD.0=1,双滤波MOD.3=0*/
#define C_CAN_MOD_WORK				0x00					/*设置为工作模式，关闭自我测试模式和只听模式，清除休眠模式*/
#define C_CAN_IER_DISABLE			0x00					/*中断使能寄存器，禁止CAN所有中断*/
#define C_CAN_IER_RECEIVE			0x01					/*中断使能寄存器，使能CAN接收中断*/
#define C_CAN_BTR0					0x00					/*总线定时寄存器地址0*/
#define C_CAN_BTR1					0x1C					/*总线定时寄存器地址1*/
#define C_CAN_OCR					0x1A					/*输出控制寄存器，设置为正常输出模式，TX1悬空，TX2推挽*/
//user should configure acr and amr register
#define C_CAN_ACR0					0x0E					/*验收代码0*/
#define C_CAN_ACR1					0x00					/*验收代码1*/
#define C_CAN_ACR2					0x10					/*验收代码2*/
#define C_CAN_ACR3					0x60					/*验收代码3*/
#define C_CAN_AMR0					0xff					/*屏蔽代码0*/
#define C_CAN_AMR1					0xff					/*屏蔽代码1*/
#define C_CAN_AMR2					0xff					/*屏蔽代码2*/
#define C_CAN_AMR3					0xff					/*屏蔽代码3*/
#define C_CAN_RXFIFO				0x00					/*设置接收缓冲器FIFO起始地址为0*/
#define C_CAN_CDR					0xC8					/*时钟分频寄存器，设置时钟分频器，选择PeliCAN模式，关闭时钟*/

#define C_CMR_RRB					0x4						/*释放接收缓冲器*/
#define C_CMR_TR					0x1						/*发送请求*/

#define C_SR_TCS_COMPLETE			0x8						/*发送完成状态*/
#define C_SR_TBS_EMPTY				0x4						/*发送缓冲器状态*/
#define C_SR_RBS_READY				0x1						/*接收缓冲器状态*/

/*CAN寄存器设置值定义*/
#define CCAN_SR \
	   (CAN_SR << 16) | 00
#define CCAN_MOD_RESET \
			(CAN_MOD << 16) | C_CAN_MOD_RESET
#define CCAN_MOD_WORK \
			(CAN_MOD << 16) | C_CAN_MOD_WORK
#define CCAN_IER_DISABLE \
			(CAN_IER << 16) | C_CAN_IER_DISABLE
#define	CCAN_IER_RECEIVE \
			(CAN_IER << 16) | C_CAN_IER_RECEIVE
#define CCAN_BTR0 \
			(CAN_BTR0 << 16) | C_CAN_BTR0
#define CCAN_BTR1 \
			(CAN_BTR1 << 16) | C_CAN_BTR1
#define CCAN_OCR \
			(CAN_OCR << 16) | C_CAN_OCR
#define CCAN_ACR0 \
			(CAN_ACR0 << 16) | C_CAN_ACR0
#define CCAN_ACR1 \
			(CAN_ACR1 << 16) | C_CAN_ACR1
#define CCAN_ACR2 \
			(CAN_ACR2 << 16) | C_CAN_ACR2
#define CCAN_ACR3 \
			(CAN_ACR3 << 16) | C_CAN_ACR3
#define CCAN_AMR0 \
			(CAN_AMR0 << 16) | C_CAN_AMR0
#define CCAN_AMR1 \
			(CAN_AMR1 << 16) | C_CAN_AMR1
#define CCAN_AMR2 \
			(CAN_AMR2 << 16) | C_CAN_AMR2
#define CCAN_AMR3 \
			(CAN_AMR3 << 16) | C_CAN_AMR3
#define CCAN_RXFIFO \
			(CAN_RXFIFO << 16) | C_CAN_RXFIFO
#define CCAN_CDR \
			(CAN_CDR << 16) | C_CAN_CDR
#define	CCAN_CMR_RRB \
			(CAN_CMR << 16) | C_CMR_RRB
#define CCAN_CMR_TR \
			(CAN_CMR << 16) | C_CMR_TR
#define CCAN_ANSWER0 \
			(CAN_TXBUF << 16) | 0x08
#define CCAN_ANSWER1 \
			((CAN_TXBUF +1) << 16) | 0x50
#define CCAN_ANSWER2 \
			((CAN_TXBUF + 2) << 16) | 0x40
#define CCAN_ANSWER3 \
			((CAN_TXBUF + 3) << 16) | 0x10
#define CCAN_ANSWER4 \
			((CAN_TXBUF + 4) << 16) | 0xFF
#define CCAN_ANSWER5 \
			((CAN_TXBUF + 5) << 16) | 0x20
#define CCAN_ANSWER6 \
			((CAN_TXBUF + 6) << 16) | 0xFF
#define CCAN_ANSWER7 \
			((CAN_TXBUF + 7) << 16) | 0x20
#define CCAN_ANSWER8 \
			((CAN_TXBUF + 8) << 16) | 0xFF
#define CCAN_ANSWER9 \
			((CAN_TXBUF + 9) << 16) | 0x20
#define CCAN_ANSWER10 \
			((CAN_TXBUF + 10) << 16) | 0x6D
#define CCAN_TELEMETRY_ANSWER0 \
			(CAN_TXBUF << 16 ) | 0x08
#define CCAN_TELEMETRY_ANSWER1 \
			((CAN_TXBUF + 1) << 16 ) | 0x90
#define CCAN_TELEMETRY_ANSWER2 \
			((CAN_TXBUF + 2) << 16 ) | 0x60
#define CCAN_TELEMETRY_ANSWER3_ADDR \
			((CAN_TXBUF + 3) << 16 )
#define CCAN_TELEMETRY_ANSWER4_ADDR(i) \
			((CAN_TXBUF + 4 + i) << 16 )
/*#define CCAN_TELEMETRY_ANSWER4_ADDR(i) \
			((CAN_TXBUF + 4 + i) << 16 )*/
#define CCAN_DATABOLCK_ANSWER0 \
			(CAN_TXBUF << 16) | 0x08
#define CCAN_DATABOLCK_ANSWER1 \
			((CAN_TXBUF +1) << 16) | 0x50
#define CCAN_DATABOLCK_ANSWER2 \
			((CAN_TXBUF + 2) << 16) | 0x40
#define CCAN_DATABOLCK_ANSWER3 \
			((CAN_TXBUF + 3) << 16) | 0x10
#define CCAN_DATABOLCK_ANSWER4 \
			((CAN_TXBUF + 4) << 16) | 0xFF
#define CCAN_DATABOLCK_ANSWER5 \
			((CAN_TXBUF + 5) << 16) | 0x40
#define CCAN_DATABOLCK_ANSWER6 \
			((CAN_TXBUF + 6) << 16) | 0xFF
#define CCAN_DATABOLCK_ANSWER7 \
			((CAN_TXBUF + 7) << 16) | 0x40
#define CCAN_DATABOLCK_ANSWER8 \
			((CAN_TXBUF + 8) << 16) | 0xFF
#define CCAN_DATABOLCK_ANSWER9 \
			((CAN_TXBUF + 9) << 16) | 0x40
#define CCAN_DATABOLCK_ANSWER10 \
			((CAN_TXBUF + 10) << 16) | 0xCD
#define CCAN_IER \
	   (CAN_IER << 16) | 0xff


/* Interrupt Register Bit define */
#define INT_RI			0x01	//Receive Interrupt

/* Interrupt Enable Register Bit define */
#define INT_EN_RI		0x01	//Receive Interrupt Enable

#define MAX_LEN_TX_ONCE         10

#define EXTEND_FRAME_FORMAT 	0x80	//define the EFF


typedef struct can_data{
	uint8_t frame_info;
	uint8_t id0;
	uint8_t id1;
	uint8_t data[8];
}S_CAN_DATA;

/* device CAN operation functions define */
typedef struct dev_can_ops
{
    /* CAN OPS */
    int (*init)( S_DEV_INFO * dev);
    int  (*write)( S_DEV_INFO * dev);
    int  (*read)( S_DEV_INFO * dev);
    void (*ctrl)(uint8_t *arg);
} S_DEV_CAN_OPS;

extern S_DEV_CAN_OPS platform_dev_can_ops;

/******************* Function define *********************/
uint8_t drv_can_init(void);
uint8_t driver_can_init(S_DEV_CONF config);
//uint8_t drv_can_transmit(uint8_t canNo, uint8_t* data, uint8_t len);
//void drv_can_isr(uint8_t canNo);

#endif /* DRIVER_ENABLE_CAN */

#endif /* SRC_DRV_CAN_H_ */
