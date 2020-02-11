/*
 * user_config.c
 *
 *  Created on: 2018锟斤拷10锟斤拷25锟斤拷
 *      Author: 508
 */

#include "user_interface.h"
static uint8_t g8activecannum = 0;
static uint8_t g8TelemetryType = 0xff;
uint8_t        g8UartCommandCnt = 0;
uint8_t        g8UartErrCommandCnt = 0;

static uint8_t getConfigIndex(uint8_t ch_num)
{
	for(uint8_t loop = 0; loop < MAX_DEV_NUM; loop++)
	{
		if(gDevConfigTable[loop].dev_id == ch_num)
		{
			return loop;
		}
	}

	return 0xff;
}
void uartsendmsg(uint8_t cmdNo, uint8_t* databuf, uint32_t len)
{
	S_UART_CMD resp;
	uint32_t m32loop = 0;
	uint32_t m32subloop = 0;
	uint8_t  m8resplen  = 0;

	memset(&resp, 0xff, sizeof(S_UART_CMD)); //initial response

	switch(cmdNo)
	{
	case 0xF0: // heartbeat resp
		resp.cmd = cmdNo;
		resp.validLen = len;
		for(m32subloop = 0; m32subloop < len; m32subloop++)
		{
			resp.databuf[m32subloop] = databuf[m32subloop];
		}
#ifdef DRIVER_ENABLE_UART16550
		ioDevWrite(DEV_NUM_UART16550_A, (uint8_t*)&resp, sizeof(S_UART_CMD));
		ioDevWrite(DEV_NUM_UART16550_B, (uint8_t*)&resp, sizeof(S_UART_CMD));
#endif
#ifdef DRIVER_ENABLE_UART
		ioDevWrite(DEV_NUM_UART_A, (uint8_t*)&resp, sizeof(S_UART_CMD));
		ioDevWrite(DEV_NUM_UART_B, (uint8_t*)&resp, sizeof(S_UART_CMD));
#endif
		delay_us(0x1000);
		return ;
	case 0xF1:
	case 0xF2:
		m8resplen = 0x0A;
		resp.devSelect = g8activecannum;
		break;
	case 0xF3:
	case 0xF4:
		m8resplen = 0x04;
		resp.devSelect = g8activecannum;
		break;
	default:
		m8resplen = 0x0C;
		break;
	}

	for(m32loop = 0; m32loop < (len / m8resplen); m32loop++)
	{
		resp.cmd = cmdNo;
		if(cmdNo == 0xf1 || cmdNo == 0xf3)
		{
			resp.index = 0;
			resp.totalPackage = 1;
		}
		else if(cmdNo == 0xf2 || cmdNo == 0xf4)
		{
			resp.totalPackage = (len / m8resplen);
			resp.type = g8TelemetryType;
		}

		resp.validLen = m8resplen;

		for(m32subloop = 0; m32subloop < m8resplen; m32subloop++)
		{
			resp.databuf[m32subloop] = databuf[m32loop * m8resplen + m32subloop];
		}
#ifdef DRIVER_ENABLE_UART16550
		ioDevWrite(DEV_NUM_UART16550_A, (uint8_t*)&resp, sizeof(S_UART_CMD));
		ioDevWrite(DEV_NUM_UART16550_B, (uint8_t*)&resp, sizeof(S_UART_CMD));
#endif
#ifdef DRIVER_ENABLE_UART
		ioDevWrite(DEV_NUM_UART_A, (uint8_t*)&resp, sizeof(S_UART_CMD));
		ioDevWrite(DEV_NUM_UART_B, (uint8_t*)&resp, sizeof(S_UART_CMD));
#endif
		delay_us(0x1000);
	}

	if(m32loop * m8resplen != len)
	{
		memset(&resp, 0xff, sizeof(S_UART_CMD)); //initial response

		resp.cmd = cmdNo;
		resp.devSelect = g8activecannum;
		resp.validLen = (len - m32loop * m8resplen);

		if(cmdNo == 0xf1 || cmdNo == 0xf3)
		{
			resp.index = 0;
		}
		else if(cmdNo == 0xf2 || cmdNo == 0xf4)
		{
			resp.totalPackage = (len / m8resplen);
		}

		for(m32subloop = 0; m32subloop < (len - m32loop * m8resplen); m32subloop++)
		{
			resp.databuf[m32subloop] = databuf[m32loop * m8resplen + m32subloop];
		}

#ifdef DRIVER_ENABLE_UART16550
		ioDevWrite(DEV_NUM_UART16550_A, (uint8_t*)&resp, sizeof(S_UART_CMD));
		ioDevWrite(DEV_NUM_UART16550_B, (uint8_t*)&resp, sizeof(S_UART_CMD));
#endif
#ifdef DRIVER_ENABLE_UART
		ioDevWrite(DEV_NUM_UART_A, (uint8_t*)&resp, sizeof(S_UART_CMD));
		ioDevWrite(DEV_NUM_UART_B, (uint8_t*)&resp, sizeof(S_UART_CMD));
#endif
		delay_us(0x1000);
	}
}

//static void TelemetryDataCtrl(uint8_t* m8databuf, uint8_t index)
//{
//	memcpy(&g8TelemetryData[index][g32TelemetryDataLen[index]], m8databuf, CAN_PROTO_LENGTH);
//	g32TelemetryDataLen[index] += CAN_PROTO_LENGTH;
//	if(m8databuf[2] == (g8TelemetryCnt[index] - 1))
//	{
//		g8TelemetryType = index;
//		uartsendmsg(UART_CTRL_RESP_IDF2, g8TelemetryData[index], g32TelemetryDataLen[index]);
//		g32TelemetryDataLen[index] = 0;
//	}
//}

