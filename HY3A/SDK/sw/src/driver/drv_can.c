/*
 * drv_can.c
 *
 *  Created on: 2018Äê9ÔÂ13ÈÕ
 *      Author: 508
 */

#include "drv_can.h"

#ifdef DRIVER_ENABLE_CAN
/*
 * Function : sja1000Write()
 * param    :
 * 	   canNo: select CAN device
 * 	   addr : sja1000 write control register
 * 	   val  : sja1000 write value
 * return   : none
 *
 * write sja1000 register
 * */
static void sja1000Write(uint32_t regaddr, uint16_t addr, uint16_t val)
{
	uint32_t m32value = 0;

	m32value = addr;
	m32value = m32value << 16 | val;

	//write control register
	WRITE_REG(regaddr, m32value);

	return ;
}

/*
 * Function : sja1000Read()
 * param    :
 * 	   canNo: select CAN device
 * 	   addr : sja1000 read control register
 * return   : sja1000 read value
 *
 * read sja1000 register
 * */
static uint8_t sja1000Read(uint32_t regaddr, uint8_t addr)
{
	//read enable
	WRITE_REG(regaddr +  0x04, addr);

	//return read can value
	return (uint8_t)(READ_REG(regaddr + 0x08));
}

//receive data to buffer
static uint8_t can_recv(S_DEV_CONF* conf)
{
	uint8_t m8datalen  = 0; 		// store the frame information data length
	uint8_t m8info     = 0; 		// store the frame information
	uint8_t m8loop     = 0;
	uint8_t m8bufSts   = 0;         // get buffer status
	uint8_t m8bufnum   = BUF_NUM_INVALID; // store buffer number
	uint8_t m8databuf[16] = {0};    // data buffer

	uint32_t m32timeout = 0;        // timeout counter
	int     m32actuallen = 0;       // actual data length send to buffer

	//select can device number
	switch(conf->dev_id)
	{
	case DEV_NUM_CAN_A:
		m8bufnum = BUF_NUM_CAN_A;
		break;
	case DEV_NUM_CAN_B:
		m8bufnum = BUF_NUM_CAN_B;
		break;
	default:
		break;
	}

	//get message frame information
	m8info = sja1000Read(conf->base_addr, CAN_RXBUF);
	if (!(m8info & EXTEND_FRAME_FORMAT))   //standard Frame Format
	{
		m8datalen = m8info & 0x0f;
		m8databuf[0] = sja1000Read(conf->base_addr, CAN_RXBUF + 1); // receive id0
		m8databuf[1] = sja1000Read(conf->base_addr, CAN_RXBUF + 2); // receive id1
		m8databuf[1] = m8databuf[1] | m8datalen;

		//receive valid data space
		for(m8loop = 0; m8loop < m8datalen; m8loop++)
		{
			m8databuf[2 + m8loop] = sja1000Read(conf->base_addr, CAN_RXBUF + 3 + m8loop);
		}
	}

	//send data to buffer
	do
	{
		checkBufStatus(m8bufnum, &m8bufSts);
		if ((m8bufSts & BUF_FULL) != BUF_FULL)
		{
			m32actuallen = sendDatatoBuf(m8bufnum , (uint8_t*)m8databuf, (8 + 2));
			if(m32actuallen != (8 + 2))
			{
				//DEV_IO_MSG("%s : %d there have no enough space to store can message\n", __func__, __LINE__);
			}
			break;
		}
		m32timeout++;
	}while  (m32timeout < MAX_TIMEOUT);

	if(m32timeout > MAX_TIMEOUT)
	{
		//DEV_IO_MSG("Receive Data Timeout!\n\r");
	}

	//release receive buffer
	sja1000Write(conf->base_addr, CAN_CMR, C_CMR_RRB);

	return (uint8_t)m32actuallen;
}

