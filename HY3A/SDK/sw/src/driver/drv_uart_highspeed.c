/*
 * drv_uart_highspped.c
 *
 *  Created on: 2019Äê2ÔÂ26ÈÕ
 *      Author: 508
 */


#include "drv_uart_highspeed.h"

static void uart_mst2mst_recv(void *InstancePtr)
{
	S_DEV_INFO * dev = InstancePtr;
	uint32_t m32data = 0;
	uint32_t m32length = 0;
	uint8_t  buf[256] = {0};

	while(1)
	{
		m32data = READ_REG(dev->dev_conf->base_addr + UART_REG_READ_DATA);
		buf[m32length + 0] = (m32data >> 24) & 0xff;
		buf[m32length + 1] = (m32data >> 16) & 0xff;
		buf[m32length + 2] = (m32data >>  8) & 0xff;
		buf[m32length + 3] = (m32data      ) & 0xff;
		m32length+=4;
		m32data = READ_REG(dev->dev_conf->base_addr + UART_REG_DATA);
		if(m32data & UART_FIFO_EMPTY)
		{
			break;
		}
	}
	sendDatatoBuf(BUF_NUM_UART_RECV_0, (uint8_t*)buf, m32length);

	return ;
}

static void uart_mst2slv_recv(void *InstancePtr)
{
	S_DEV_INFO * dev = InstancePtr;
	uint32_t m32data = 0;
	uint32_t m32length = 0;
	uint8_t  buf[256] = {0};

	while(1)
	{
		m32data = READ_REG(dev->dev_conf->base_addr + UART_REG_READ_DATA);
		buf[m32length + 0] = (m32data >> 24) & 0xff;
		buf[m32length + 1] = (m32data >> 16) & 0xff;
		buf[m32length + 2] = (m32data >>  8) & 0xff;
		buf[m32length + 3] = (m32data      ) & 0xff;
		m32length+=4;

		m32data = READ_REG(dev->dev_conf->base_addr + UART_REG_DATA);
		if(m32data & UART_FIFO_EMPTY)
		{
			break;
		}
	}
	sendDatatoBuf(BUF_NUM_UART_RECV_1, (uint8_t*)buf, m32length);

	return ;
}

static void uart_slv2mst_recv(void *InstancePtr)
{
	S_DEV_INFO * dev = InstancePtr;
	uint32_t m32data = 0;
	uint32_t m32length = 0;
	uint8_t  buf[256] = {0};

	while(1)
	{
		m32data = READ_REG(dev->dev_conf->base_addr + UART_REG_READ_DATA);
		buf[m32length + 0] = (m32data >> 24) & 0xff;
		buf[m32length + 1] = (m32data >> 16) & 0xff;
		buf[m32length + 2] = (m32data >>  8) & 0xff;
		buf[m32length + 3] = (m32data      ) & 0xff;
		m32length+=4;

		m32data = READ_REG(dev->dev_conf->base_addr + UART_REG_DATA);
		if(m32data & UART_FIFO_EMPTY)
		{
			break;
		}
	}
	sendDatatoBuf(BUF_NUM_UART_RECV_2, (uint8_t*)buf, m32length);

	return ;
}

static void uart_slv2slv_recv(void *InstancePtr)
{
	S_DEV_INFO * dev = InstancePtr;
	uint32_t m32data = 0;
	uint32_t m32length = 0;
	uint8_t  buf[256] = {0};

	while(1)
	{
		m32data = READ_REG(dev->dev_conf->base_addr + UART_REG_READ_DATA);
		buf[m32length + 0] = (m32data >> 24) & 0xff;
		buf[m32length + 1] = (m32data >> 16) & 0xff;
		buf[m32length + 2] = (m32data >>  8) & 0xff;
		buf[m32length + 3] = (m32data      ) & 0xff;
		m32length+=4;

		m32data = READ_REG(dev->dev_conf->base_addr + UART_REG_DATA);
		if(m32data & UART_FIFO_EMPTY)
		{
			break;
		}
	}
	sendDatatoBuf(BUF_NUM_UART_RECV_3, (uint8_t*)buf, m32length);

	return ;
}