static void CmdRespSend(void)
{
	uint8_t databuf[8] = {0};

	databuf[0] = g8TelemetryMask;
	databuf[1] = g8broadcastmask;
	databuf[2] = g8ActiveCANNum;

	databuf[6] = g8UartCommandCnt;
	databuf[7] = g8UartErrCommandCnt;

	uartsendmsg(UART_CTRL_RESP_IDF0, databuf, 8);
}

/*
 * Function : cmdParse
 * 		parse response
 * Param    :
 * 		bufNo : assign the data buffer index
  *
 * Return   :
 * 		None
 *
 * */
void CanCmdParse(uint8_t bufNo)
{
	uint8_t m8datalen  = 0;
	uint8_t m8databuf[0x10] = {0};

	//get data len
	m8datalen = revDatafromBuf(bufNo, &m8databuf[0], CAN_PROTO_LENGTH); // get RTR, DLC

	if(m8datalen != CAN_PROTO_LENGTH && m8datalen > 0)
	{
		//DEV_CMD_MSG("%s : %d get invalid len %d from CAN buffer\r\n",__func__, __LINE__, m8datalen);
	}
	else
	{
		if(m8databuf[0] == g8RemoteCtrlID[0] || m8databuf[0] == g8RemoteCtrlID[1] ||
		   m8databuf[0] == g8RemoteCtrlID[2] || m8databuf[0] == g8RemoteCtrlID[3] ||
		   m8databuf[0] == g8RemoteCtrlID[4] || m8databuf[0] == g8RemoteCtrlID[5] ||
		   m8databuf[0] == g8RemoteCtrlID[6] || m8databuf[0] == g8RemoteCtrlID[7]   )  // remote control ACK ID
		{
			uartsendmsg(UART_CTRL_RESP_IDF1, m8databuf, (m8databuf[1] & 0x0f) + 2);
		}
		else if(m8databuf[0] == g8TelemetryID[0] || m8databuf[0] == g8TelemetryID[1] ||
				m8databuf[0] == g8TelemetryID[2] || m8databuf[0] == g8TelemetryID[3] ||
				m8databuf[0] == g8TelemetryID[4] || m8databuf[0] == g8TelemetryID[5] ||
				m8databuf[0] == g8TelemetryID[6] || m8databuf[0] == g8TelemetryID[7]) // control telemetry data
		{
			uartsendmsg(UART_CTRL_RESP_IDF2, m8databuf, (m8databuf[1] & 0x0f) + 2);
		}
		else  //invalid CAN ID
		{
			uartsendmsg(UART_CTRL_RESP_IDFF, m8databuf, (m8databuf[1] & 0x0f) + 2);
		}
	}

	return ;
}

static void Uart1XCmdHandle(S_UART_CMD* uartcmd)
{
	switch(uartcmd->cmd)
	{
	case UART_CTRL_CMD_ID10 :
		switch(uartcmd->devSelect)
		{
		case 0:
			ioDevWrite(DEV_NUM_CAN_A, uartcmd->databuf, uartcmd->validLen);
			break;
		case 1:
			ioDevWrite(DEV_NUM_CAN_B, uartcmd->databuf, uartcmd->validLen);
			break;
		case 2:
			break;
		default:
			g8UartErrCommandCnt++;
			return ;
		}
		g8UartCommandCnt++;
		break;
	case UART_CTRL_CMD_ID11 :
		break;
	case UART_CTRL_CMD_ID12 :
		break;
	case UART_CTRL_CMD_ID13 :
		break;
	case UART_CTRL_CMD_ID14 :
		break;
	case UART_CTRL_CMD_ID15 :
		break;
	case UART_CTRL_CMD_ID16 :
		break;
	case UART_CTRL_CMD_ID17 :
		break;
	case UART_CTRL_CMD_ID18 :
		break;
	case UART_CTRL_CMD_ID1A :
		break;
	case UART_CTRL_CMD_ID1B :
		break;
	case UART_CTRL_CMD_ID1C :
		break;
	case UART_CTRL_CMD_ID1D :
		break;
	case UART_CTRL_CMD_ID1E :
		break;
	case UART_CTRL_CMD_ID1F :
		break;
	default:
		break;
	}
}

static void Uart2XCmdHandle(S_UART_CMD* uartcmd)
{
	switch(uartcmd->cmd)
	{
	case UART_CTRL_CMD_ID20 :
	case UART_CTRL_CMD_ID21 :
	case UART_CTRL_CMD_ID22 :
	case UART_CTRL_CMD_ID23 :
	case UART_CTRL_CMD_ID24 :
	case UART_CTRL_CMD_ID25 :
	case UART_CTRL_CMD_ID26 :
	case UART_CTRL_CMD_ID27 :
		if(uartcmd->index == 0)
		{
			g8TelemetryCnt[uartcmd->cmd - UART_CTRL_CMD_ID20] = 0;
		}
		memcpy(&g8TelemetryReq[uartcmd->cmd - UART_CTRL_CMD_ID20][g8TelemetryCnt[uartcmd->cmd - UART_CTRL_CMD_ID20]],
				uartcmd->databuf, uartcmd->validLen);
		g8TelemetryCnt[uartcmd->cmd - UART_CTRL_CMD_ID20] += uartcmd->validLen;
		break;
	case UART_CTRL_CMD_ID28 :
	case UART_CTRL_CMD_ID29 :
	case UART_CTRL_CMD_ID2A :
	case UART_CTRL_CMD_ID2B :
	case UART_CTRL_CMD_ID2C :
	case UART_CTRL_CMD_ID2D :
	case UART_CTRL_CMD_ID2E :
	case UART_CTRL_CMD_ID2F :
		if(uartcmd->index == 0)
		{
			g32BroadcastLen[uartcmd->cmd - UART_CTRL_CMD_ID28] = 0;
		}
		memcpy(&g8BroadCastBuf[uartcmd->cmd - UART_CTRL_CMD_ID28][g32BroadcastLen[uartcmd->cmd - UART_CTRL_CMD_ID28]],
				uartcmd->databuf, uartcmd->validLen);
		g32BroadcastLen[uartcmd->cmd - UART_CTRL_CMD_ID28] += uartcmd->validLen;
		break;
	default:
		break;
	}
}

