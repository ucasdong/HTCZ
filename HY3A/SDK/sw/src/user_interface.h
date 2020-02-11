/*
 * user_config.h
 *
 *  Created on: 2018Äê9ÔÂ16ÈÕ
 *      Author: Administrator
 */

#ifndef SRC_USER_INTERFACE_H_
#define SRC_USER_INTERFACE_H_

#include "./driver/driver.h"

/******************************* SERIAL COMMAND DEFINE *******************************/
#define UART_CMD_LENGTH			    0x10
//UART send control command
//global reset
#define UART_CMD_RESET_BUF	    	0x00
#define UART_CTRL_CMD_SYNC          0x01
//only control command, transmit command or set broadcast message
#define UART_CTRL_CMD_ID10			0x10  // send bus remote control command
#define UART_CTRL_CMD_ID11			0x11  // set GPS time broadcast base time
#define UART_CTRL_CMD_ID12			0x12  // set satellite ass data broadcast
#define UART_CTRL_CMD_ID13			0x13  // set camera ass data broadcast
#define UART_CTRL_CMD_ID14			0x14  // send internal reset CAN bus broadcast
#define UART_CTRL_CMD_ID15			0x15  // send upload data broadcast
#define UART_CTRL_CMD_ID16			0x16  // send detector line cycle broadcast
#define UART_CTRL_CMD_ID17			0x17
#define UART_CTRL_CMD_ID18			0x18
#define UART_CTRL_CMD_ID19			0x19
#define UART_CTRL_CMD_ID1A			0x1A
#define UART_CTRL_CMD_ID1B			0x1B
#define UART_CTRL_CMD_ID1C			0x1C
#define UART_CTRL_CMD_ID1D			0x1D
#define UART_CTRL_CMD_ID1E			0x1E
#define UART_CTRL_CMD_ID1F			0x1F
//need response command, need return telemetry data
#define UART_CTRL_CMD_ID20			0x20  // huode zuo ban shi chang xin hao chu li yao ce
#define UART_CTRL_CMD_ID21			0x21  // huode zuo ban shi chang cheng xu shang zhu yao ce
#define UART_CTRL_CMD_ID22			0x22  // huode you ban shi chang xin hao chu li yao ce
#define UART_CTRL_CMD_ID23			0x23  // huode you ban shi chang cheng xu shang zhu yao ce
#define UART_CTRL_CMD_ID24			0x24  // huode zhi leng kong zhi qi A yao ce
#define UART_CTRL_CMD_ID25			0x25  // huode zhi leng kong zhi qi B zhi leng kong zhi yao ce
#define UART_CTRL_CMD_ID26			0x26  // huode zhi leng kong zhi qi B ji gou kong zhi yao ce
#define UART_CTRL_CMD_ID27			0x27
#define UART_CTRL_CMD_ID28			0x28
#define UART_CTRL_CMD_ID29			0x29
#define UART_CTRL_CMD_ID2A			0x2A
#define UART_CTRL_CMD_ID2B			0x2B
#define UART_CTRL_CMD_ID2C			0x2C
#define UART_CTRL_CMD_ID2D			0x2D
#define UART_CTRL_CMD_ID2E			0x2E
#define UART_CTRL_CMD_ID2F			0x2F
//running control command, need change PL register
#define UART_CTRL_CMD_ID30			0x30  // Telemetry switch control command
#define UART_CTRL_CMD_ID31			0x31  // Broadcast switch control command
#define UART_CTRL_CMD_ID32			0x32  // work mode change control command
#define UART_CTRL_CMD_ID33			0x33  // change CAN Number
#define UART_CTRL_CMD_ID34			0x34
#define UART_CTRL_CMD_ID35			0x35
#define UART_CTRL_CMD_ID36			0x36
#define UART_CTRL_CMD_ID37			0x37
#define UART_CTRL_CMD_ID38			0x38
#define UART_CTRL_CMD_ID39			0x39
#define UART_CTRL_CMD_ID3A			0x3A
#define UART_CTRL_CMD_ID3B			0x3B
#define UART_CTRL_CMD_ID3C			0x3C
#define UART_CTRL_CMD_ID3D			0x3D
#define UART_CTRL_CMD_ID3E			0x3E
#define UART_CTRL_CMD_ID3F			0x3F
//device send message to app
#define UART_CTRL_CMD_ID40			0x40  //
#define UART_CTRL_CMD_ID41			0x41  // set GPS time broadcast base time
#define UART_CTRL_CMD_ID42			0x42  // satellite ass data
#define UART_CTRL_CMD_ID43			0x43  // camera ass data
#define UART_CTRL_CMD_ID44			0x44  // send internal reset CAN bus broadcast
#define UART_CTRL_CMD_ID45			0x45  // send upload data broadcast
#define UART_CTRL_CMD_ID46			0x46
#define UART_CTRL_CMD_ID47			0x47
#define UART_CTRL_CMD_ID48			0x48
#define UART_CTRL_CMD_ID49			0x49
#define UART_CTRL_CMD_ID4A			0x4A
#define UART_CTRL_CMD_ID4B			0x4B
#define UART_CTRL_CMD_ID4C			0x4C
#define UART_CTRL_CMD_ID4D			0x4D
#define UART_CTRL_CMD_ID4E			0x4E
#define UART_CTRL_CMD_ID4F			0x4F

