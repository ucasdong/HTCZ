/******************************************************************************
*
* Copyright (C) 2008 - 2014 Xilinx, Inc.  All rights reserved.
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

#ifndef __PLATFORM_H_
#define __PLATFORM_H_

#include "include.h"
#include "./driver/driver.h"

#include "bufsts.h"
#include "platform_dev.h"
#include "user_interface.h"

#define __DEBUG_INFO__
#ifdef __DEBUG_INFO__
#define MASTER_NONE_DEBUG             0x00
#define MASTER_BUF_DEBUG              0x01
#define MASTER_CMD_DEBUG              0x02
#define MASTER_RW_DEBUG               0x04
#define MASTER_IO_DEBUG               0x08
#define MASTER_ALL_DEBUG              (MASTER_BUF_DEBUG | MASTER_CMD_DEBUG | MASTER_RW_DEBUG | MASTER_IO_DEBUG)

#define masterDbgLevel   			  MASTER_NONE_DEBUG

#define MST_BUF_MSG(f, a...)                              \
    do                                                          \
        {                                                       \
        if (masterDbgLevel & MASTER_BUF_DEBUG)              \
            xil_printf ("buf msg:"f, ## a);                     \
        } while (0)

#define MST_CMD_MSG(f, a...)                              \
    do                                                          \
        {                                                       \
        if (masterDbgLevel & MASTER_CMD_DEBUG)              \
            xil_printf ("cmd msg:"f, ## a);                     \
        } while (0)

#define MST_RW_MSG(f, a...)                              \
    do                                                          \
        {                                                       \
        if (masterDbgLevel & MASTER_RW_DEBUG)              \
            xil_printf ("rw msg:"f, ## a);                     \
        } while (0)
#define MST_IO_MSG(f, a...)                              \
    do                                                          \
        {                                                       \
        if (masterDbgLevel & MASTER_IO_DEBUG)              \
            xil_printf ("io msg:"f, ## a);                     \
        } while (0)

#else
#define MST_BUF_MSG(f, a...)
#define MST_CMD_MSG(f, a...)
#define MST_RW_MSG(f, a...)
#define MST_IO_MSG(f, a...)
#endif

// write register macro
#define WRITE_REG(addr, val) \
{\
	*(volatile uint32_t*)(addr) = val;\
}

// read register macro
#define READ_REG(addr) 	(*(volatile uint32_t*)(addr))

T_DEV_NUM	gProtocolDevNum;
T_DEV_NUM	gProtocolDevNumBak;

uint8_t     gProtocolBufNum;
uint8_t     gProtocolBufNumBak;

//uint8_t Xuart_RecvtmpBuffer[XUART_BUFFER_SIZE];

/* function define */
void init_platform();
void cleanup_platform();

void init_platform();
void cleanup_platform();
void delay_us(uint32_t us);

#endif