int device_reset(bool changeSource, char sourceType)
{

	Xil_Out32(0xb0000,0xfc00); //(1)复位入口数据： mwr 0xb0000 0xfc00  ----
	if(changeSource)
		Xil_Out32(0x80044,sourceType); //（2）数据源切换-----
	delay_us(50000);			//(3)延时----50ms  保证发送上位机帧结构完整
	Xil_Out32(0x80004,1); //(4) mwr 0x8_0004 1		ddr datamover的复位 高复位
	Xil_Out32(0x80004,0); //(5)mwr 0x8_0004 0		ddr datamover的复位 清复位
	Xil_Out32(0x80010,0x1); //(6)mwr 0x80010   0x1    计算器清楚
	Xil_Out32(0x80010,0x0); //(7)mwr 0x80010   0x0    计算器清楚
	Xil_Out32(0xb0000,0xfcff); //(8)放开入口数据： mwr 0xb0000 0xfcff
	return 0;
}

static void Uart3XCmdHandle(S_UART_CMD* uartcmd)
{
	switch(uartcmd->cmd)
	{
	case UART_CTRL_CMD_ID30 :// telemetry mux control
		for(uint8_t m8loop = 0; m8loop < uartcmd->validLen - 1; m8loop++)
		{
			if(uartcmd->databuf[m8loop + 1] & 0x0f)
			{
				g8TelemetryMask |= (1 << m8loop);
				g8TelemetryPeriod[m8loop] = ((uartcmd->databuf[m8loop + 1] >> 4) & 0x0f);
			}
			else
			{
				g8TelemetryMask &= ~(1 << m8loop);
			}
		}
		if(uartcmd->databuf[0])
		{
			uartcmd->databuf[0] = SEC_PULSE_CMD_START;
			//ioDevCtrl(DEV_NUM_SEC_PULSE_PPS, &uartcmd->databuf[0]);
		}
		else
		{
			uartcmd->databuf[0] = SEC_PULSE_CMD_STOP;
			//ioDevCtrl(DEV_NUM_SEC_PULSE_PPS, &uartcmd->databuf[0]);
		}
		break;
	case UART_CTRL_CMD_ID31 : // broadcast mux control
		for(uint8_t m8loop = 0; m8loop < (uartcmd->validLen); m8loop++)
		{
			if(uartcmd->databuf[m8loop] & 0x0f)
			{
				g8broadcastmask |= (1 << m8loop);
				g8BroadcastPeriod[m8loop] = ((uartcmd->databuf[m8loop] >> 4) & 0x0f);
			}
			else
			{
				g8broadcastmask &= ~(1 << m8loop);
			}
		}
		break;
	case UART_CTRL_CMD_ID32 :
		device_reset(FALSE,0);
		break;
	case UART_CTRL_CMD_ID33 : // active can switch
		if(uartcmd->param1 == 0xff && uartcmd->param2 == 0xff && uartcmd->validLen == 0x01)
		{
			g8ActiveCANNum = uartcmd->databuf[0];
		}
		break;
	case UART_CTRL_CMD_ID34 :
		device_reset(TRUE,uartcmd->databuf[0]);
		break;
	case UART_CTRL_CMD_ID35 :
		break;
	case UART_CTRL_CMD_ID36 :
		break;
	case UART_CTRL_CMD_ID37 :
		break;
	case UART_CTRL_CMD_ID38 :
		break;
	case UART_CTRL_CMD_ID39 :
		break;
	case UART_CTRL_CMD_ID3A :
		break;
	case UART_CTRL_CMD_ID3B :
		break;
	case UART_CTRL_CMD_ID3C :
		break;
	case UART_CTRL_CMD_ID3D :
		break;
	case UART_CTRL_CMD_ID3E :
		break;
	case UART_CTRL_CMD_ID3F :
	default:
		break;
	}
}

