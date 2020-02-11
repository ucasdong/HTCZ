/******************************************************************************
*
* Copyright (C) 2010 - 2015 Xilinx, Inc.  All rights reserved.
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

#include "xil_cache.h"


#include "./driver/driver.h"

/*
 * Uncomment one of the following two lines, depending on the target,
 * if ps7/psu init source files are added in the source directory for
 * compiling example outside of SDK.
 */
/*#include "ps7_init.h"*/
/*#include "psu_init.h"*/

#ifdef STDOUT_IS_16550
 #include "xuartns550_l.h"

 #define UART_BAUD 9600
#endif

#define UARTLITE_DEVICE_ID	XPAR_UARTLITE_0_DEVICE_ID

void
enable_caches()
{
#ifdef __PPC__
    Xil_ICacheEnableRegion(CACHEABLE_REGION_MASK);
    Xil_DCacheEnableRegion(CACHEABLE_REGION_MASK);
#elif __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    Xil_DCacheEnable();
#endif
#endif
}

void
disable_caches()
{
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheDisable();
#endif
#endif
}

void
init_uart()
{
#ifdef STDOUT_IS_16550
    XUartNs550_SetBaud(STDOUT_BASEADDR, XPAR_XUARTNS550_CLOCK_HZ, UART_BAUD);
    XUartNs550_SetLineControlReg(STDOUT_BASEADDR, XUN_LCR_8_DATA_BITS);
#endif
    /* Bootrom/BSP configures PS7/PSU UART to 115200 bps */
}


static void parameter_init(void)
{
	//message buffer initial
	bufattr_init();

	//global variable initial
	platform_dev_init();

	globalVarInit();

	g32SecondCnt    = 0;
	g32SecondCntTmp = 0;
	gUpdateTimeFlag = 0;
	g8PauseTimeFlag = 0;
	//gGPSDataValid   = 0xaaaa; //default valid data

	g8broadcastmask = 0;
	g8TelemetryMask = 0;
}

