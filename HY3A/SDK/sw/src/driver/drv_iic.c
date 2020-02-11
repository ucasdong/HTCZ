/*
 * drv_iic.c
 *
 *  Created on: 2018Äê9ÔÂ14ÈÕ
 *      Author: 508
 */

#include "driver.h"
#include "drv_iic.h"

#ifdef DRIVER_ENABLE_IIC


static void SendHandler(XIic * InstancePtr)
{
	TransmitComplete = 0;
}

static void ReceiveHandler(XIic * InstancePtr)
{
	ReceiveComplete = 0;
}

static void StatusHandler(XIic * InstancePtr, int Event)
{

}


int EepromWriteData(u16 ByteCount)
{
	int Status;

	/*
	 * Set the defaults.
	 */
	TransmitComplete = 1;
	IicInstance.Stats.TxErrors = 0;

	/*
	 * Start the IIC device.
	 */
	Status = XIic_Start(&IicInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Send the Data.
	 */
	Status = XIic_MasterSend(&IicInstance, WriteBuffer, ByteCount);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Wait till the transmission is completed.
	 */
	while ((TransmitComplete) || (XIic_IsIicBusy(&IicInstance) == TRUE)) {
		/*
		 * This condition is required to be checked in the case where we
		 * are writing two consecutive buffers of data to the EEPROM.
		 * The EEPROM takes about 2 milliseconds time to update the data
		 * internally after a STOP has been sent on the bus.
		 * A NACK will be generated in the case of a second write before
		 * the EEPROM updates the data internally resulting in a
		 * Transmission Error.
		 */
		if (IicInstance.Stats.TxErrors != 0) {


			/*
			 * Enable the IIC device.
			 */
			Status = XIic_Start(&IicInstance);
			if (Status != XST_SUCCESS) {
				return XST_FAILURE;
			}


			if (!XIic_IsIicBusy(&IicInstance)) {
				/*
				 * Send the Data.
				 */
				Status = XIic_MasterSend(&IicInstance,
							 WriteBuffer,
							 ByteCount);
				if (Status == XST_SUCCESS) {
					IicInstance.Stats.TxErrors = 0;
				}
				else {
				}
			}
		}
	}

	/*
	 * Stop the IIC device.
	 */
	Status = XIic_Stop(&IicInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int EepromReadData(u8 *BufferPtr, u16 ByteCount)
{
	int Status;
	AddressType Address = EEPROM_TEST_START_ADDRESS;

	/*
	 * Set the Defaults.
	 */
	ReceiveComplete = 1;

	/*
	 * Position the Pointer in EEPROM.
	 */
	if (sizeof(Address) == 1) {
		WriteBuffer[0] = (u8) (Address);
	}
	else {
		WriteBuffer[0] = (u8) (Address >> 8);
		WriteBuffer[1] = (u8) (Address);
	}

//	Status = EepromWriteData(sizeof(Address));
//	if (Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}

	/*
	 * Start the IIC device.
	 */
	Status = XIic_Start(&IicInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Receive the Data.
	 */
	Status = XIic_MasterRecv(&IicInstance, BufferPtr, ByteCount);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Wait till all the data is received.
	 */
	while ((ReceiveComplete) || (XIic_IsIicBusy(&IicInstance) == TRUE)) {

	}

	/*
	 * Stop the IIC device.
	 */
	Status = XIic_Stop(&IicInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int EepromReadData2(AddressType addr, u8 *BufferPtr, u16 ByteCount)
{
	int Status;
//	AddressType Address = EEPROM_TEST_START_ADDRESS;
	AddressType Address;
	Address = addr;

	/*
	 * Set the Defaults.
	 */
	ReceiveComplete = 1;

	/*
	 * Position the Pointer in EEPROM.
	 */
	if (sizeof(Address) == 1) {
		WriteBuffer[0] = (u8) (Address);
	}
	else {
		WriteBuffer[0] = (u8) (Address >> 8);
		WriteBuffer[1] = (u8) (Address);
	}

	Status = EepromWriteData(sizeof(Address));
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Start the IIC device.
	 */
	Status = XIic_Start(&IicInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Receive the Data.
	 */
	Status = XIic_MasterRecv(&IicInstance, BufferPtr, ByteCount);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Wait till all the data is received.
	 */
	while ((ReceiveComplete) || (XIic_IsIicBusy(&IicInstance) == TRUE)) {

	}

	/*
	 * Stop the IIC device.
	 */
	Status = XIic_Stop(&IicInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

// IIC select si570 device with pca9548 MUX
static uint8_t select_si570(uint8_t si750No)
{
	int Status = XST_FAILURE;

	//select switch address
	Status = XIic_SetAddress(&IicInstance, XII_ADDR_TO_SEND_TYPE,
			 IIC_SWITCH_ADDRESS);
	if (Status != XST_SUCCESS) {
		return IIC_FAIL;
	}

	//turn on the target channel
	switch(si750No)
	{
	case DEV_IIC_SI570433A:
		WriteBuffer[0] = IIC_BUS_1; //Select IIC Bus 1 - U2 570 A
		break;
	case DEV_IIC_SI570433B:
		WriteBuffer[0] = IIC_BUS_2; //Select IIC Bus 2 - U2 570 B
		break;
	default:
		break;
	}

	Status = EepromWriteData(1);
	if (Status != XST_SUCCESS) {
		return IIC_FAIL;
	}

	//select target slave address
	Status = XIic_SetAddress(&IicInstance, XII_ADDR_TO_SEND_TYPE,
			IIC_SI570433_ADDRESS);
	if (Status != XST_SUCCESS) {
		return IIC_FAIL;
	}

	return IIC_SUCCESS;
}

// IIC select si570 device with pca9548 MUX
static uint8_t select_si5324(void)
{
	int Status = XST_FAILURE;

	//select switch address
	Status = XIic_SetAddress(&IicInstance, XII_ADDR_TO_SEND_TYPE,
			 IIC_SWITCH_ADDRESS);
	if (Status != XST_SUCCESS) {
		return IIC_FAIL;
	}

	//turn on the target channel
	WriteBuffer[0] = IIC_BUS_4; //Select IIC Bus 1 - U2 570
	Status = EepromWriteData(1);
	if (Status != XST_SUCCESS) {
		return IIC_FAIL;
	}

	//select target slave address
	Status = XIic_SetAddress(&IicInstance, XII_ADDR_TO_SEND_TYPE,
			IIC_SI5324_ADDRESS);
	if (Status != XST_SUCCESS) {
		return IIC_FAIL;
	}

	return IIC_SUCCESS;
}


//IIC select pca9548 MUX
static uint8_t select_switch(void)
{
	int Status = XST_FAILURE;

	//select switch address
	Status = XIic_SetAddress(&IicInstance, XII_ADDR_TO_SEND_TYPE,
			 IIC_SWITCH_ADDRESS);
	if (Status != XST_SUCCESS) {
		return IIC_FAIL;
	}

	return IIC_SUCCESS;
}

// IIC select dm100mb203 device with pca9548 MUX and pca95
static uint8_t select_b203(uint8_t b203No)
{
	int Status = XST_FAILURE;

	//select switch address
	Status = XIic_SetAddress(&IicInstance, XII_ADDR_TO_SEND_TYPE,
			 IIC_SWITCH_ADDRESS);
	if (Status != XST_SUCCESS) {
		return IIC_FAIL;
	}

	//turn on the target channel
	WriteBuffer[0] = IIC_BUS_7; //Select IIC Bus 7 - U7 second switch mux
	Status = EepromWriteData(1);
	if (Status != XST_SUCCESS) {
		return IIC_FAIL;
	}

	//select target slave address, second switch device
	Status = XIic_SetAddress(&IicInstance, XII_ADDR_TO_SEND_TYPE,
			IIC_SWITCH_SECOND_ADDRESS);
	if (Status != XST_SUCCESS) {
		return IIC_FAIL;
	}

	//turn on the target channel
	switch(b203No)
	{
	case IIC_DM100MB203A_ADDRESS:
		WriteBuffer[0] = IIC_BUS2_0; //Select IIC Bus 0 - U0 B203A
		break;
	case IIC_DM100MB203B_ADDRESS:
		WriteBuffer[0] = IIC_BUS2_1; //Select IIC Bus 1 - U1 B203B
		break;
	case IIC_DM100MB203C_ADDRESS:
		WriteBuffer[0] = IIC_BUS2_2; //Select IIC Bus 2 - U2 B203C
		break;
	case IIC_DM100MB203D_ADDRESS:
		WriteBuffer[0] = IIC_BUS2_3; //Select IIC Bus 3 - U3 B203D
		break;
	default:
		break;
	}

	Status = EepromWriteData(1);
	if (Status != XST_SUCCESS) {
		return IIC_FAIL;
	}

	//select target b203 device
	Status = XIic_SetAddress(&IicInstance, XII_ADDR_TO_SEND_TYPE,
			b203No);
	if (Status != XST_SUCCESS) {
		return IIC_FAIL;
	}

	return IIC_SUCCESS;
}

uint8_t iic_select_dev(S_DEV_NUM devNo)
{

	uint8_t m8Result = IIC_FAIL;
	switch(devNo)
	{
	case 	DEV_IIC_SWITCH  :
		m8Result = select_switch();
		break;
	case	DEV_IIC_SI570433A:
		m8Result = select_si570(DEV_IIC_SI570433A);
		break;
	case	DEV_IIC_SI570433B:
		m8Result = select_si570(DEV_IIC_SI570433B);
		break;
	case	DEV_IIC_SI5324	:
		m8Result = select_si5324();
		break;
	case    DEV_IIC_B203A   :
		m8Result = select_b203((uint8_t)IIC_DM100MB203A_ADDRESS);
		break;
	case    DEV_IIC_B203B   :
		m8Result = select_b203((uint8_t)IIC_DM100MB203B_ADDRESS);
		break;
	case    DEV_IIC_B203C   :
		m8Result = select_b203((uint8_t)IIC_DM100MB203C_ADDRESS);
		break;
	case    DEV_IIC_B203D   :
		m8Result = select_b203((uint8_t)IIC_DM100MB203D_ADDRESS);
		break;
	default:
		xil_printf("%s : %d IIC Select a invalid salve address\r\n", __func__ , __LINE__);
		break;
	}

	return m8Result;
}

uint8_t iic_write_data(uint8_t* dataBuf, uint8_t startAddr, uint8_t len)
{
	uint8_t m8loop = 0;
	int Status ;

	if(len > PAGE_SIZE)
	{
		xil_printf("%s : %d data length is to long\r\n");
		return IIC_FAIL;
	}

	WriteBuffer[0] = startAddr;

	//fill the IIC write buffer with control register and valid data
	for(m8loop = 0; m8loop < len; m8loop++)
	{
		WriteBuffer[1 + m8loop] = dataBuf[m8loop];
	}
	//write data
	Status = EepromWriteData(len + 1);
	if (Status != XST_SUCCESS) {
		return IIC_FAIL;
	}

	return IIC_SUCCESS;
}

uint8_t iic_read_data(uint8_t* dataBuf, uint8_t startAddr, uint8_t len)
{
//	EepromReadData2(startAddr, dataBuf, len);

	return IIC_SUCCESS;
}

static int modify(u8 reg0 , u8 reg1)
{
	uint8_t data = 0;

	data = reg0;
	iic_write_data(&data, 0x0e, 1);
	data = reg1;
	iic_write_data(&data, 0x0f, 1);

	data = reg0;
	iic_write_data(&data, 0x15, 1);
	data = reg1;
	iic_write_data(&data, 0x16, 1);

	data = reg0;
	iic_write_data(&data, 0x1c, 1);
	data = reg1;
	iic_write_data(&data, 0x1d, 1);

	data = reg0;
	iic_write_data(&data, 0x23, 1);
	data = reg1;
	iic_write_data(&data, 0x24, 1);

	data = reg0;
	iic_write_data(&data, 0x2b, 1);
	data = reg1;
	iic_write_data(&data, 0x2c, 1);

	data = reg0;
	iic_write_data(&data, 0x39, 1);
	data = reg1;
	iic_write_data(&data, 0x3a, 1);

	data = reg0;
	iic_write_data(&data, 0x41, 1);
	return 0;
}


void config_b203_master()
{
	uint8_t databuf[2] = {0};

	databuf[0] = 0x07;
	iic_write_data(databuf, 0x5e, 1);

	//select backup
	databuf[0] = 0xf0;
	iic_write_data(databuf, 0x5f, 1);

	databuf[0] = 0x01;
	iic_write_data(databuf, 0x02, 1);

	databuf[0] = 0x01;
	iic_write_data(databuf, 0x00, 1);
}

void config_b203_slave()
{
	uint8_t databuf[2] = {0};


	databuf[0] = 0x07;
	iic_write_data(databuf, 0x5e, 1);

	//select backup
	databuf[0] = 0x00;
	iic_write_data(databuf, 0x5f, 1);

	databuf[0] = 0x01;
	iic_write_data(databuf, 0x02, 1);

	databuf[0] = 0x01;
	iic_write_data(databuf, 0x00, 1);

}

void config_b203(uint8_t iicNo)
{
	uint8_t databuf[2] = {0};
	uint8_t rdbuf[0x60] = {0};
	/* read and check the register */
	iic_read_data((uint8_t*)rdbuf, 0, 0x60);

	databuf[0] = 0x07;
	iic_write_data(databuf, 0x5e, 1);

	modify(0x0c, 0x0);

	databuf[0] = 0x08;
	iic_write_data(databuf, 0x08, 1);

	databuf[0] = 0x18;
	iic_write_data(databuf, 0x06, 1);

	databuf[0] = 0x01;
	iic_write_data(databuf, 0x02, 1);

	databuf[0] = 0x01;
	iic_write_data(databuf, 0x00, 1);
//
//	if(iicNo == DEV_IIC_B203A)
//	{
//		databuf[0] = 0xff;
//		iic_write_data(databuf, 0x01, 1);
//	}

	/* read and check the register */
	iic_read_data((uint8_t*)rdbuf, 0, 0x60);

	return ;
}

/*
 * input  : 100MHz
 * output : 156.25 MHz
 * use Programmable Oscillator Calculator tool
 *
 * Registers for the New Configuration
   Register   Data
	  7       0x01
	  8       0xC2
	  9       0xBC
	 10       0x01
	 11       0x1E
	 12       0xB8
 * */
void config_si570(uint8_t mode)
{
	uint8_t databuf[32];

	if(mode == IIC_OUTPUT_CLK_100M)
	{
		return ;
	}
	if(mode == IIC_OUTPUT_CLK_156_25M)
	{
		databuf[0] = 0x01;
		databuf[1] = 0xc2;
		databuf[2] = 0xbc;
		databuf[3] = 0x01;
		databuf[4] = 0x1e;
		databuf[5] = 0xb8;
		iic_write_data((uint8_t*)databuf, 7, 6);

		//update output
		databuf[0] = 0x00;
		iic_write_data((uint8_t*)databuf, 137, 1);

		databuf[0] = 0x40;
		iic_write_data((uint8_t*)databuf, 135, 1);
	}
	return ;
}

/*
 * input  : 156.25MHz
 * output : 156.25 MHz
 * use DSPLLsim tool
 * */
void config_si5324(uint8_t mode)
{
	uint8_t databuf[32];

	//iic_read_data((uint8_t*)databuf, 0, 32);

	//set FREE_RUN_ON CKOUT_ALWAYS_ON BYPASS_REG to 0
	databuf[0] = 0x00;
	iic_write_data((uint8_t*)databuf, 0, 1);

	//set CKSEL_REG DHOLD to 0, SQ_ICAL to 1
	databuf[0] = 0x10;
	iic_write_data((uint8_t*)databuf, 3, 1);
	iic_read_data((uint8_t*)databuf, 3, 1);

	//set BWSEL_REG to 0xa bit[7:4]
	databuf[0] = 0xa0;
	iic_write_data((uint8_t*)databuf, 2, 1);
	iic_read_data((uint8_t*)databuf, 2, 1);

	//AUTOSEL_REG 0x2   bit[7:6]
	//HIST_DEL    0x12  bit[4:0]
	databuf[0] = 0x92;
	iic_write_data((uint8_t*)databuf, 4, 1);
	iic_read_data((uint8_t*)databuf, 4, 1);

	//ICMOS    0x3  bit[7:6]
	databuf[0] = 0xc0;
	iic_write_data((uint8_t*)databuf, 5, 1);
	iic_read_data((uint8_t*)databuf, 5, 1);

	//register 6 default
	//register 7 default
	//register 8 default
	//register 9 default
	//register 10 default
	//register 11 default

	//FLAT_VALID set to 0
	databuf[0] = 0x00;
	iic_write_data((uint8_t*)databuf, 17, 1);
	iic_read_data((uint8_t*)databuf, 17, 1);

	//FOS_EN  0
	//FOS_THR 1
	//VALTIME 1
	//LOCKT   1
	databuf[0] = 0x29;
	iic_write_data((uint8_t*)databuf, 19, 1);
	iic_read_data((uint8_t*)databuf, 19, 1);

	//register 20 default

	//INCDEC_PIN    0
	//CCK1_ACTV_PIN 1
	//CKSEL_PIN     1
	databuf[0] = 0x03;
	iic_write_data((uint8_t*)databuf, 21, 1);
	iic_read_data((uint8_t*)databuf, 21, 1);

	//register 22 default
	//register 23 default
	//register 24 default


	if(mode == IIC_OUTPUT_CLK_100M)//configure si5324 to 100M
	{
		//N1_HS 0x03
		databuf[0] = 0x60;
		iic_write_data((uint8_t*)databuf, 25, 1);
		iic_read_data((uint8_t*)databuf, 25, 1);

		//NC1_LS 0x07
		databuf[0] = 0x00;
		databuf[1] = 0x00;
		databuf[2] = 0x07;
		//NC2_LS 0x07
		databuf[3] = 0x00;
		databuf[4] = 0x00;
		databuf[5] = 0x07;
		iic_write_data((uint8_t*)databuf, 31, 6);
		iic_read_data((uint8_t*)databuf, 31, 6);

		//N2_HS  0x00 bit[7:5]
		//N2_LS 0x2bb
		databuf[0] = 0x00;
		databuf[1] = 0x01;
		databuf[2] = 0xbb;
		//N31  0x31
		databuf[3] = 0x00;
		databuf[4] = 0x00;
		databuf[5] = 0x31;
		//N32  0x31
		databuf[6] = 0x00;
		databuf[7] = 0x00;
		databuf[8] = 0x31;

		iic_write_data((uint8_t*)databuf, 40, 9);
		iic_read_data((uint8_t*)databuf, 40, 9);
	}
	else if(mode == IIC_OUTPUT_CLK_140M)
	{

	}
	else if(mode == IIC_OUTPUT_CLK_156_25M) //configure si5324 to 156.25M
	{
		//N1_HS 0x02
		databuf[0] = 0x40;
		iic_write_data((uint8_t*)databuf, 25, 1);
		iic_read_data((uint8_t*)databuf, 25, 1);

		//NC1_LS 0x05
		databuf[0] = 0x00;
		databuf[1] = 0x00;
		databuf[2] = 0x05;
		//NC2_LS 0x05
		databuf[3] = 0x00;
		databuf[4] = 0x00;
		databuf[5] = 0x05;
		iic_write_data((uint8_t*)databuf, 31, 6);
		iic_read_data((uint8_t*)databuf, 31, 6);

		//N2_HS  0x05 bit[7:5]
		//N2_LS 0x13b
		databuf[0] = 0xa0;
		databuf[1] = 0x01;
		databuf[2] = 0x3b;
		//N31  0x4e
		databuf[3] = 0x00;
		databuf[4] = 0x00;
		databuf[5] = 0x4e;
		//N32  0x4e
		databuf[6] = 0x00;
		databuf[7] = 0x00;
		databuf[8] = 0x4e;

		iic_write_data((uint8_t*)databuf, 40, 9);
		iic_read_data((uint8_t*)databuf, 40, 9);
	}

	//register 55 default
	//register 128 default
	//register 129 default
	//register 130 default
	//register 131 default
	//register 132 default

	//PARTNUM_RO 0xb4  134 bit[7:0] 135 bit[7:4]
	//REVID_RO   0x00  135 bit[3:0]
	databuf[0] = 0x0b;
	databuf[1] = 0x40;
	//RST_REG    0     136 bit[7]
	//ICAL       1     136 bit[6]
	databuf[2] = 0x40;
	//FASTLOCK   1     137 bit[0]
	databuf[3] = 0x01;
	// LOS1_EN   0x03  138 bit[0] 139 bit[4]
	// LOS2_EN   0x03  138 bit[1] 139 bit[5]
	databuf[4] = 0x03;
	//FOS1_EN    1
	//FOS2_EN    1
	databuf[5] = 0xff;
	iic_write_data((uint8_t*)databuf, 134, 6);
	iic_read_data((uint8_t*)databuf, 134, 6);

	//register 142 default
	//register 143 default

	return ;
}

/* platform device iic operation functions implementation */

int dev_iic_init(S_DEV_INFO * dev)
{
	int ret = IIC_SUCCESS;

	ret = XIic_Initialize(&IicInstance, IIC_DEVICE_ID);
	if (ret != XST_SUCCESS) {
		return ret;
	}

	//parameter check
	if(dev->dev_conf->dev_id == DEV_NUM_INVALID)
	{
		MST_IO_MSG( "%s: IIC initial fail, device No %d\n", __func__, dev->dev_conf->dev_id);
		return IIC_FAIL;
	}

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	ret = XIntc_Connect(&InterruptController, dev->dev_conf->intr_num,
				   (XInterruptHandler) XIic_InterruptHandler,
				   &IicInstance);
	if (ret != XST_SUCCESS) {
		return IIC_FAIL;
	}

	/*
	 * Enable the interrupt for the CAN device.
	 */
	XIntc_Enable(&InterruptController, dev->dev_conf->intr_num);

	XIic_SetSendHandler(&IicInstance, &IicInstance,
			(XIic_Handler) SendHandler);
	XIic_SetRecvHandler(&IicInstance, &IicInstance,
			(XIic_Handler) ReceiveHandler);
	XIic_SetStatusHandler(&IicInstance, &IicInstance,
			(XIic_StatusHandler) StatusHandler);


	return ret;
}

int dev_iic_write(S_DEV_INFO * dev)
{
	int ret = IIC_SUCCESS;

	//parameter check
	if(dev->dev_conf->dev_id == DEV_NUM_INVALID)
	{
		MST_IO_MSG( "%s: IIC Write fail, device No %d\n", __func__, dev->dev_conf->dev_id);
		return IIC_FAIL;
	}

	if(dev->dev_iic_arg->src == NULL)
	{
		MST_IO_MSG( "%s: IIC Write Error, source data is NULL\n", __func__, dev->dev_conf->dev_id);
		return IIC_FAIL;
	}

	if(dev->dev_iic_arg->iicNo != DEV_INVALID)
	{
		//select target IIC device
		iic_select_dev(dev->dev_iic_arg->iicNo);
		iic_write_data(dev->dev_iic_arg->src, dev->dev_iic_arg->addr, dev->dev_iic_arg->len); // write data
	}

	return ret;
}

int dev_iic_read(S_DEV_INFO * dev)
{
	int ret = IIC_SUCCESS;

	//parameter check
	if(dev->dev_conf->dev_id == DEV_NUM_INVALID)
	{
		MST_IO_MSG( "%s: IIC Read fail, device No %d\n", __func__, dev->dev_conf->dev_id);
		return IIC_FAIL;
	}

	if(dev->dev_iic_arg->iicNo != DEV_INVALID)
	{
		//select target IIC device
		iic_select_dev(dev->dev_iic_arg->iicNo);
		iic_read_data(dev->dev_iic_arg->src, dev->dev_iic_arg->addr, dev->dev_iic_arg->len); // read data
	}

	return ret;
}

int dev_iic_ctrl(S_DEV_INFO * dev)
{
	int ret = IIC_SUCCESS;

	//parameter check
	if(dev->dev_conf->dev_id == DEV_NUM_INVALID)
	{
		MST_IO_MSG( "%s: IIC Read fail, device No %d\n", __func__, dev->dev_conf->dev_id);
		return IIC_FAIL;
	}

	if(dev->dev_iic_arg->iicNo != DEV_INVALID)
	{
		if(dev->dev_iic_arg->src == NULL)
		{
			MST_IO_MSG( "%s: IIC Ctrl fail, device No %d\n", __func__, dev->dev_conf->dev_id);
			return IIC_FAIL;
		}
		else
		{
			//select target IIC device
			iic_select_dev(dev->dev_iic_arg->iicNo);
			switch(dev->dev_iic_arg->src[0])
			{
			case IIC_CTRL_PARM_CONFIG:
				switch(dev->dev_iic_arg->iicNo)
				{
				case DEV_IIC_SI570433A :
				case DEV_IIC_SI570433B :
					config_si570(dev->dev_iic_arg->src[1]);
					break;
				case DEV_IIC_SI5324	   :
					config_si5324(dev->dev_iic_arg->src[1]);
					break;
				case DEV_IIC_B203A     :
				case DEV_IIC_B203B     :
				case DEV_IIC_B203C     :
				case DEV_IIC_B203D     :
					config_b203(dev->dev_iic_arg->iicNo);
					break;
				default:
					break;
				}
				break;
			case IIC_CTRL_MASTER_SELECT:
				config_b203_master();
				break;
			case IIC_CTRL_SLAVE_SELECT:
				config_b203_slave();
				break;
			default:
				break;
			}
		}
	}

	return ret;
}
/* platform device IIC operation functions implementation */
S_DEV_IIC_OPS platform_dev_iic_ops = {
		.init = dev_iic_init,
		.write = dev_iic_write,
		.read = dev_iic_read,
		.ctrl = dev_iic_ctrl
};
#endif /* DRIVER_ENABLE_IIC */