static void Uart4XCmdHandle(S_UART_CMD* uartcmd)
{
	switch(uartcmd->cmd)
	{
		case UART_CTRL_CMD_ID40 :
			if(uartcmd->param1 == 0xff && uartcmd->param2 == 0xff && uartcmd->validLen == 0x08)
			{
				uint32_t m32addr = 0;
				m32addr = m32addr << 8 | uartcmd->databuf[0];
				m32addr = m32addr << 8 | uartcmd->databuf[1];
				m32addr = m32addr << 8 | uartcmd->databuf[2];
				m32addr = m32addr << 8 | uartcmd->databuf[3];
				uint32_t m32val = 0;
				m32val = m32val << 8 | uartcmd->databuf[4];
				m32val = m32val << 8 | uartcmd->databuf[5];
				m32val = m32val << 8 | uartcmd->databuf[6];
				m32val = m32val << 8 | uartcmd->databuf[7];

				writel(m32addr, m32val);
			}
			break;
		case UART_CTRL_CMD_ID41 :
			if(uartcmd->param1 == 0xff && uartcmd->param2 == 0xff && uartcmd->validLen == 0x04)
			{
				uint32_t m32addr = 0;
				m32addr = m32addr << 8 | uartcmd->databuf[0];
				m32addr = m32addr << 8 | uartcmd->databuf[1];
				m32addr = m32addr << 8 | uartcmd->databuf[2];
				m32addr = m32addr << 8 | uartcmd->databuf[3];
				uint32_t m32val = 0;
				m32val = readl(m32addr);
				uartcmd->databuf[0] = (m32val >> 24) & 0xff;
				uartcmd->databuf[1] = (m32val >> 16) & 0xff;
				uartcmd->databuf[2] = (m32val >>  8) & 0xff;
				uartcmd->databuf[3] = (m32val      ) & 0xff;

				uartsendmsg(UART_CTRL_RESP_IDF5, uartcmd->databuf, 4);
			}
			break;
		case UART_CTRL_CMD_ID42 :
			if(uartcmd->param1 == 0x00)
			{
				switch(uartcmd->param2)
				{
				case 0x00: //
					for(uint8_t m8loop = 0; m8loop < uartcmd->validLen; m8loop++)
					{
						g8RemoteCtrlID[m8loop] = uartcmd->databuf[m8loop];
					}
					break;
				case 0x01:
					for(uint8_t m8loop = 0; m8loop < uartcmd->validLen; m8loop++)
					{
						g8TelemetryID[m8loop] = uartcmd->databuf[m8loop];
					}
					break;
				case 0x02:
					for(uint8_t m8loop = 0; m8loop < uartcmd->validLen; m8loop++)
					{
						g8TelemetryCnt[m8loop] = uartcmd->databuf[m8loop];
					}
					break;
				default:
					break;
				}
			}
			break;
		case UART_CTRL_CMD_ID43 :
			break;
		case UART_CTRL_CMD_ID44 :
		case UART_CTRL_CMD_ID45 :
		case UART_CTRL_CMD_ID46 :
		case UART_CTRL_CMD_ID47 :
		case UART_CTRL_CMD_ID48 :
		case UART_CTRL_CMD_ID49 :
		case UART_CTRL_CMD_ID4A :
		case UART_CTRL_CMD_ID4B :
		case UART_CTRL_CMD_ID4C :
		case UART_CTRL_CMD_ID4D :
		case UART_CTRL_CMD_ID4E :
		case UART_CTRL_CMD_ID4F :
		default:
			break;
	}
}

/*
 * Function : uartCmdParse
 * 		parse CAN Command
 * Param    :
 * 		cmdNo : UART command index(from APP)
 *
 * Return   :
 * 		None
 *
 * */
/*
 * databuf[0] : command index
 * databuf[1] : for can number select
 * databuf[2] : valid data length
 * databuf[15 : 3] : valid data
 * */
void uartCmdParse(uint8_t* databuf)
{
	S_UART_CMD* uartcmd = (S_UART_CMD*)databuf;

	switch(uartcmd->cmd)
	{
		case 	 UART_CMD_RESET_BUF : // reset buffer attr status
			if(uartcmd->param1 == 0x00 && uartcmd->param2 == 0x00 && uartcmd->validLen == 0x00)
			{
				bufattr_init();
#ifdef DRIVER_ENABLE_UART16550
				g8Uart16550RecvCntTmpA = 0;
				g8Uart16550RecvCntTmpB = 0;
#endif
#ifdef DRIVER_ENABLE_UART
				g8UartRecvCntTmpA = 0;
				g8UartRecvCntTmpB = 0;
#endif
			}
//			global_var_init();
			break;
		case UART_CTRL_CMD_SYNC:
			if(uartcmd->param1 == 0x01 && uartcmd->param2 == 0x01 && uartcmd->validLen == 0x01)
			{
				CmdRespSend();
			}
			break;
		case     UART_CTRL_CMD_ID10 :
		case     UART_CTRL_CMD_ID11 :
		case     UART_CTRL_CMD_ID12 :
		case     UART_CTRL_CMD_ID13 :
		case     UART_CTRL_CMD_ID14 :
		case     UART_CTRL_CMD_ID15 :
		case     UART_CTRL_CMD_ID16 :
		case     UART_CTRL_CMD_ID17 :
		case     UART_CTRL_CMD_ID18 :
		case     UART_CTRL_CMD_ID1A :
		case     UART_CTRL_CMD_ID1B :
		case     UART_CTRL_CMD_ID1C :
		case     UART_CTRL_CMD_ID1D :
		case     UART_CTRL_CMD_ID1E :
		case     UART_CTRL_CMD_ID1F :
			Uart1XCmdHandle(uartcmd);
			break;
		case     UART_CTRL_CMD_ID20 :
		case     UART_CTRL_CMD_ID21 :
		case     UART_CTRL_CMD_ID22 :
		case     UART_CTRL_CMD_ID23 :
		case     UART_CTRL_CMD_ID24 :
		case     UART_CTRL_CMD_ID25 :
		case     UART_CTRL_CMD_ID26 :
		case     UART_CTRL_CMD_ID27 :
		case     UART_CTRL_CMD_ID28 :
		case     UART_CTRL_CMD_ID29 :
		case     UART_CTRL_CMD_ID2A :
		case     UART_CTRL_CMD_ID2B :
		case     UART_CTRL_CMD_ID2C :
		case     UART_CTRL_CMD_ID2D :
		case     UART_CTRL_CMD_ID2E :
		case     UART_CTRL_CMD_ID2F :
			Uart2XCmdHandle(uartcmd);
			break;
		case     UART_CTRL_CMD_ID30 :
		case     UART_CTRL_CMD_ID31 :
		case     UART_CTRL_CMD_ID32 :
		case     UART_CTRL_CMD_ID33 :
		case     UART_CTRL_CMD_ID34 :
		case     UART_CTRL_CMD_ID35 :
		case     UART_CTRL_CMD_ID36 :
		case     UART_CTRL_CMD_ID37 :
		case     UART_CTRL_CMD_ID38 :
		case     UART_CTRL_CMD_ID39 :
		case     UART_CTRL_CMD_ID3A :
		case     UART_CTRL_CMD_ID3B :
		case     UART_CTRL_CMD_ID3C :
		case     UART_CTRL_CMD_ID3D :
		case     UART_CTRL_CMD_ID3E :
		case     UART_CTRL_CMD_ID3F :
			Uart3XCmdHandle(uartcmd);
			break;
		case     UART_CTRL_CMD_ID40 :
		case     UART_CTRL_CMD_ID41 :
		case     UART_CTRL_CMD_ID42 :
		case     UART_CTRL_CMD_ID43 :
		case     UART_CTRL_CMD_ID44 :
		case     UART_CTRL_CMD_ID45 :
		case     UART_CTRL_CMD_ID46 :
		case     UART_CTRL_CMD_ID47 :
		case     UART_CTRL_CMD_ID48 :
		case     UART_CTRL_CMD_ID49 :
		case     UART_CTRL_CMD_ID4A :
		case     UART_CTRL_CMD_ID4B :
		case     UART_CTRL_CMD_ID4C :
		case     UART_CTRL_CMD_ID4D :
		case     UART_CTRL_CMD_ID4E :
		case     UART_CTRL_CMD_ID4F : 
			Uart4XCmdHandle(uartcmd);
			break;
		default:
			g8UartErrCommandCnt++;
			break;
	}

	return ;
}

