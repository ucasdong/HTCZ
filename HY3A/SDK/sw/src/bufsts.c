
#include "bufsts.h"

/*
    Function : checkBufStatus
        check special buffer status (full, in data, empty)
    Param    :
        bufNo  : buffer index
        bufSts : return buffer status
    Return : function status
*/
uint8_t checkBufStatus(uint8_t bufNo, uint8_t * bufSts)
{
	if (bufNo >= MAX_BUF_NUM)
	{
		MST_BUF_MSG("checkBufStatus: Invalid buffer No: %d\n", bufNo);
		return (-INVALID_PARAMETER);
	}
	if (bufSts == NULL)
	{
		MST_BUF_MSG("checkBufStatus: Invalid parameters bufsts\n");
		return (-INVALID_PARAMETER);
	}

	*bufSts = (uint8_t)(g32bufStatus >> (bufNo * BUF_INTERVAL));

	return (STS_SUCCESS);
}

/*
    Function : setBufStatus
        set special buffer status to global var
    Param    :
        bufNo  : buffer index
        bufSts : buffer status
    Return : None
*/
void setBufStatus(uint8_t bufNo, uint8_t status)
{
	if (bufNo >= MAX_BUF_NUM)
	{
		MST_BUF_MSG("setBufStatus: Invalid buffer No: %d\n", bufNo);
		return;
	}
	g32bufStatus &= ~(3 << (bufNo * BUF_INTERVAL));
	g32bufStatus |= (uint32_t)(((status) << (bufNo * BUF_INTERVAL)) & 0xFFFFFFFF);
	MST_BUF_MSG("setBufStatus: buffer status: %d\n", g32bufStatus);
}

/*
    Function : sendDatatoBuf
        send data to data buffer
    Param    :
        bufNo  : buffer index
        data   : valid data ptr
        len    : data length
    Return   : actual data length to buffer
*/
int sendDatatoBuf(uint8_t bufNo, uint8_t * data, int len)
{
	int i, lenTemp = 0;
	uint8_t bufSts = 0;

	if (bufNo >= MAX_BUF_NUM)
	{
		MST_BUF_MSG("sendDatatoBuf: Invalid buffer No: %d\n", bufNo);
		return (-1);
	}

	checkBufStatus(bufNo, &bufSts);
	if ((bufSts & BUF_FULL) == BUF_FULL)
	{
		MST_BUF_MSG("bufSts is FULL, need return\n");
		return (-1);
	}
	for (i = 0; i < len; i++)
	{
		*(volatile uint8_t *)(bufAttr[bufNo].bufAddr + bufAttr[bufNo].tail) = *(volatile uint8_t *)(data + i);
		bufAttr[bufNo].tail = (bufAttr[bufNo].tail + 1) % bufAttr[bufNo].bufSize;
		lenTemp++;
		if (bufAttr[bufNo].header == bufAttr[bufNo].tail)
		{
			setBufStatus(bufNo, BUF_FULL);
			return lenTemp;
		}
	}
	setBufStatus(bufNo, BUF_INDATA);
	return lenTemp;
}

/*
    Function : revDatafromBuf
        receive data from data buffer, and set data buffer status
    Param    :
        bufNo  : buffer index
        data   : return data ptr
        len    : data length
    Return     : actual data length from buffer
*/
int revDatafromBuf(uint8_t bufNo, uint8_t * data, int len)
{
	int i, lenTemp = 0;
	uint8_t bufSts;

	if (bufNo >= MAX_BUF_NUM)
	{
		MST_BUF_MSG("revDatafromBuf: Invalid buffer No: %d\n", bufNo);
		return (-1);
	}
	checkBufStatus(bufNo, &bufSts);
	if (bufSts == BUF_EMPTY)
	{
		MST_BUF_MSG("bufSts is EMPTY, need return\n");
		return (-1);
	}
	for (i = 0; i < len; i++)
	{
		*(volatile uint8_t *)(data + i) = *(volatile uint8_t *)(bufAttr[bufNo].bufAddr + bufAttr[bufNo].header);
		bufAttr[bufNo].header = (bufAttr[bufNo].header + 1) % bufAttr[bufNo].bufSize;
		lenTemp++;
		if (bufAttr[bufNo].tail == bufAttr[bufNo].header)
		{
			setBufStatus(bufNo, BUF_EMPTY);
			return lenTemp;
		}
	}

	return lenTemp;
}

void bufattr_init(void)
{
	uint8_t m8loop = 0;

	for(m8loop = 0; m8loop < MAX_BUF_NUM; m8loop++)
	{
		bufAttr[m8loop].bufAddr = (0x18000 + m8loop * MAX_BUF_SIZE);
		bufAttr[m8loop].bufSize = MAX_BUF_SIZE;
		bufAttr[m8loop].header  = 0;
		bufAttr[m8loop].tail    = 0;
	}
	g32bufStatus = 0;
}
