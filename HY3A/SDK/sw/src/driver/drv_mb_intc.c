/*
 * intc.c
 *
 *  Created on: 2018Äê9ÔÂ13ÈÕ
 *      Author: 508
 */

#include "driver.h"


#if 0
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
#endif

//static void s2mm_error_intr(void * InstancePtr)
//{
//	RESET_DATA_PATH();
//}

int ctrl_intc(uint8_t* arg)
{
	int Status = 0;
	switch(arg[0])
	{
	case CTRL_INTC_START:
#if 0


		Status = XIntc_Connect(&InterruptController, XPAR_CPU_AXI_INTC_0_B1_B2_PATH_AXI_DATAMOVER_0_S2MM_ERR_INTR,
				(XInterruptHandler)s2mm_error_intr,
				(void *)NULL);
		if (Status != XST_SUCCESS)
		{
			return SECOND_PULSE_FAIL;
		}

		XIntc_Enable(&InterruptController, XPAR_CPU_AXI_INTC_0_B1_B2_PATH_AXI_DATAMOVER_0_S2MM_ERR_INTR);
#endif
		/*
		 * Start the interrupt controller so interrupts are enabled for all
		 * devices that cause interrupts.
		 */
		Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Initialize the exception table.
		 */
		Xil_ExceptionInit();

		/*
		 * Register the interrupt controller handler with the exception table.
		 */
		Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				 (Xil_ExceptionHandler)XIntc_InterruptHandler,
				 &InterruptController);

		/*
		 * Enable exceptions.
		 */
		Xil_ExceptionEnable();
		break;
	case CTRL_INTC_STOP:
		break;
	case CTRL_INTC_DISABLE:
		break;
	default:
		break;
	}

	return 0;
}

uint8_t init_intc(void)
{
	int Status;

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	Status = XIntc_Initialize(&InterruptController, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return INTC_FAIL;
	}

#if 0
	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device.
	 */
	Status = XIntc_Connect(&InterruptController, DEV_CAN_A_INTR_ID,
			   (XInterruptHandler)can_intr_a_handler,
			   (void *)NULL);
	if (Status != XST_SUCCESS) {
		return INTC_FAIL;
	}

#ifdef __ENABLE_CAN_B__
	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the
	 * specific interrupt processing for the device.
	 */
	Status = XIntc_Connect(&InterruptController, DEV_CAN_B_INTR_ID,
			   (XInterruptHandler)can_intr_b_handler,
			   (void *)NULL);
	if (Status != XST_SUCCESS) {
		return INTC_FAIL;
	}

#endif
	/*
	 * Start the interrupt controller such that interrupts are enabled for
	 * all devices that cause interrupts, specific real mode so that
	 * the UartLite can cause interrupts through the interrupt controller.
	 */
	Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return INTC_FAIL;
	}

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XIntc_Connect(&InterruptController, IIC_INTR_ID,
				   (XInterruptHandler) XIic_InterruptHandler,
				   &IicInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Start the interrupt controller so interrupts are enabled for all
	 * devices that cause interrupts.
	 */
	Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Enable the interrupt for the UartLite device.
	 */
	XIntc_Enable(&InterruptController, DEV_CAN_A_INTR_ID);
#ifdef __ENABLE_CAN_B__
	XIntc_Enable(&InterruptController, DEV_CAN_B_INTR_ID);
#endif
	XIntc_Enable(&InterruptController, IIC_INTR_ID);

	/*
	 * Initialize the exception table.
	 */
	Xil_ExceptionInit();

	/*
	 * Register the interrupt controller handler with the exception table.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 (Xil_ExceptionHandler)XIntc_InterruptHandler,
			 &InterruptController);

	/*
	 * Enable exceptions.
	 */
	Xil_ExceptionEnable();

	XIic_SetSendHandler(&IicInstance, &IicInstance,
			(XIic_Handler) SendHandler);
	XIic_SetRecvHandler(&IicInstance, &IicInstance,
			(XIic_Handler) ReceiveHandler);
	XIic_SetStatusHandler(&IicInstance, &IicInstance,
			(XIic_StatusHandler) StatusHandler);

#endif
	return INTC_SUCCESS;
}