void globalVarInit(void)
{
	for(int loop = 0; loop < MAX_TELEMETRY_COUNT; loop++)
	{
		memset(g8TelemetryData[loop], 0, TELEMETRY_BUFFER_LENGTH);
		memset(g8TelemetryReq[loop], 0, 0x10);
		g8TelemetryID[loop] = 0xff;
		g8TelemetryCnt[loop] = 0;
		g8TelemetryPeriod[loop] = 0;
	}
	for(int loop = 0; loop < MAX_BROADCAST_COUNT; loop++)
	{
		memset(g8BroadCastBuf[loop], 0, BROADCAST_BUFFER_LENGTH);
		g8BroadcastPeriod[loop] = 0;
	}
	for(int loop = 0; loop < MAX_REMOTE_CONTROL_CNT; loop++)
	{
		g8RemoteCtrlID[loop] = 0xff;
	}
}

static void send_telemetry()
{
	static uint8_t m8TimerCnt = 0;

	if((g8TelemetryMask & TELEMETRY_MUX_ID_0) == TELEMETRY_MUX_ID_0)
	{
		if(m8TimerCnt % g8TelemetryPeriod[0] == 0)
		{
			if(g8TelemetryReq[0][1] != 0)
			{
				ioDevWrite(g8ActiveCANNum, g8TelemetryReq[0], (g8TelemetryReq[0][1] & 0x0f) + 2);
			}
		}
	}
	if((g8TelemetryMask & TELEMETRY_MUX_ID_1) == TELEMETRY_MUX_ID_1)
	{
		if(m8TimerCnt % g8TelemetryPeriod[1] == 0)
		{
			if(g8TelemetryReq[1][1] != 0)
			{
				ioDevWrite(g8ActiveCANNum, g8TelemetryReq[1], (g8TelemetryReq[1][1] & 0x0f) + 2);
			}
		}
	}
	if((g8TelemetryMask & TELEMETRY_MUX_ID_2) == TELEMETRY_MUX_ID_2)
	{
		if(m8TimerCnt % g8TelemetryPeriod[2] == 0)
		{
			if(g8TelemetryReq[2][1] != 0)
			{
				ioDevWrite(g8ActiveCANNum, g8TelemetryReq[2], (g8TelemetryReq[2][1] & 0x0f) + 2);
			}
		}
	}
	if((g8TelemetryMask & TELEMETRY_MUX_ID_3) == TELEMETRY_MUX_ID_3)
	{
		if(m8TimerCnt % g8TelemetryPeriod[3] == 0)
		{
			if(g8TelemetryReq[3][1] != 0)
			{
				ioDevWrite(g8ActiveCANNum, g8TelemetryReq[3], (g8TelemetryReq[3][1] & 0x0f) + 2);
			}
		}
	}
	if((g8TelemetryMask & TELEMETRY_MUX_ID_4) == TELEMETRY_MUX_ID_4)
	{
		if(m8TimerCnt % g8TelemetryPeriod[4] == 0)
		{
			if(g8TelemetryReq[4][1] != 0)
			{
				ioDevWrite(g8ActiveCANNum, g8TelemetryReq[4], (g8TelemetryReq[4][1] & 0x0f) + 2);
			}
		}
	}
	if((g8TelemetryMask & TELEMETRY_MUX_ID_5) == TELEMETRY_MUX_ID_5)
	{
		if(m8TimerCnt % g8TelemetryPeriod[5] == 0)
		{
			if(g8TelemetryReq[5][1] != 0)
			{
				ioDevWrite(g8ActiveCANNum, g8TelemetryReq[5], (g8TelemetryReq[5][1] & 0x0f) + 2);
			}
		}
	}
	if((g8TelemetryMask & TELEMETRY_MUX_ID_6) == TELEMETRY_MUX_ID_6)
	{
		if(m8TimerCnt % g8TelemetryPeriod[6] == 0)
		{
			if(g8TelemetryReq[6][1] != 0)
			{
				ioDevWrite(g8ActiveCANNum, g8TelemetryReq[6], (g8TelemetryReq[6][1] & 0x0f) + 2);
			}
		}
	}
	if((g8TelemetryMask & TELEMETRY_MUX_ID_7) == TELEMETRY_MUX_ID_7)
	{
		if(m8TimerCnt % g8TelemetryPeriod[7] == 0)
		{
			if(g8TelemetryReq[7][1] != 0)
			{
				ioDevWrite(g8ActiveCANNum, g8TelemetryReq[7], (g8TelemetryReq[7][1] & 0x0f) + 2);
			}
		}
	}
	m8TimerCnt++;
}