static void device_config(void)
{
	uint8_t databuf[0x10] = {0};

    //ioDevCtrl start INTR
    databuf[0] = CTRL_INTC_START;
    ioDevCtrl(DEV_NUM_INTR, databuf);

    //ioDevCtrl start Timer

#ifdef DRIVER_ENABLE_GPIO
	//ioDevCtrl set gpio default output
	databuf[0] = GPIO_CMD_SET_DIR;
	databuf[1] = 0;
	databuf[2] = 0;
	databuf[3] = 0;
	databuf[4] = 0;

	ioDevCtrl(DEV_NUM_GPIO_0, databuf);

	//set default receive master data
	databuf[0] = 0;
	databuf[1] = 0;
	//databuf[2] = 0xf0;ziceshi
	databuf[2] = 0xfc;
	databuf[3] = 0xff;

	ioDevWrite(DEV_NUM_GPIO_0, databuf, 4);

	//ioDevOpen GPIO_1
//	ioDevOpen(DEV_NUM_GPIO_1);
#endif /* DRIVER_ENABLE_GPIO */

#ifdef DRIVER_ENABLE_IIC
	S_DEV_IIC_ARG i2cArg;

    //set axi iic to master mode
	XIic_WriteReg(IicInstance.BaseAddress,XIIC_ADR_REG_OFFSET,IIC_FORBID_ADDRESS);

	//ioDevCtrl config si570
	i2cArg.iicNo = DEV_IIC_SI570433A;
	databuf[0] = IIC_CTRL_PARM_CONFIG;
	databuf[1] = IIC_OUTPUT_CLK_156_25M;  //set default clock to 100M
	i2cArg.src = databuf;
	ioDevCtrl(DEV_NUM_IIC, (uint8_t*)&i2cArg);

	i2cArg.iicNo = DEV_IIC_SI570433B;
	databuf[0] = IIC_CTRL_PARM_CONFIG;
	databuf[1] = IIC_OUTPUT_CLK_100M;  //set default clock to 100M
	i2cArg.src = databuf;
	ioDevCtrl(DEV_NUM_IIC, (uint8_t*)&i2cArg);

	//ioDevCtrl config si5324
	i2cArg.iicNo = DEV_IIC_SI5324;
	databuf[0] = IIC_CTRL_PARM_CONFIG;
	databuf[1] = IIC_OUTPUT_CLK_156_25M;  //set default clock to 100M
	i2cArg.src = databuf;
	ioDevCtrl(DEV_NUM_IIC, (uint8_t*)&i2cArg);

	//ioDevCtrl config b203
	i2cArg.iicNo = DEV_IIC_B203A;
	databuf[0] = IIC_CTRL_PARM_CONFIG;
	i2cArg.src = databuf;
	ioDevCtrl(DEV_NUM_IIC, (uint8_t*)&i2cArg);
	i2cArg.iicNo = DEV_IIC_B203B;
	databuf[0] = IIC_CTRL_PARM_CONFIG;
	i2cArg.src = databuf;
	ioDevCtrl(DEV_NUM_IIC, (uint8_t*)&i2cArg);
	i2cArg.iicNo = DEV_IIC_B203C;
	databuf[0] = IIC_CTRL_PARM_CONFIG;
	i2cArg.src = databuf;
	ioDevCtrl(DEV_NUM_IIC, (uint8_t*)&i2cArg);
	i2cArg.iicNo = DEV_IIC_B203D;
	databuf[0] = IIC_CTRL_PARM_CONFIG;
	i2cArg.src = databuf;
	ioDevCtrl(DEV_NUM_IIC, (uint8_t*)&i2cArg);
#endif /* DRIVER_ENABLE_IIC */

#ifdef DRIVER_ENABLE_AXI_SWITCH
	databuf[0] = AXI_SWITCH_CMD_M0_S0;
	ioDevCtrl(DEV_NUM_AXI_SWITCH, databuf);
#endif /* DRIVER_ENABLE_AXI_SWITCH */


#ifdef DRIVER_ENABLE_SECOND_PULSE
	uint32_t  second_pulse_arg = 0;

    //ioDevCtrl setup second pulse period and valid width
    databuf[0] = SEC_PULSE_CMD_SET_PERIOD;
    second_pulse_arg = 29125000; // 250ms
    databuf[1] = ((second_pulse_arg >> 24) & 0xff);
    databuf[2] = ((second_pulse_arg >> 16) & 0xff);
    databuf[3] = ((second_pulse_arg >>  8) & 0xff);
    databuf[4] = ((second_pulse_arg      ) & 0xff);
    ioDevCtrl(DEV_NUM_SEC_PULSE, databuf);

    databuf[0] = SEC_PULSE_CMD_SET_WIDTH;
    second_pulse_arg = 100000;
    databuf[1] = ((second_pulse_arg >> 24) & 0xff);
	databuf[2] = ((second_pulse_arg >> 16) & 0xff);
	databuf[3] = ((second_pulse_arg >>  8) & 0xff);
	databuf[4] = ((second_pulse_arg      ) & 0xff);
	ioDevCtrl(DEV_NUM_SEC_PULSE, databuf);
    //ioDevCtrl start second pulse
	databuf[0] = SEC_PULSE_CMD_START;
	ioDevCtrl(DEV_NUM_SEC_PULSE, databuf);
#endif /* DRIVER_ENABLE_SECOND_PULSE */

#ifdef DRIVER_ENABLE_FSYNC
	uint32_t  fsync_arg = 0;

    //ioDevCtrl start second pulse
	databuf[0] = FSYN_CMD_STOP;
	ioDevCtrl(DEV_NUM_FSYNC, databuf);
    //ioDevCtrl setup second pulse period and valid width
    databuf[0] = FSYN_CMD_SET_PERIOD;
//    fsync_arg = 0x15d90;
	fsync_arg = 12675;
    databuf[1] = ((fsync_arg >> 24) & 0xff);
    databuf[2] = ((fsync_arg >> 16) & 0xff);
    databuf[3] = ((fsync_arg >>  8) & 0xff);
    databuf[4] = ((fsync_arg      ) & 0xff);
    ioDevCtrl(DEV_NUM_FSYNC, databuf);

    databuf[0] = FSYN_CMD_SET_WIDTH;
    fsync_arg = 10;
    databuf[1] = ((fsync_arg >> 24) & 0xff);
	databuf[2] = ((fsync_arg >> 16) & 0xff);
	databuf[3] = ((fsync_arg >>  8) & 0xff);
	databuf[4] = ((fsync_arg      ) & 0xff);
	ioDevCtrl(DEV_NUM_FSYNC, databuf);

    //ioDevCtrl start second pulse
	databuf[0] = FSYN_CMD_START;
	ioDevCtrl(DEV_NUM_FSYNC, databuf);

#endif /* DRIVER_ENABLE_FSYNC */

	//ioDevCtrl start second pulse
	databuf[0] = FSYN_CMD_STOP;
	ioDevCtrl(DEV_NUM_FSYNC_1, databuf);
	//ioDevCtrl setup second pulse period and valid width
	databuf[0] = FSYN_CMD_SET_PERIOD;
//    fsync_arg = 0x15d90;
	fsync_arg = 0x22B83EA0; //5s
	databuf[1] = ((fsync_arg >> 24) & 0xff);
	databuf[2] = ((fsync_arg >> 16) & 0xff);
	databuf[3] = ((fsync_arg >>  8) & 0xff);
	databuf[4] = ((fsync_arg      ) & 0xff);
	ioDevCtrl(DEV_NUM_FSYNC_1, databuf);

	databuf[0] = FSYN_CMD_SET_WIDTH;
	fsync_arg = 11650; // 100us
	databuf[1] = ((fsync_arg >> 24) & 0xff);
	databuf[2] = ((fsync_arg >> 16) & 0xff);
	databuf[3] = ((fsync_arg >>  8) & 0xff);
	databuf[4] = ((fsync_arg      ) & 0xff);
	ioDevCtrl(DEV_NUM_FSYNC_1, databuf);

	//ioDevCtrl start second pulse
	databuf[0] = FSYN_CMD_START;
	ioDevCtrl(DEV_NUM_FSYNC_1, databuf);

	databuf[0] = FSYN_CMD_STOP;
	ioDevCtrl(DEV_NUM_FSYNC_2, databuf);
	//ioDevCtrl setup second pulse period and valid width
	databuf[0] = FSYN_CMD_SET_PERIOD;
//    fsync_arg = 0x15d90;
	fsync_arg = 2502; //5s
	databuf[1] = ((fsync_arg >> 24) & 0xff);
	databuf[2] = ((fsync_arg >> 16) & 0xff);
	databuf[3] = ((fsync_arg >>  8) & 0xff);
	databuf[4] = ((fsync_arg      ) & 0xff);
	ioDevCtrl(DEV_NUM_FSYNC_2, databuf);

	databuf[0] = FSYN_CMD_SET_WIDTH;
	fsync_arg = 10; //
	databuf[1] = ((fsync_arg >> 24) & 0xff);
	databuf[2] = ((fsync_arg >> 16) & 0xff);
	databuf[3] = ((fsync_arg >>  8) & 0xff);
	databuf[4] = ((fsync_arg      ) & 0xff);
	ioDevCtrl(DEV_NUM_FSYNC_2, databuf);

	databuf[0] = 0x5a;
	databuf[1] = 0x5a;
	databuf[2] = 0x5a;
	databuf[3] = 0x5a;
	ioDevWrite(DEV_NUM_UART_2M_MST, &databuf[0], 4);

	//ioDevCtrl start second pulse
	databuf[0] = FSYN_CMD_START;
	ioDevCtrl(DEV_NUM_FSYNC_2, databuf);
}

