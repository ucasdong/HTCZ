#ifndef BUFSTS_H_INCLUDED
#define BUFSTS_H_INCLUDED

#include "platform.h"

//macro define start

/*******************************************************/
//status define
#define STS_SUCCESS             0
#define STS_FAIL                1
#define INVALID_NO              2
#define INVALID_PARAMETER       3
/*******************************************************/

#define MAX_BUF_NUM             6
#define MAX_BUF_SIZE            2048

/*******************************************************/
//buffer status define
#define BUF_EMPTY              0
#define BUF_INDATA             (1 << 0)
#define BUF_FULL               (1 << 1)
#define BUF_INTERVAL           2
/*******************************************************/
//macro define end

// struct define start
/*******************************************************/
//data buffer attribute struct
typedef struct buf_attr
{
	uint32_t bufSize;
	uint32_t header;
	uint32_t tail;
	uint32_t bufAddr;
} BUF_ATTR, *PBUF_ATTR;

typedef enum
{
	BUF_NUM_UART_A = 0,
	BUF_NUM_UART_B,
#ifdef PROTOCOL_TYPE_CAN
	BUF_NUM_CAN_A,
	BUF_NUM_CAN_B,
#endif
#ifdef PROTOCOL_TYPE_SPI
	BUF_NUM_SPI,
#endif
#ifdef PROTOCOL_TYPE_SPI_NBYTE
	BUF_NUM_SPI_NBYTE,
#endif
#ifdef PROTOCOL_TYPE_UART_2M
	BUF_NUM_UART_RECV_0,
	BUF_NUM_UART_RECV_1,
	BUF_NUM_UART_RECV_2,
	BUF_NUM_UART_RECV_3,
#endif
	BUF_NUM_INVALID = 0xff
}T_BUF_NUM;

/*******************************************************/
// struct define end

//global variable declare
BUF_ATTR bufAttr[MAX_BUF_NUM];
uint32_t g32bufStatus;

/*******************************************************/
//function  declare

uint8_t checkBufStatus(uint8_t bufNo, uint8_t * bufSts);
void setBufStatus(uint8_t bufNo, uint8_t status);
int sendDatatoBuf(uint8_t bufNo, uint8_t * data, int len);
int revDatafromBuf(uint8_t bufNo, uint8_t * data, int len);
void bufattr_init(void);

/*******************************************************/

#endif // BUFSTS_H_INCLUDED