static void send_broadcast_time(uint32_t time)
{
	static uint8_t canNo = 0;
	static uint8_t m8TimerCnt = 0;
	if((g8broadcastmask & BROADCAST_MUX_ID_0) == BROADCAST_MUX_ID_0)
	{
		if(m8TimerCnt % g8BroadcastPeriod[0] == 0)
		{
			if(g32BroadcastLen[0] != 0)
			{
				ioDevWrite(g8ActiveCANNum, g8BroadCastBuf[0], g32BroadcastLen[0]);
			}
		}

		canNo %= 2;
	}
	if((g8broadcastmask & BROADCAST_MUX_ID_1) == BROADCAST_MUX_ID_1)
	{
		if(m8TimerCnt % g8BroadcastPeriod[1] == 0)
		{
			if(g32BroadcastLen[1] != 0)
				ioDevWrite(g8ActiveCANNum, g8BroadCastBuf[1], g32BroadcastLen[1]);
		}
	}
	if((g8broadcastmask & BROADCAST_MUX_ID_2) == BROADCAST_MUX_ID_2)
	{
		if(m8TimerCnt % g8BroadcastPeriod[2] == 0)
		{
			if(g32BroadcastLen[2] != 0)
				ioDevWrite(g8ActiveCANNum, g8BroadCastBuf[2], g32BroadcastLen[2]);
		}
	}
	if((g8broadcastmask & BROADCAST_MUX_ID_3) == BROADCAST_MUX_ID_3)
	{
		if(m8TimerCnt % g8BroadcastPeriod[3] == 0)
		{
			if(g32BroadcastLen[3] != 0)
				ioDevWrite(g8ActiveCANNum, g8BroadCastBuf[3], g32BroadcastLen[3]);
		}
	}
	if((g8broadcastmask & BROADCAST_MUX_ID_4) == BROADCAST_MUX_ID_4)
	{
		if(m8TimerCnt % g8BroadcastPeriod[4] == 0)
		{
			if(g32BroadcastLen[4] != 0)
				ioDevWrite(g8ActiveCANNum, g8BroadCastBuf[4], g32BroadcastLen[4]);
		}
	}
	if((g8broadcastmask & BROADCAST_MUX_ID_5) == BROADCAST_MUX_ID_5)
	{
		if(m8TimerCnt % g8BroadcastPeriod[5] == 0)
		{
			if(g32BroadcastLen[5] != 0)
				ioDevWrite(g8ActiveCANNum, g8BroadCastBuf[5], g32BroadcastLen[5]);
		}
	}
	if((g8broadcastmask & BROADCAST_MUX_ID_6) == BROADCAST_MUX_ID_6)
	{
		if(m8TimerCnt % g8BroadcastPeriod[6] == 0)
		{
			if(g32BroadcastLen[6] != 0)
				ioDevWrite(g8ActiveCANNum, g8BroadCastBuf[6], g32BroadcastLen[6]);
		}
	}
	if((g8broadcastmask & BROADCAST_MUX_ID_7) == BROADCAST_MUX_ID_7)
	{
		if(m8TimerCnt % g8BroadcastPeriod[7] == 0)
		{
			if(g32BroadcastLen[7] != 0)
				ioDevWrite(g8ActiveCANNum, g8BroadCastBuf[7], g32BroadcastLen[7]);
		}
	}
	m8TimerCnt++;
}

void userTimerIntrHandler()
{
#ifdef DRIVER_ENABLE_SECOND_PULSE
	static uint8_t m8Cnt = 0;
	if(gUpdateTimeFlag)
	{
		g32SecondCnt = g32SecondCntTmp;
		gUpdateTimeFlag = 0;
	}
	else
	{
		if(!g8PauseTimeFlag)
		{
			m8Cnt++;
			if(m8Cnt % 4 == 0)
			{
				g32SecondCnt++;
			}
		}
	}

	send_broadcast_time(g32SecondCnt);
	send_telemetry();
#endif // DRIVER_ENABLE_SECOND_PULSE
}