int dev_uart_highspeed_init(S_DEV_INFO * dev)
{
	int ret = UART_HIGH_SPEED_SUCCESS;
	uint32_t m32data = 0;


	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device.
	 */
	if(dev->dev_conf->intr_num != DEV_NUM_INVALID)
	{
		switch(dev->dev_conf->intr_num)
		{
		case XPAR_CPU_AXI_INTC_0_LOW_SPEED_PERIPHERAL_UART_RXD_MM_TXD_DET_LITETOFIFO_V1_0_S00_AXI_0_INTRUPT_O_INTR:
			ret = XIntc_Connect(&InterruptController, dev->dev_conf->intr_num,
					(XInterruptHandler)uart_mst2mst_recv,
					(void *)dev);
			break;
		case XPAR_CPU_AXI_INTC_0_LOW_SPEED_PERIPHERAL_UART_RXD_MS_TXD_REM_LITETOFIFO_V1_0_S00_AXI_0_INTRUPT_O_INTR:
			ret = XIntc_Connect(&InterruptController, dev->dev_conf->intr_num,
					(XInterruptHandler)uart_mst2slv_recv,
					(void *)dev);
			break;
		case XPAR_CPU_AXI_INTC_0_LOW_SPEED_PERIPHERAL_UART_RXD_S_M_LITETOFIFO_V1_0_S00_AXI_0_INTRUPT_O_INTR:
			ret = XIntc_Connect(&InterruptController, dev->dev_conf->intr_num,
					(XInterruptHandler)uart_slv2mst_recv,
					(void *)dev);
			break;
		case XPAR_CPU_AXI_INTC_0_LOW_SPEED_PERIPHERAL_UART_RXD_S_S_LITETOFIFO_V1_0_S00_AXI_0_INTRUPT_O_INTR:
			ret = XIntc_Connect(&InterruptController, dev->dev_conf->intr_num,
					(XInterruptHandler)uart_slv2slv_recv,
					(void *)dev);
			break;
		case XPAR_CPU_AXI_INTC_0_LOW_SPEED_PERIPHERAL_UART_2M_LITETOFIFO_V1_0_S00_AXI_0_INTRUPT_O_INTR:
			break;
		default:
			break;
		}


		if (ret != XST_SUCCESS)
		{
			return UART_HIGH_SPEED_FAIL;
		}

		while(1)
		{
			m32data = READ_REG(dev->dev_conf->base_addr + UART_REG_READ_DATA);
			m32data = READ_REG(dev->dev_conf->base_addr + UART_REG_DATA);
			if(m32data & UART_FIFO_EMPTY)
			{
				break;
			}
		}

		/*
		* Enable the interrupt for the UartLite device.
		*/
		XIntc_Enable(&InterruptController, dev->dev_conf->intr_num);
	}
	WRITE_REG(dev->dev_conf->base_addr + UART_REG_INTERVAL, 500);

	return ret;
}

int dev_uart_high_speed_send(S_DEV_INFO * dev)
{
	uint32_t m32loop = 0;
	uint32_t loop = 0;
	uint32_t data = 0;

	if(dev->dev_conf->dev_id == DEV_NUM_INVALID)
	{
		MST_IO_MSG( "%s: UART High Speed Transfer fail, device No %d\n", __func__, dev->dev_conf->dev_id);
		return UART_HIGH_SPEED_FAIL;
	}

	if(dev->dev_uart_arg->src == NULL || dev->dev_uart_arg->len <= 0)
	{
		MST_IO_MSG( "%s: UART High Speed Transfer fail, source addr is null\n", __func__);
		return UART_HIGH_SPEED_FAIL;
	}

	for(m32loop = 0; m32loop < dev->dev_uart_arg->len / 4; m32loop++)
	{
		data = 0;
		for(loop = 0; loop < 4; loop++)
		{
			data = data << 8 | dev->dev_uart_arg->src[m32loop * 4 + loop];
		}
		WRITE_REG(dev->dev_conf->base_addr + UART_REG_DATA, data);
	}

	WRITE_REG(dev->dev_conf->base_addr + UART_REG_LENGTH, dev->dev_uart_arg->len / 4);
	WRITE_REG(dev->dev_conf->base_addr + UART_REG_CNTL, UART_START_TREANSFER); //start transfer
	WRITE_REG(dev->dev_conf->base_addr + UART_REG_CNTL, 0); //start transfer

	return  UART_HIGH_SPEED_SUCCESS;
}
