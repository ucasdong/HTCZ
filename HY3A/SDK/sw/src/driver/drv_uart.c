/*
 * drv_uart.c
 *
 *  Created on: 2018Äê10ÔÂ24ÈÕ
 *      Author: 508
 */


#include "drv_uart.h"

#ifdef DRIVER_ENABLE_UART

unsigned int TotalReceivedCount = 0;
unsigned int TotalReceivedCountMsg = 0;

static void SendHandler(void *CallBackRef, unsigned int EventData)
{

}

static void RecvHandler(void *CallBackRef, unsigned int EventData)
{
	static uint8_t m8data[XUART_BUFFER_SIZE] = {0};
//	uint8_t m8len = 0;

	 while(!XUartLite_IsReceiveEmpty(XPAR_UARTLITE_0_BASEADDR))
	 {
		m8data[TotalReceivedCount] = XUartLite_RecvByte(XPAR_UARTLITE_0_BASEADDR);

//		m8len = XUartLite_Recv(&UartLite[0], &m8data[TotalReceivedCount], EventData);
//		TotalReceivedCount += m8len;
		TotalReceivedCount++;
//		for(m8len = 0; m8len < 0x10; m8len++);

		if((TotalReceivedCount % XUART_BUFFER_SIZE) == 0)
		{
			sendDatatoBuf(BUF_NUM_UART_A, (uint8_t*)m8data, XUART_BUFFER_SIZE);
			TotalReceivedCount = 0;
		}
	 }
}

static void msgSendHandler(void *CallBackRef, unsigned int EventData)
{

}

static void msgRecvHandler(void *CallBackRef, unsigned int EventData)
{
	static uint8_t m8data[XUART_BUFFER_SIZE] = {0};
//	uint8_t m8len = 0;

	while(!XUartLite_IsReceiveEmpty(XPAR_UARTLITE_1_BASEADDR))
	{
		m8data[TotalReceivedCountMsg] = XUartLite_RecvByte(XPAR_UARTLITE_1_BASEADDR);

		TotalReceivedCountMsg++;
//		for(m8len = 0; m8len < 0x10; m8len++);
		if((TotalReceivedCountMsg % XUART_BUFFER_SIZE) == 0)
		{
			sendDatatoBuf(BUF_NUM_UART_B, (uint8_t*)m8data, XUART_BUFFER_SIZE);
			TotalReceivedCountMsg = 0;
		}
	}
}


int dev_uart_init(S_DEV_INFO * dev)
{
	uint16_t deviveId = 0xff;
	int Status;

	/*
	 * Initialize the UartLite driver so that it's ready to use.
	 */
	switch(dev->dev_conf->dev_id)
	{
	case DEV_NUM_UART_A:
		deviveId = UART_DEV_ID_0;
		break;
	case DEV_NUM_UART_B:
		deviveId = UART_DEV_ID_1;
		break;
	default:
		break;
	}

	Status = XUartLite_Initialize(&UartLite[deviveId], deviveId);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XUartLite_SelfTest(&UartLite[deviveId]);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device.
	 */
	Status = XIntc_Connect(&InterruptController, dev->dev_conf->intr_num,
			   (XInterruptHandler)XUartLite_InterruptHandler,
			   (void *)&UartLite[deviveId]);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Enable the interrupt for the UartLite device.
	 */
	XIntc_Enable(&InterruptController, dev->dev_conf->intr_num);

	/*
	 * Setup the handlers for the UartLite that will be called from the
	 * interrupt context when data has been sent and received, specify a
	 * pointer to the UartLite driver instance as the callback reference so
	 * that the handlers are able to access the instance data.
	 */
	switch(dev->dev_conf->dev_id)
	{
	case DEV_NUM_UART_A:
		XUartLite_SetSendHandler(&UartLite[deviveId], SendHandler, &UartLite[deviveId]);
		XUartLite_SetRecvHandler(&UartLite[deviveId], RecvHandler, &UartLite[deviveId]);
		break;
	case DEV_NUM_UART_B:
		XUartLite_SetSendHandler(&UartLite[deviveId], msgSendHandler, &UartLite[deviveId]);
		XUartLite_SetRecvHandler(&UartLite[deviveId], msgRecvHandler, &UartLite[deviveId]);
		break;
	default:
		break;
	}
	/*
	 * Enable the interrupt of the UartLite so that interrupts will occur.
	 */
	XUartLite_EnableInterrupt(&UartLite[deviveId]);


	return Status;
}

int dev_uart_transfer(S_DEV_INFO * dev)
{
	//parameter check
	if(dev->dev_conf->dev_id == DEV_NUM_INVALID)
	{
		MST_IO_MSG( "%s: CAN Transfer fail, device No %d\n", __func__, dev->dev_conf->dev_id);
		return UART_FAIL;
	}

	//parameter check
	if(dev->dev_uart_arg->src == NULL)
	{
		MST_IO_MSG( "%s: CAN Transfer fail, source data invalid\n", __func__);
		return UART_FAIL;
	}
	switch(dev->dev_conf->dev_id)
	{
	case DEV_NUM_UART_A:
		XUartLite_Send(&UartLite[0], dev->dev_uart_arg->src, dev->dev_uart_arg->len);
		break;
	case DEV_NUM_UART_B:
		XUartLite_Send(&UartLite[1], dev->dev_uart_arg->src, dev->dev_uart_arg->len);
		break;
	default:
		break;
	}

	return UART_SUCCESS;
}

#endif /* DRIVER_ENABLE_UART */