#ifdef DRIVER_ENABLE_DMA_CORE
void config_dma_core(uint8_t chan_num, uint32_t startaddr, uint32_t hsize_wr, uint32_t vsize_wr, uint32_t hsize_rd, uint32_t vsize_rd, uint8_t frame_num, uint8_t wrcmd_mode, uint32_t vsize_jump)
{
	uint8_t ch_num = getConfigIndex(chan_num);

	if(ch_num != 0xff)
	{
		writel(gDevConfigTable[ch_num].base_addr + DMA_CORE_REG_OFF_START_ADDR, startaddr);
		writel(gDevConfigTable[ch_num].base_addr + DMA_CORE_REG_OFF_HSIZE_WR, hsize_wr);
		writel(gDevConfigTable[ch_num].base_addr + DMA_CORE_REG_OFF_VSIZE_WR, vsize_wr);
		writel(gDevConfigTable[ch_num].base_addr + DMA_CORE_REG_OFF_HSIZE_RD, hsize_rd);
		writel(gDevConfigTable[ch_num].base_addr + DMA_CORE_REG_OFF_VSIZE_RD, vsize_rd);
		writel(gDevConfigTable[ch_num].base_addr + DMA_CORE_REG_OFF_FRAME_NUM, frame_num);
		writel(gDevConfigTable[ch_num].base_addr + DMA_CORE_REG_OFF_WRCMD_MODE, wrcmd_mode);
		writel(gDevConfigTable[ch_num].base_addr + DMA_CORE_REG_OFF_VSIZE_JUMP, vsize_jump);
	}
}
#endif // DRIVER_ENABLE_DMA_CORE

static void dataOutCamerlinkConfig(uint32_t linecnt)
{
#if 0
	uint8_t ch_num = getConfigIndex(DEV_NUM_DO_CAMERLINK);

	if(ch_num != 0xff)
	{
	writel(gDevConfigTable[ch_num].base_addr + 0x00, linecnt);
	writel(gDevConfigTable[ch_num].base_addr + 0x04, 10);

	writel(gDevConfigTable[ch_num].base_addr + 0x18, linecnt);
	}
#endif
}

static void dataOutConfig(void)
{
#ifdef DRIVER_ENABLE_CAMERLINK_OUT
	dataOutCamerlinkConfig(3371);
#endif // DRIVER_ENABLE_CAMERLINK_OUT

#ifdef DRIVER_ENABLE_AURORA_OUT

#endif // DRIVER_ENABLE_AURORA_OUT
}

static void  dmaCoreConfig(void)
{
#ifdef DRIVER_ENABLE_DMA_CORE
	config_dma_core(DEV_NUM_DMA_CH0, 0x80000000, 1584, 3371, 1584,  3371, 4, 0,  64);
//	config_dma_core(DEV_NUM_DMA_CH1, 0x88000000, 1024, 64, 1024, 128, 16, 1, 128);
//	config_dma_core(DEV_NUM_DMA_CH2, 0x90000000, 1024, 64, 1024, 64, 16, 2, 64);
//	config_dma_core(DEV_NUM_DMA_CH3, 0x98000000, 1024, 64, 1024, 64, 16, 0, 64);
//	config_dma_core(DEV_NUM_DMA_CH4, 0xa0000000, 1024, 64, 1024, 64, 16, 0, 64);
//	config_dma_core(DEV_NUM_DMA_CH5, 0xa8000000, 1024, 64, 1024, 64, 16, 0, 64);
//	config_dma_core(DEV_NUM_DMA_CH6, 0xb0000000, 1024, 64, 1024, 64, 16, 0, 64);
//	config_dma_core(DEV_NUM_DMA_CH7, 0xb8000000, 1024, 64, 1024, 64, 16, 0, 64);
#endif // DRIVER_ENABLE_DMA_CORE
}

static void gtx2711DataInConfig(uint8_t chan_num, uint8_t mode, uint32_t period)
{
#ifdef DRIVER_ENABLE_2711_GTX_IN
//	DEV_NUM_DI_2711_GTX_CH0
	uint8_t ch_num = getConfigIndex(chan_num);

	if(ch_num != 0xff)
	{
		writel(gDevConfigTable[ch_num].base_addr + 0x00, period); // period
		writel(gDevConfigTable[ch_num].base_addr + 0x04, mode);
	}
#endif // DRIVER_ENABLE_2711_GTX_IN
}

static void gtx2711DataInChanEn(uint8_t chan)
{
#if 0
	uint8_t ch_num = getConfigIndex(DEV_NUM_DI_2711_GTX_RST);

	if(ch_num != 0xff)
	{
		writel(gDevConfigTable[ch_num].base_addr + chan * 0x04, 1); // period
	}
#endif
}

