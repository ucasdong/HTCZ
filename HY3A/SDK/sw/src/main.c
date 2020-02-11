/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */
/*
---  XIic_MasterRecv  can't support read from 0 iic addr  
---  XIic_MasterSend  can't support send to 0 iic addr   if send 0 iic addr will auto change FPGA IIC
---  Controller to slave mode  this will make FPGA IIC Controller err 
--   By setting        XIic_WriteReg(IicInstance.BaseAddress,0x110,IIC_FORBID_ADDRESS);
--   will avoid this err !!!!!
*/

#ifdef __MICROBLAZE__
#include "mb_interface.h"
#endif

#ifdef __PPC__
#include "xexception_l.h"
#endif

#include "./driver/driver.h"

/*
 * Function : getDatafromBuf
 * 		check buffer status and receive special data length from receive buffer
 * Param    :
 * 		bufNo  : buffer index
 * 		dataBuf: data buffer pointer
 * 		len    : data length
 *
 * Return   :
 * 		actually receive data length
 *
 * */
static uint8_t getDatafromBuf(uint8_t bufNo, uint8_t* dataBuf, uint8_t len)
{
    uint8_t m8bufSts = 0;
    uint8_t m8len    = 0;

    //check buffer status, not empty
    checkBufStatus(bufNo, &m8bufSts);
	if ((m8bufSts & BUF_INDATA) == BUF_INDATA || (m8bufSts & BUF_FULL) == BUF_FULL)
	{
	    //receive data from data buffer
		m8len = revDatafromBuf(bufNo, dataBuf, len); // get id0

		return m8len; //return actual data length
	}

	//if empty return 0
	return 0;
}


static void cmdMachine(void)
{
    uint8_t m8len    = 0;
    uint8_t m8uartbuffer[UART_CMD_LENGTH] = {0};

    //UART Command handle
    //receive data from data buffer UART
	m8len = getDatafromBuf(BUF_NUM_UART_A, m8uartbuffer, UART_CMD_LENGTH); // get UART command

	if ((m8len != UART_CMD_LENGTH) && (m8len > 0))
	{
		MST_BUF_MSG("%s : %d get invalid len %d from can buffer\n",__func__, __LINE__, m8len);
	}
	else if(m8len != 0)
	{
		//command handle
		uartCmdParse(m8uartbuffer);
	}

	m8len = getDatafromBuf(BUF_NUM_UART_B, m8uartbuffer, UART_CMD_LENGTH); // get UART command

	if ((m8len != UART_CMD_LENGTH) && (m8len > 0))
	{
		MST_BUF_MSG("%s : %d get invalid len %d from can buffer\n",__func__, __LINE__, m8len);
	}
	else if(m8len != 0)
	{
		//command handle
		uartCmdParse(m8uartbuffer);
	}

	uint8_t m8bufSts = 0;
#ifdef DRIVER_ENABLE_CAN

	checkBufStatus(BUF_NUM_CAN_A, &m8bufSts);
	if ((m8bufSts & BUF_INDATA) == BUF_INDATA || (m8bufSts & BUF_FULL) == BUF_FULL)  // if can buffer have valid data
	{
		//command handle
		CanCmdParse(BUF_NUM_CAN_A);
	}

	checkBufStatus(BUF_NUM_CAN_B, &m8bufSts);
	if ((m8bufSts & BUF_INDATA) == BUF_INDATA || (m8bufSts & BUF_FULL) == BUF_FULL)  // if can buffer have valid data
	{
		//command handle
		CanCmdParse(BUF_NUM_CAN_B);
	}
#endif
	//uint8_t m8bufSts = 0;
#if 0
	//receive data from data buffer UART_HIGH_SPEED_0
	checkBufStatus(BUF_NUM_UART_RECV_0, &m8bufSts);
	if ((m8bufSts & BUF_INDATA) == BUF_INDATA || (m8bufSts & BUF_FULL) == BUF_FULL)  // if can buffer have valid data
	{
		//command handle
		cmdParse(BUF_NUM_UART_RECV_0);
	}

	//receive data from data buffer UART_HIGH_SPEED_1
	checkBufStatus(BUF_NUM_UART_RECV_1, &m8bufSts);
	if ((m8bufSts & BUF_INDATA) == BUF_INDATA || (m8bufSts & BUF_FULL) == BUF_FULL)  // if can buffer have valid data
	{
		//command handle
		cmdParse(BUF_NUM_UART_RECV_1);
	}

	//receive data from data buffer UART_HIGH_SPEED_2
	checkBufStatus(BUF_NUM_UART_RECV_2, &m8bufSts);
	if ((m8bufSts & BUF_INDATA) == BUF_INDATA || (m8bufSts & BUF_FULL) == BUF_FULL)  // if can buffer have valid data
	{
		//command handle
		cmdParse(BUF_NUM_UART_RECV_2);
	}

	//receive data from data buffer UART_HIGH_SPEED_3
	checkBufStatus(BUF_NUM_UART_RECV_3, &m8bufSts);
	if ((m8bufSts & BUF_INDATA) == BUF_INDATA || (m8bufSts & BUF_FULL) == BUF_FULL)  // if can buffer have valid data
	{
		//command handle
		cmdParse(BUF_NUM_UART_RECV_3);
	}
#endif
	if((gSystemStatus & SYSTEM_STS_TIMER_INTR) == SYSTEM_STS_TIMER_INTR)
	{
		gSystemStatus &= ~SYSTEM_STS_TIMER_INTR;
		userTimerIntrHandler();
	}
}