void
init_platform()
{
    /*
     * If you want to run this example outside of SDK,
     * uncomment one of the following two lines and also #include "ps7_init.h"
     * or #include "ps7_init.h" at the top, depending on the target.
     * Make sure that the ps7/psu_init.c and ps7/psu_init.h files are included
     * along with this example source files for compilation.
     */
    /* ps7_init();*/
    /* psu_init();*/
//	disable_caches();
	Xil_DCacheDisable();
	Xil_ICacheDisable();
//	enable_caches();

    //initial global var
    parameter_init();

    //ioDevOpen INTR
    ioDevOpen(DEV_NUM_INTR); //always enable interrupt device

#ifdef DRIVER_ENABLE_UART  //enable uart device
    //ioDevOpen UART
    ioDevOpen(DEV_NUM_UART_A);
    ioDevOpen(DEV_NUM_UART_B);
#endif /*DRIVER_ENABLE_UART*/

#ifdef DRIVER_ENABLE_SECOND_PULSE  //enable second pulse device
    //ioDevOpen Second Pulse
    ioDevOpen(DEV_NUM_SEC_PULSE);
    ioDevOpen(DEV_NUM_FSYNC_TIMER);
#endif
#ifdef DRIVER_ENABLE_FSYNC        //enable FSYNC device
    //ioDevOpen fsync, sim data control
    ioDevOpen(DEV_NUM_FSYNC);
    ioDevOpen(DEV_NUM_FSYNC_1);
    ioDevOpen(DEV_NUM_FSYNC_2);
#endif /* DRIVER_ENABLE_FSYNC */

#ifdef DRIVER_ENABLE_IIC
    //ioDevOpen IIC
    ioDevOpen(DEV_NUM_IIC);
#endif /* #ifdef DRIVER_ENABLE_IIC */

#ifdef DRIVER_ENABLE_SPI
    //ioDevOpen SPI SEND
    ioDevOpen(DEV_NUM_SPI_SEND);
    //ioDevOpen SPI RECV
    ioDevOpen(DEV_NUM_SPI_RECV);
#endif /* DRIVER_ENABLE_SPI */

#ifdef DRIVER_ENABLE_SPI_NBYTE
    //ioDevOpen SPI SEND
    ioDevOpen(DEV_NUM_SPI_NBYTE_SEND);
    //ioDevOpen SPI RECV
    ioDevOpen(DEV_NUM_SPI_NBYTE_RECV);
#endif /* DRIVER_ENABLE_SPI */

#ifdef DRIVER_ENABLE_AXI_SWITCH
    ioDevOpen(DEV_NUM_AXI_SWITCH);
#endif /* DRIVER_ENABLE_AXI_SWITCH */

#ifdef DRIVER_ENABLE_CAN //enable CAN device
    //ioDevOpen CAN A
	ioDevOpen(DEV_NUM_CAN_A);
    //ioDevOpen CAN B
    ioDevOpen(DEV_NUM_CAN_B);
#endif /*DRIVER_ENABLE_CAN */

#ifdef DRIVER_ENABLE_UART_2M
    ioDevOpen( DEV_NUM_UART_1M_MST2MST);   // uart 1M master 2 master channel
    ioDevOpen( DEV_NUM_UART_1M_MST2SLV);   // uart 1M master 2 slave channel
	ioDevOpen( DEV_NUM_UART_1M_SLV2MST);   // uart 1M slave 2 master channel
	ioDevOpen( DEV_NUM_UART_1M_SLV2SLV);   // uart 1M slave 3 slave channel
	ioDevOpen( DEV_NUM_UART_2M_MST);       // uart 2M master send device
#endif /* DRIVER_ENABLE_UART_2M */

#ifdef DRIVER_ENABLE_USER_IP
    ioDevOpen(DEV_NUM_USER_0);
    ioDevOpen(DEV_NUM_USER_1);
#endif /* DRIVER_ENABLE_AXI_SWITCH */

#ifdef DRIVER_ENABLE_GPIO
    //ioDevOpen GPIO_0
	ioDevOpen(DEV_NUM_GPIO_0);
	//ioDevOpen GPIO_1
//	ioDevOpen(DEV_NUM_GPIO_1);
#endif /* DRIVER_ENABLE_GPIO */

	plConfig();

    device_config();

	return ;
}

void delay_us(uint32_t us)
{
	uint32_t m32loop = 0;
	uint32_t m32delay = 0;

	for(m32delay = 0; m32delay < us; m32delay++)
	{
		for(m32loop = 0; m32loop < 8; m32loop++);
	}
}

void
cleanup_platform()
{
    disable_caches();
}