static void gtx2711DataInGTXInit(void)
{
#if 0
	uint8_t ch_num = getConfigIndex(DEV_NUM_DI_2711_GTX_CFG);

	if(ch_num != 0xff)
	{
		writel(gDevConfigTable[ch_num].base_addr + 13 * 0x04, 0x1000); // timeout cnt
		writel(gDevConfigTable[ch_num].base_addr + 16 * 0x04, 0x0); // rx raw
		writel(gDevConfigTable[ch_num].base_addr + 17 * 0x04, 0); // loopback normal mode
//		writel(gDevConfigTable[ch_num].base_addr + 18 * 0x04, 0); // send en

		writel(gDevConfigTable[ch_num].base_addr + 14 * 0x04, 1); // release tx gth reset
		writel(gDevConfigTable[ch_num].base_addr + 15 * 0x04, 1); // release rx gth reset
	}
#endif
}
static void dataInConfig(void)
{
#ifdef  DRIVER_ENABLE_LVDS_IN

#endif // DRIVER_ENABLE_LVDS_IN

#ifdef DRIVER_ENABLE_2711_GTX_IN

	gtx2711DataInGTXInit();

	gtx2711DataInConfig(DEV_NUM_DI_2711_GTX_CH0, 0, 1582);
//	gtx2711DataInConfig(DEV_NUM_DI_2711_GTX_CH1, 0, 1582);
//	gtx2711DataInConfig(DEV_NUM_DI_2711_GTX_CH2, 0, 1582);
//	gtx2711DataInConfig(DEV_NUM_DI_2711_GTX_CH3, 0, 1582);
//	gtx2711DataInConfig(DEV_NUM_DI_2711_GTX_CH4, 0, 1582);
//	gtx2711DataInConfig(DEV_NUM_DI_2711_GTX_CH5, 0, 1582);
//	gtx2711DataInConfig(DEV_NUM_DI_2711_GTX_CH6, 0, 1582);
//	gtx2711DataInConfig(DEV_NUM_DI_2711_GTX_CH7, 0, 1582);

	gtx2711DataInChanEn(0);

#endif // DRIVER_ENABLE_2711_GTX_IN

#ifdef DRIVER_ENABLE_2711_IN

#endif // DRIVER_ENABLE_2711_IN
}

static void simSourceInit(void)
{
	configChan0();

	startfsync(0xaaaa, 30);
}

void plConfig(void)
{
#if 0
	simSourceInit();

	/* initial DO module
	 * initial data out module first */
	dataOutConfig();

	/* initial DMA core module
	 * before release data in module, DMA core must configure done*/
	dmaCoreConfig();

	/* !after DMA and DO module ready, release DI module reset
	 *
	 * initial DI module
	 * */
	dataInConfig();
#endif

#ifdef DRIVER_ENABLE_USER_IP
/*2019/12/20*/
	Xil_Out32(0x80038,6192);
//	Xil_Out32(0x8003C,64);
	Xil_Out32(0x8003C,63);			///////////////////
	Xil_Out32(0x80014,6144);
	Xil_Out32(0x80014,0x34F3FA);
	Xil_Out32(0x80018,0xA4A3A2A1);
	Xil_Out32(0x8001c,0xC4C3C2C1);






//	Xil_Out32(0x210014,0xa);
//	//Xil_Out32(0x210004,0xB1C3D0);
//	Xil_Out32(0x210004,12675);

	Xil_Out32(0x150000,6192);
	Xil_Out32(0x150004,1024);
	Xil_Out32(0x150008,6192*16);
	Xil_Out32(0x15000c,1024/16);
	Xil_Out32(0x150020,0x80000000);
	Xil_Out32(0x150028,48);
	Xil_Out32(0x15002c,6144);

	Xil_Out32(0x150010,6192);
	Xil_Out32(0x150014,1024);
	Xil_Out32(0x150018,6192*16);
	Xil_Out32(0x15001c,1024/16);
	Xil_Out32(0x150024,0x84000000);
	Xil_Out32(0x150030,48);
	Xil_Out32(0x150034,6144);
	Xil_Out32(0x150038,4);

	Xil_Out32(0x130000,6192);
	Xil_Out32(0x130004,1024);
	Xil_Out32(0x130008,6192*16);
	Xil_Out32(0x13000c,1024/16);
	Xil_Out32(0x130020,0x88000000);
	Xil_Out32(0x130028,48);
	Xil_Out32(0x13002c,6144);
	Xil_Out32(0x130010,6192);
	Xil_Out32(0x130014,1024);
	Xil_Out32(0x130018,6192*16);
	Xil_Out32(0x13001c,1024/16);
	Xil_Out32(0x130024,0x8c000000);
	Xil_Out32(0x130030,48);
	Xil_Out32(0x130034,6144);
	Xil_Out32(0x130038,4);

	Xil_Out32(0x140000,6192);
	Xil_Out32(0x140004,1024);
	Xil_Out32(0x140008,6192*16);
	Xil_Out32(0x14000c,1024/16);
	Xil_Out32(0x140020,0x90000000);
	Xil_Out32(0x140028,48);
	Xil_Out32(0x14002c,6144);
	Xil_Out32(0x140010,6192);
	Xil_Out32(0x140014,1024);
	Xil_Out32(0x140018,6192*16);
	Xil_Out32(0x14001c,1024/16);
	Xil_Out32(0x140024,0x94000000);
	Xil_Out32(0x140030,48);
	Xil_Out32(0x140034,6144);
	Xil_Out32(0x140038,4);

	Xil_Out32(0x160000,6192);
	Xil_Out32(0x160004,1024);
	Xil_Out32(0x160008,6192*16);
	Xil_Out32(0x16000c,1024/16);
	Xil_Out32(0x160020,0x98000000);
	Xil_Out32(0x160028,48);
	Xil_Out32(0x16002c,6144);
	Xil_Out32(0x160010,6192);
	Xil_Out32(0x160014,1024);
	Xil_Out32(0x160018,6192*16);
	Xil_Out32(0x16001c,1024/16);
	Xil_Out32(0x160024,0x9c000000);
	Xil_Out32(0x160030,48);
	Xil_Out32(0x160034,6144);
	Xil_Out32(0x160038,4);
//	Xil_Out32(0x80000,0x1);
//	Xil_Out32(0x80000,0x0);
//	opFlag = 1;

	//Xil_Out32(0x210000,0xaaaa);


#endif
}