int main()
{
#ifdef DRIVER_ENABLE_IIC
	delay_us(500000); //waiting IIC device
	int i;
#endif /* DRIVER_ENABLE_IIC */
	//switch
	Xil_Out32(0x230000+0x40,0x80000000);
	Xil_Out32(0x230000+0x44,0x0);
	Xil_Out32(0x230000+0x0,0x2);
//Fsyn_v1_1_S00_AXI_0		缁夘垰鍨庨弮鍫曟？閿涳拷0.6ms
	Xil_Out32(0x210000,0x5555);
	Xil_Out32(0x210004,0xEA60);	//缁夘垰鍨庨弮鍫曟？0.6ms
	Xil_Out32(0x210014,0x10);	//閼村鍟块惃鍕箒閺佸牆顔旀惔锟�
//	Xil_Out32(0x210000,0xaaaa);	//aaaa:鐢冩倱濮濄儰楠囬悽鐕傜礉bbbb閿涙艾褰ф禍褏鏁撴稉锟藉▎鈽呯礉5555閿涘苯鍙ч梻锟�
	//Fsyn_v1_1_S00_AXI_1		缁夘垰鍨庨弮鍫曟？閿涳拷2.4ms
	Xil_Out32(0x220000,0x5555);
	Xil_Out32(0x220004,0x3A980);	//缁夘垰鍨庨弮鍫曟？0.6ms
	Xil_Out32(0x220014,0x10);	//閼村鍟块惃鍕箒閺佸牆顔旀惔锟�
//	Xil_Out32(0x210000,0xaaaa);	//aaaa:鐢冩倱濮濄儰楠囬悽鐕傜礉bbbb閿涙艾褰ф禍褏鏁撴稉锟藉▎鈽呯礉5555閿涘苯鍙ч梻锟�




	init_platform();

	ioDevTest();
	//L1L2/M_RegLite_Config_0

	Xil_Out32(0x130000,0x3038);	//wrcmd hsize 12288+40=12328
	Xil_Out32(0x130004,0x80);	//wrcmd vsize 64
	Xil_Out32(0x130008,0x6070);	//rdcmd hsize (12288+40=12328)x2 = 24656
	Xil_Out32(0x13000c,0x40);	//rdcmd vsize 32

	Xil_Out32(0x130010,0x6070);	//wrcmd hsize 24576+40=24616 == 0x6028
	Xil_Out32(0x130014,0x40);	//wrcmd vsize 64
	Xil_Out32(0x130018,0x6070);	//rdcmd hsize
	Xil_Out32(0x13001c,0x40);	//rdcmd vsize 32
//	M_RegLite_Config_1

	Xil_Out32(0x80008,0x3000);	//ImageNub:12288

	Xil_Out32(0x8000c,0x6000);	//ImageNub:24576
	Xil_Out32(0x210000,0xAAAA);
	Xil_Out32(0x220000,0xAAAA);

	while(1)
	{
	//	for(i=0;i< 126; i++)
//		{
//			Xil_Out32(0x210000,0x5555);
//			delay_us(500000);
//		}
		//all command machine
		cmdMachine();
	}

	cleanup_platform();

	return 0;
}