#define UART_CTRL_RESP_IDF0			0xF0  //
#define UART_CTRL_RESP_IDF1			0xF1  //
#define UART_CTRL_RESP_IDF2			0xF2  //
#define UART_CTRL_RESP_IDF3			0xF3  //
#define UART_CTRL_RESP_IDF4			0xF4  //
#define UART_CTRL_RESP_IDF5			0xF5  //
#define UART_CTRL_RESP_IDF6			0xF6  //
#define UART_CTRL_RESP_IDF7			0xF7  //
#define UART_CTRL_RESP_IDF8			0xF8  //
#define UART_CTRL_RESP_IDF9			0xF9  //
#define UART_CTRL_RESP_IDFA			0xFA  //
#define UART_CTRL_RESP_IDFB			0xFB  //
#define UART_CTRL_RESP_IDFC			0xFC  //
#define UART_CTRL_RESP_IDFD			0xFD  //
#define UART_CTRL_RESP_IDFE			0xFE  //
#define UART_CTRL_RESP_IDFF			0xFF  //


/***************************** END SERIAL COMMAND DEFINE *****************************/

#define MAX_BUFFER_SIZE				0x100

/******************************** CAN COMMAND DEFINE *********************************/

//broadcast mask define, bit0 for CAN select
#define UART_CMD_ERR_MASK			0x80

/****************************** END CAN COMMAND DEFINE *******************************/

/***************************** BROADCAST COMMAND DEFINE ******************************/
#define BROADCAST_BUFFER_LENGTH     0x200
#define MAX_BROADCAST_COUNT         8

#define BROADCAST_MUX_INIT          0x00

#define BROADCAST_MUX_ID_0          0x01
#define BROADCAST_MUX_ID_1          0x02
#define BROADCAST_MUX_ID_2          0x04
#define BROADCAST_MUX_ID_3          0x08
#define BROADCAST_MUX_ID_4          0x10
#define BROADCAST_MUX_ID_5          0x20
#define BROADCAST_MUX_ID_6          0x40
#define BROADCAST_MUX_ID_7          0x80

/*************************** END BROADCAST COMMAND DEFINE ****************************/
/***************************** BROADCAST COMMAND DEFINE ******************************/
#define TELEMETRY_BUFFER_LENGTH     0x200
#define MAX_TELEMETRY_COUNT		    8

#define TELEMETRY_MUX_INIT          0x00

#define TELEMETRY_MUX_ID_0          0x01
#define TELEMETRY_MUX_ID_1          0x02
#define TELEMETRY_MUX_ID_2          0x04
#define TELEMETRY_MUX_ID_3          0x08
#define TELEMETRY_MUX_ID_4          0x10
#define TELEMETRY_MUX_ID_5          0x20
#define TELEMETRY_MUX_ID_6          0x40
#define TELEMETRY_MUX_ID_7          0x80

/*************************** END BROADCAST COMMAND DEFINE ****************************/

#define CAN_PROTO_LENGTH            10

//typedef UART command structure
#define UART_BUF_LENGTH             12
typedef struct uart_cmd{
	uint8_t cmd;          // command number
	union{
		uint8_t devSelect;        // select protocol device
		uint8_t totalPackage;     // total package count
		uint8_t param1;           // set parameter 1
	};
	union{
		uint8_t vldPackageCnt;    // valid package count
		uint8_t index;            // package index
		uint8_t type;             // telemetry type
		uint8_t param2;           // set parameter 2
	};
	uint8_t validLen;             // valid data length
	uint8_t databuf[UART_BUF_LENGTH];          // data buffer
}S_UART_CMD;


typedef struct telemetry_counter{
	uint32_t header;
	uint32_t tail;
}S_TELE_COUNTER;

typedef uint8_t bool;
#define TRUE 1
#define FALSE 0

// broadcast mask
uint8_t  g8broadcastmask;
uint8_t  g8BroadCastBuf[MAX_BROADCAST_COUNT][BROADCAST_BUFFER_LENGTH];
uint32_t g32BroadcastLen[MAX_BROADCAST_COUNT];

// telemetry mask
uint8_t g8TelemetryMask;
uint8_t g8TelemetryData[MAX_TELEMETRY_COUNT][TELEMETRY_BUFFER_LENGTH];
uint32_t g32TelemetryDataLen[MAX_TELEMETRY_COUNT];

#define MAX_REMOTE_CONTROL_CNT 		8
uint8_t g8RemoteCtrlID[MAX_REMOTE_CONTROL_CNT];
uint8_t g8TelemetryReq[MAX_TELEMETRY_COUNT][0x10];
uint8_t g8TelemetryID[MAX_TELEMETRY_COUNT];
uint8_t g8TelemetryCnt[MAX_TELEMETRY_COUNT];
uint8_t g8TelemetryPeriod[MAX_TELEMETRY_COUNT];
uint8_t g8BroadcastPeriod[MAX_BROADCAST_COUNT];
// define active CAN number
uint8_t g8ActiveCANNum;

uint8_t g8PauseTimeFlag;
uint8_t gUpdateTimeFlag;


void CanCmdParse(uint8_t bufNo);
void uartCmdParse(uint8_t* databuf);

void globalVarInit(void);
void plConfig(void);
void userTimerIntrHandler();

/****************************** END CAN COMMAND DEFINE *******************************/
#endif /* SRC_USER_INTERFACE_H_ */