static int can_init(uint32_t addr)
{
	uint8_t m8loop = 0;
	uint8_t m8result = 0;

	/* initial device can to Peli mode */

	//enter reset mode
	for(m8loop = 0; m8loop < 0x10; m8loop++)
	{
		sja1000Write(addr, CAN_MOD, C_CAN_MOD_RESET);

		m8result = sja1000Read(addr, CAN_MOD);

		if((m8result & C_CAN_MOD_RESET) == C_CAN_MOD_RESET)
		{
			//when check the reset mode, break
			break;
		}
	}

	/* Select PeliCan mode, disable CBP and disable CLKOUT */
	sja1000Write(addr, CAN_CDR, C_CAN_CDR);

	//disable can intr
	sja1000Write(addr, CAN_IER, C_CAN_IER_DISABLE);

	/* Init Frame Format Accept special ID */
	//acr
	sja1000Write(addr, CAN_ACR0, C_CAN_ACR0);
	sja1000Write(addr, CAN_ACR1, C_CAN_ACR1);
	sja1000Write(addr, CAN_ACR2, C_CAN_ACR2);
	sja1000Write(addr, CAN_ACR3, C_CAN_ACR3);

	//amr
	sja1000Write(addr, CAN_AMR0, C_CAN_AMR0);
	sja1000Write(addr, CAN_AMR1, C_CAN_AMR1);
	sja1000Write(addr, CAN_AMR2, C_CAN_AMR2);
	sja1000Write(addr, CAN_AMR3, C_CAN_AMR3);

	//set can baud rate
	sja1000Write(addr, CAN_BTR0, C_CAN_BTR0);
	sja1000Write(addr, CAN_BTR1, C_CAN_BTR1);

	//set output enable
	sja1000Write(addr, CAN_OCR, C_CAN_OCR);

	//set RXfifo Base Addr  to 0
	sja1000Write(addr, CAN_RXFIFO, C_CAN_RXFIFO);

	//enable receive interrupt
	sja1000Write(addr, CAN_IER, C_CAN_IER_RECEIVE);

	//enter normal mode
	for(m8loop = 0; m8loop < 0x10; m8loop++)
	{
		sja1000Write(addr, CAN_MOD, C_CAN_MOD_WORK);

		m8result = sja1000Read(addr, CAN_MOD);

		if((m8result & C_CAN_MOD_WORK) == C_CAN_MOD_WORK)
		{
			//when check the reset mode, break
			break;
		}
	}

	return CAN_SUCCESS;
}


/*
 * function : dev_can_init
 * */
uint8_t drv_can_init(void)
{
	uint8_t m8sts = 0;

	m8sts = can_init(DEV_CAN_A);
    if(m8sts) // error handle
    {
    	xil_printf("%s : %d Device Can A initial Fail\r\n", __func__, __LINE__);
    	return CAN_FAIL;
    }
#ifdef __ENABLE_CAN_B__
	m8sts = can_init(DEV_CAN_B);
	if(m8sts) // error handle
	{
		xil_printf("%s : %d Device Can B initial Fail\r\n", __func__, __LINE__);
		return CAN_FAIL;
	}
#endif

	return CAN_SUCCESS;
}
#if 0
static uint8_t sja1000_txbuf_is_idle(uint8_t canNo)
{
	if(sja1000Read(canNo, CAN_SR) && (C_SR_TBS_EMPTY | C_SR_TCS_COMPLETE) == (C_SR_TBS_EMPTY | C_SR_TCS_COMPLETE))
	{
		return 1;
	}
	else
	{
		return 0;
	}

	return 0;
}
#endif
//transmit data
uint8_t drv_can_transmit(uint32_t addr, uint8_t* data, uint32_t len)
{
	uint8_t m8loop    = 0;
	uint8_t m8counter = 0;
	uint8_t m8datalen = 0;
	uint32_t m32delay = 0;

	for(m8counter = 0; m8counter <= (len / MAX_LEN_TX_ONCE); m8counter++)
	{
		if(m8counter * MAX_LEN_TX_ONCE == len)  // if len is divisible by MAX_LEN_TX_ONCE
		{
			break;
		}

		//check transmit buffer status
//		while(!sja1000_txbuf_is_idle(addr));

		m8datalen = data[m8counter * MAX_LEN_TX_ONCE + 1] & 0x0f;// get actually transfer data length
#if 1
		sja1000Write(addr, CAN_TXBUF, data[m8counter * MAX_LEN_TX_ONCE + 1]& 0x0f);
				//fill ID0 - ID1
		sja1000Write(addr, CAN_TXBUF + 1, data[m8counter * MAX_LEN_TX_ONCE]);
		sja1000Write(addr, CAN_TXBUF + 2, data[m8counter * MAX_LEN_TX_ONCE+1]);
#else
		//fill RTR DLC
		sja1000Write(addr, CAN_TXBUF, data[m8counter * MAX_LEN_TX_ONCE + 1]);
		//fill ID0 - ID1
		sja1000Write(addr, CAN_TXBUF + 1, data[m8counter * MAX_LEN_TX_ONCE]);
		sja1000Write(addr, CAN_TXBUF + 2, data[m8counter * MAX_LEN_TX_ONCE + 1] & 0xf0);
#endif
		for(m8loop = 0; m8loop < m8datalen; m8loop++)
		{
			sja1000Write(addr, CAN_TXBUF + 3 + m8loop,
					data[m8counter * MAX_LEN_TX_ONCE + 2 + m8loop]);
		}

		sja1000Write(addr, CAN_CMR, C_CMR_TR);

		//delay for can data transfer
		for(m32delay = 0; m32delay < 0x1000; m32delay++);
	}

	return (m8counter * MAX_LEN_TX_ONCE + m8loop);
}


//sja1000 receive interrupt handler
void drv_can_isr(S_DEV_CONF* conf)
{
    uint8_t m8canIntr     = 0;
    uint8_t	m8canIntrMask = 0;

    m8canIntrMask = sja1000Read(conf->base_addr, CAN_IER);

    if(m8canIntrMask)
    {
    	m8canIntr = sja1000Read(conf->base_addr, CAN_IR);

		if (((m8canIntrMask & INT_EN_RI) & (m8canIntr & INT_RI)) == INT_RI)
		{
			/* disable receive interrupt */
			sja1000Write(conf->base_addr, CAN_IER, C_CAN_IER_DISABLE);

			can_recv(conf);

			/* enable receive interrupt */
			sja1000Write(conf->base_addr, CAN_IER, C_CAN_IER_RECEIVE);
		}
    }

	return ;
}


static void can_intr_a_handler(void *InstancePtr)
{
	S_DEV_INFO * dev = InstancePtr;

	drv_can_isr(dev->dev_conf);
}


static void can_intr_b_handler(void *InstancePtr)
{
	S_DEV_INFO * dev = InstancePtr;

	drv_can_isr(dev->dev_conf);
}

/* platform device CAN operation functions implementation */

int dev_can_init(S_DEV_INFO * dev)
{
	int ret = CAN_SUCCESS;

	//parameter check
	if(dev->dev_conf->dev_id == DEV_NUM_INVALID)
	{
		MST_IO_MSG( "%s: CAN initial fail, device No %d\n", __func__, dev->dev_conf->dev_id);
		return CAN_FAIL;
	}

	//initial can device
	ret = can_init(dev->dev_conf->base_addr);

	//connect can interrupt to interrupt controller
	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device.
	 */
	switch(dev->dev_conf->intr_num)
	{
	case XPAR_CPU_AXI_INTC_0_CPU_UTIL_VECTOR_LOGIC_1_RES_INTR:
		ret = XIntc_Connect(&InterruptController, dev->dev_conf->intr_num,
			   (XInterruptHandler)can_intr_a_handler,
			   (void *)dev);
		break;
	case XPAR_CPU_AXI_INTC_0_CPU_UTIL_VECTOR_LOGIC_0_RES_INTR:
		ret = XIntc_Connect(&InterruptController, dev->dev_conf->intr_num,
			   (XInterruptHandler)can_intr_b_handler,
			   (void *)dev);
		break;
	default:
		ret = CAN_FAIL;
		break;
	}
	if (ret != XST_SUCCESS) {
		return ret;
	}

	/*
	 * Enable the interrupt for the CAN device.
	 */
	XIntc_Enable(&InterruptController, dev->dev_conf->intr_num);

	return ret;
}

int dev_can_write(S_DEV_INFO * dev)
{
	int ret = CAN_SUCCESS;
	//parameter check
	if(dev->dev_conf->dev_id == DEV_NUM_INVALID)
	{
		MST_IO_MSG( "%s: CAN Transfer fail, device No %d\n", __func__, dev->dev_conf->dev_id);
		return CAN_FAIL;
	}

	//parameter check
	if(dev->dev_can_arg->src == NULL)
	{
		MST_IO_MSG( "%s: CAN Transfer fail, source data invalid\n", __func__);
		return CAN_FAIL;
	}

	drv_can_transmit(dev->dev_conf->base_addr, dev->dev_can_arg->src, dev->dev_can_arg->len);

	return ret;
}

int dev_can_read(S_DEV_INFO * dev)
{
	int ret = CAN_SUCCESS;

	return ret;
}

void dev_can_ctrl(uint8_t * arg)
{

}

/* can driver function define */
S_DEV_CAN_OPS platform_dev_can_ops = {
		.init  = dev_can_init,
		.write = dev_can_write,
		.read  = dev_can_read,
		.ctrl  = dev_can_ctrl
};

#endif /* DRIVER_ENABLE_CAN */
