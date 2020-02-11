/*
 * driver.c
 *
 *  Created on: 2018Äê10ÔÂ22ÈÕ
 *      Author: 508
 */


#include "driver.h"

/*****************************************************************************
 *
 *  Function Implementation Section
 * Add prototypes for all functions called by this
 * module, with the exception of runtime routines.
 *
 *****************************************************************************/

/*****************************************************************************
 * Lookup the device configuration based on the unique device ID.  The table
 * ConfigTable contains the configuration info for each device in the system.
 *
 * @param	Device pointer include DeviceId
 * 		deivceId is the device identifier to lookup.
 *
 * @return
 * 		void
 *
 * @note		None.
 ******************************************************************************/
S_DEV_CONF* platformDevLookupConfig(uint8_t devNo)
{
    uint32_t loop_device = 0;
    /* Lookup the device configuration based on the unique device ID */
    for(loop_device = 0; loop_device < MAX_DEV_NUM; loop_device++)
    {
        if(gDevConfigTable[loop_device].dev_id == devNo)
        {
            return &gDevConfigTable[loop_device];
        }
    }

    return NULL; // if don't search the meet configuration, return NULL
}

#ifdef DRIVER_ENABLE_CAN
/***************************** CAN Function Implementation *************************************/
/* initial can device */
static int can_init(uint8_t devNo)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	platformDev->dev_info[devNo]->dev_conf = platformDevLookupConfig(devNo);

	//if can't search device configuration
	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Open device fail\n", __func__, devNo);
		return IO_OPEN_FAIL;
	}

	//call can device initial function
	if(platformDev->can_ops->init != NULL)
	{
		platformDev->can_ops->init(platformDev->dev_info[devNo]);
	}
	else
	{
		ret = IO_OPEN_FAIL;
		MST_IO_MSG( "%s: Device Can OPS are not declared. Open device fail\r\n", __func__ );
	}
	return ret;
}

static int can_transfer(uint8_t devNo, uint8_t * arg, uint32_t len)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Please Open Device\n", __func__, devNo);
		return IO_WRITE_FAIL;
	}

	if(platformDev->can_ops->write != NULL)
	{
		//copy source data to driver layer
		platformDev->dev_info[devNo]->dev_can_arg->src = arg;
		platformDev->dev_info[devNo]->dev_can_arg->len = len;

		platformDev->can_ops->write(platformDev->dev_info[devNo]);
	}
	else
	{
		ret = IO_WRITE_FAIL;
		MST_IO_MSG( "%s: Device Can OPS are not declared. Write device fail\r\n", __func__ );
	}

	return ret;
}
/*************************** End CAN Function Implementation ***********************************/
#endif /* DRIVER_ENABLE_CAN */
/***************************** INTR Function Implementation *************************************/
static int intr_init(uint8_t devNo)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	platformDev->dev_info[devNo]->dev_conf = platformDevLookupConfig(devNo);

	//if can't search device configuration
	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Open device fail\n", __func__, devNo);
		return IO_OPEN_FAIL;
	}

	ret = init_intc();

	return ret;
}

static int intr_ctrl(uint8_t* arg)
{
	int ret = IO_STS_SUCCESS;

	//if can't search device configuration
	if(arg == NULL)
	{
		return IO_OPEN_FAIL;
	}

	ret = ctrl_intc(arg);

	return ret;
}
/*************************** End INTR Function Implementation ***********************************/

#ifdef DRIVER_ENABLE_SECOND_PULSE
/*************************** Second Pulse Function Implementation *******************************/
static int second_pulse_init(uint8_t devNo)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	platformDev->dev_info[devNo]->dev_conf = platformDevLookupConfig(devNo);

	//if can't search device configuration
	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Open device fail\n", __func__, devNo);
		return IO_OPEN_FAIL;
	}

	ret = init_sec_pulse(platformDev->dev_info[devNo]);

	return ret;
}

static int second_pulse_ctrl(uint8_t devNo, uint8_t *arg)
{
	S_DEV_INFO * dev_info = gPlatformDev.dev_info[devNo];
	uint32_t     m32data  = 0;

	switch(arg[0])
	{
	case SEC_PULSE_CMD_SET_PERIOD:
		m32data =                arg[1];
		m32data = m32data << 8 | arg[2];
		m32data = m32data << 8 | arg[3];
		m32data = m32data << 8 | arg[4];

		set_sec_pulse_period(dev_info->dev_conf->base_addr, m32data);
		break;
	case SEC_PULSE_CMD_SET_WIDTH:
		m32data =                arg[1];
		m32data = m32data << 8 | arg[2];
		m32data = m32data << 8 | arg[3];
		m32data = m32data << 8 | arg[4];

		set_sec_pulse_width(dev_info->dev_conf->base_addr, m32data);
		break;
	case SEC_PULSE_CMD_START:
		start_sec_pulse(dev_info->dev_conf->base_addr);
		break;
	case SEC_PULSE_CMD_STOP:
		stop_sec_pulse(dev_info->dev_conf->base_addr);
		break;
	default:
		break;
	}

	return 0;
}

/************************* End Second Pulse Function Implementation *****************************/
#endif /* DRIVER_ENABLE_SECOND_PULSE */


#ifdef DRIVER_ENABLE_FSYNC
/*************************** Second Pulse Function Implementation *******************************/
static int fsync_init(uint8_t devNo)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	platformDev->dev_info[devNo]->dev_conf = platformDevLookupConfig(devNo);

	//if can't search device configuration
	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Open device fail\n", __func__, devNo);
		return IO_OPEN_FAIL;
	}

	ret = init_fsyn(platformDev->dev_info[devNo]);

	return ret;
}

static int fsync_ctrl(uint8_t devNo, uint8_t *arg)
{
	S_DEV_INFO * dev_info = gPlatformDev.dev_info[devNo];
	uint32_t     m32data  = 0;

	switch(arg[0])
	{
	case FSYN_CMD_SET_PERIOD:
		m32data =                arg[1];
		m32data = m32data << 8 | arg[2];
		m32data = m32data << 8 | arg[3];
		m32data = m32data << 8 | arg[4];

		set_fsync_period(dev_info->dev_conf->base_addr, m32data);
		break;
	case FSYN_CMD_SET_WIDTH:
		m32data =                arg[1];
		m32data = m32data << 8 | arg[2];
		m32data = m32data << 8 | arg[3];
		m32data = m32data << 8 | arg[4];

		set_fsync_width(dev_info->dev_conf->base_addr, m32data);
		break;
	case FSYN_CMD_START:
		start_fsync(dev_info->dev_conf->base_addr);
		break;
	case FSYN_CMD_STOP:
		stop_fsync(dev_info->dev_conf->base_addr);
		break;
	default:
		break;
	}

	return 0;
}

/************************* End Second Pulse Function Implementation *****************************/
#endif /* DRIVER_ENABLE_FSYNC */


#ifdef DRIVER_ENABLE_IIC

/***************************** IIC Function Implementation *************************************/
static int iic_init(uint8_t devNo)
{
	int ret = IO_STS_SUCCESS;
	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	//search device configuration table
	platformDev->dev_info[devNo]->dev_conf = platformDevLookupConfig(devNo);

	//if can't search device configuration
	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Open device fail\n", __func__, devNo);
		return IO_OPEN_FAIL;
	}

	//call can device initial function
	if(platformDev->iic_ops->init != NULL)
	{
		platformDev->iic_ops->init(platformDev->dev_info[devNo]);
	}
	else
	{
		ret = IO_OPEN_FAIL;
		MST_IO_MSG( "%s: Device Can OPS are not declared. Open device fail\r\n", __func__ );
	}

	return ret;
}

static int iic_ctrl(uint8_t devNo, uint8_t *arg)
{
	S_DEV_INFO * dev_info = gPlatformDev.dev_info[devNo];

	//if can't search device configuration
	if(dev_info->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Ctrl device fail\n", __func__, devNo);
		return IO_OPEN_FAIL;
	}

	//call can device initial function
	if(dev_info->platform_dev->iic_ops->ctrl != NULL)
	{
		dev_info->dev_iic_arg = (S_DEV_IIC_ARG * )arg;
		dev_info->platform_dev->iic_ops->ctrl(dev_info);
	}
	else
	{
		MST_IO_MSG( "%s: Device Can OPS are not declared. Open device fail\r\n", __func__ );
		return IO_OPEN_FAIL;
	}


	return 0;
}
/*************************** End IIC Function Implementation ***********************************/
#endif /* DRIVER_ENABLE_IIC */

/***************************** DMA Function Implementation *************************************/
static int dma_init(uint8_t devNo)
{
	int ret = IO_STS_SUCCESS;
	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	//search device configuration table
	platformDev->dev_info[devNo]->dev_conf = platformDevLookupConfig(devNo);

	//if can't search device configuration
	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Open device fail\n", __func__, devNo);
		return IO_OPEN_FAIL;
	}

	return ret;
}
/*************************** End DMA Function Implementation ***********************************/

#ifdef DRIVER_ENABLE_GPIO
/***************************** GPIO Function Implementation *************************************/
static int gpio_init(uint8_t devNo)
{
	int ret = IO_STS_SUCCESS;
	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	//search device configuration table
	platformDev->dev_info[devNo]->dev_conf = platformDevLookupConfig(devNo);

	//if can't search device configuration
	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Open device fail\n", __func__, devNo);
		return IO_OPEN_FAIL;
	}

	ret = init_gpio(platformDev->dev_info[devNo]);

	return ret;
}

static int gpio_write(uint8_t devNo, uint8_t* arg)
{
	int ret = IO_STS_SUCCESS;
	uint32_t m32val = 0;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Please Open Device\n", __func__, devNo);
		return IO_WRITE_FAIL;
	}

	m32val =               arg[0];
	m32val = m32val << 8 | arg[1];
	m32val = m32val << 8 | arg[2];
	m32val = m32val << 8 | arg[3];

	write_gpio(platformDev->dev_info[devNo]->dev_conf->base_addr, m32val);

	return ret;
}

static int gpio_read(uint8_t devNo, uint8_t* arg)
{
	int ret = IO_STS_SUCCESS;
	uint32_t m32val = 0;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Please Open Device\n", __func__, devNo);
		return IO_WRITE_FAIL;
	}

	m32val = read_gpio(platformDev->dev_info[devNo]->dev_conf->base_addr);

	arg[0] = (m32val >> 24) & 0xff;
	arg[1] = (m32val >> 16) & 0xff;
	arg[2] = (m32val >>  8) & 0xff;
	arg[3] = (m32val      ) & 0xff;

	return ret;
}

static int gpio_ctrl(uint8_t devNo, uint8_t * arg)
{
	int ret = IO_STS_SUCCESS;
	uint32_t m32dir = 0;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	switch(arg[0])
	{
	case GPIO_CMD_SET_DIR:
		m32dir =               arg[1];
		m32dir = m32dir << 8 | arg[2];
		m32dir = m32dir << 8 | arg[3];
		m32dir = m32dir << 8 | arg[4];
		set_dir_gpio(platformDev->dev_info[devNo]->dev_conf->base_addr, m32dir);
		break;
	default:
		break;
	}
	return ret;
}

/*************************** End GPIO Function Implementation ***********************************/
#endif /* DRIVER_ENABLE_GPIO */


#ifdef DRIVER_ENABLE_SPI
/***************************** SPI Function Implementation *************************************/
static int spi_init(uint8_t devNo)
{
	int ret = IO_STS_SUCCESS;
	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	//search device configuration table
	platformDev->dev_info[devNo]->dev_conf = platformDevLookupConfig(devNo);

	//if can't search device configuration
	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Open device fail\n", __func__, devNo);
		return IO_OPEN_FAIL;
	}

	if(platformDev->spi_ops->init != NULL)
	{
		platformDev->spi_ops->init(platformDev->dev_info[devNo]);
	}

	return ret;
}


static int spi_write(uint8_t devNo, uint8_t* arg, uint32_t len)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Please Open Device\n", __func__, devNo);
		return IO_WRITE_FAIL;
	}

	if(platformDev->spi_ops->write != NULL)
	{
		platformDev->dev_info[devNo]->dev_spi_arg->src = arg;
		platformDev->dev_info[devNo]->dev_spi_arg->len = len;

		platformDev->spi_ops->write(platformDev->dev_info[devNo]);
	}

	return ret;
}

static int spi_read(uint8_t devNo, uint8_t* arg, uint32_t len)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Please Open Device\n", __func__, devNo);
		return IO_WRITE_FAIL;
	}

	if(platformDev->spi_ops->read != NULL)
	{
		platformDev->dev_info[devNo]->dev_spi_arg->len = len;

		platformDev->dev_info[devNo]->dev_spi_arg->src = arg;

		platformDev->spi_ops->read(platformDev->dev_info[devNo]);

	}

	return ret;
}

/*************************** End SPI Function Implementation ***********************************/
#endif /* DRIVER_ENABLE_SPI */



#ifdef DRIVER_ENABLE_SPI_NBYTE
/***************************** SPI Function Implementation *************************************/
static int spi_nbyte_init(uint8_t devNo)
{
	int ret = IO_STS_SUCCESS;
	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	//search device configuration table
	platformDev->dev_info[devNo]->dev_conf = platformDevLookupConfig(devNo);

	//if can't search device configuration
	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Open device fail\n", __func__, devNo);
		return IO_OPEN_FAIL;
	}

	ret = init_spi_nbyte(platformDev->dev_info[devNo]->dev_conf->base_addr);

	return ret;
}


static int spi_nbyte_write(uint8_t devNo, uint8_t* arg, uint32_t len)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Please Open Device\n", __func__, devNo);
		return IO_WRITE_FAIL;
	}

	if(arg != NULL && len > 0)
	{
		write_spi_nbyte(platformDev->dev_info[devNo]->dev_conf->base_addr, arg, len);
	}


	return ret;
}

static int spi_nbyte_read(uint8_t devNo, uint8_t* arg, uint32_t len)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Please Open Device\n", __func__, devNo);
		return IO_WRITE_FAIL;
	}

	if(arg != NULL && len > 0)
	{
		read_spi_nbyte(platformDev->dev_info[devNo]->dev_conf->base_addr, arg, len);
	}

	return ret;
}

/*************************** End SPI Function Implementation ***********************************/
#endif /* DRIVER_ENABLE_SPI_NBYTE */


#ifdef DRIVER_ENABLE_AXI_SWITCH

static int axi_switch_init(uint8_t devNo)
{
	int ret = IO_STS_SUCCESS;
	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	//search device configuration table
	platformDev->dev_info[devNo]->dev_conf = platformDevLookupConfig(devNo);

	//if can't search device configuration
	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Open device fail\n", __func__, devNo);
		return IO_OPEN_FAIL;
	}

	ret = init_axi_switch(platformDev->dev_info[devNo]);

	return ret;
}

static int axi_switch_write(uint8_t devNo, uint8_t* arg, uint32_t len)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Please Open Device\n", __func__, devNo);
		return IO_WRITE_FAIL;
	}

	if(arg != NULL)
	{
		write_axi_switch(platformDev->dev_info[devNo]->dev_conf->base_addr, arg, len);
	}

	return ret;
}

static int axi_switch_read(uint8_t devNo, uint8_t* arg, uint32_t len)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Please Open Device\n", __func__, devNo);
		return IO_WRITE_FAIL;
	}

	if(arg != NULL)
	{
		read_axi_switch(platformDev->dev_info[devNo]->dev_conf->base_addr, arg, len);
	}

	return ret;
}

static int axi_switch_ctrl(uint8_t devNo, uint8_t* arg)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Please Open Device\n", __func__, devNo);
		return IO_WRITE_FAIL;
	}

	if(arg != NULL)
	{
		ctrl_axi_switch(platformDev->dev_info[devNo]->dev_conf->base_addr, arg);
	}

	return ret;
}

#endif /* DRIVER_ENABLE_AXI_SWITCH */


#ifdef DRIVER_ENABLE_UART
/***************************** UART Function Implementation *************************************/
static int uart_init(uint8_t devNo)
{
	int ret = IO_STS_SUCCESS;
	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	//search device configuration table
	platformDev->dev_info[devNo]->dev_conf = platformDevLookupConfig(devNo);

	//if can't search device configuration
	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Open device fail\n", __func__, devNo);
		return IO_OPEN_FAIL;
	}

	//initial UART engine
	ret = dev_uart_init(platformDev->dev_info[devNo]);

	return ret;
}

static int uart_transfer(uint8_t devNo, uint8_t* arg, uint32_t len)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Please Open Device\n", __func__, devNo);
		return IO_WRITE_FAIL;
	}

	platformDev->dev_info[devNo]->dev_uart_arg->src = arg;
	platformDev->dev_info[devNo]->dev_uart_arg->len = len;

	dev_uart_transfer(platformDev->dev_info[devNo]);
	return ret;
}
/*************************** End UART Function Implementation ***********************************/
#endif /* DRIVER_ENABLE_UART */

#ifdef DRIVER_ENABLE_UART_2M
static int uart_highspeed_init(uint8_t devNo)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	//search device configuration table
	platformDev->dev_info[devNo]->dev_conf = platformDevLookupConfig(devNo);

	//if can't search device configuration
	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Open device fail\n", __func__, devNo);
		return IO_OPEN_FAIL;
	}

	//initial UART engine
	ret = dev_uart_highspeed_init(platformDev->dev_info[devNo]);

	return ret;
}

static int uart_highspeed_transfer(uint8_t devNo, uint8_t* arg, uint32_t len)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Please Open Device\n", __func__, devNo);
		return IO_WRITE_FAIL;
	}

	platformDev->dev_info[devNo]->dev_uart_arg->src = arg;
	platformDev->dev_info[devNo]->dev_uart_arg->len = len;

	dev_uart_high_speed_send(platformDev->dev_info[devNo]);
	return ret;
}

#endif /* DRIVER_ENABLE_UART_2M */

#ifdef DRIVER_ENABLE_TIMER
/***************************** TIMER Function Implementation *************************************/
static int timer_init(uint8_t devNo)
{
	int ret = IO_STS_SUCCESS;
	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	//search device configuration table
	platformDev->dev_info[devNo]->dev_conf = platformDevLookupConfig(devNo);

	//if can't search device configuration
	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Open device fail\n", __func__, devNo);
		return IO_OPEN_FAIL;
	}

	ret = init_timer(platformDev->dev_info[devNo]);

	return ret;
}

static int timer_read(uint8_t devNo, uint8_t* arg, uint32_t len)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Please Open Device\n", __func__, devNo);
		return IO_WRITE_FAIL;
	}

	platformDev->dev_info[devNo]->dev_com_arg->len = len;

	platformDev->dev_info[devNo]->dev_com_arg->src = arg;

	read_timer(platformDev->dev_info[devNo]);

	return ret;
}

static int timer_ctrl(uint8_t devNo, uint8_t* arg)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Please Open Device\n", __func__, devNo);
		return IO_WRITE_FAIL;
	}

	ctrl_timer(devNo, arg);

	return ret;
}

/*************************** End TIMER Function Implementation ***********************************/
#endif /* DRIVER_ENABLE_TIMER */

#ifdef DRIVER_ENABLE_USER_IP
/***************************** USER Define IP Function Implementation *************************************/
static int user_ip_init(uint8_t devNo)
{
	int ret = IO_STS_SUCCESS;
	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	//search device configuration table
	platformDev->dev_info[devNo]->dev_conf = platformDevLookupConfig(devNo);

	//if can't search device configuration
	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Open device fail\n", __func__, devNo);
		return IO_OPEN_FAIL;
	}

	ret = init_user_ip(platformDev->dev_info[devNo]);

	return ret;
}

static int user_ip_write(uint8_t devNo, uint8_t* arg, uint32_t len)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Please Open Device\n", __func__, devNo);
		return IO_WRITE_FAIL;
	}

	if(arg != NULL)
	{
		write_user_ip(platformDev->dev_info[devNo]->dev_conf->base_addr, arg, len);
	}

	return ret;
}

static int user_ip_read(uint8_t devNo, uint8_t* arg, uint32_t len)
{
	int ret = IO_STS_SUCCESS;

	S_PLATFORM_DEV * platformDev = &gPlatformDev;

	if(platformDev->dev_info[devNo]->dev_conf == NULL)
	{
		MST_IO_MSG( "%s: Can't found device configuration, device No %d, Please Open Device\n", __func__, devNo);
		return IO_WRITE_FAIL;
	}

	if(arg != NULL)
	{
		read_user_ip(platformDev->dev_info[devNo]->dev_conf->base_addr, arg, len);
	}

	return ret;
}
/*************************** End USER Define IP Function Implementation ***********************************/
#endif /* DRIVER_ENABLE_USER_IP */

/***************************** IODEV Layer Function Implementation ********************************/
/*
 * Function : ioDevOpen
 * 		open device and initial it
 * Parameter :
 * 		devNo : device ID
 * 		arg   : parameter will send to driver layer
 * 		len   : parameter's valid length
 * Return :
 * 		function execute status
 * */
int ioDevOpen(uint8_t devNo)
{
	int ret = IO_STS_SUCCESS;

	//check if a invalid device number
    if ( devNo >= MAX_DEV_NUM )
    {
        MST_IO_MSG( "%s: Device No. is out of range. Open device fail\n", __func__ );
        return IO_INVALID_PARA;
    }

    switch(devNo)
    {
    case DEV_NUM_CAN_A:	    	  // CAN A device ID
    case DEV_NUM_CAN_B:           // CAN B device ID
#ifdef DRIVER_ENABLE_CAN
    	ret = can_init(devNo);
#endif /* DRIVER_ENABLE_CAN */
    	break;
    case DEV_NUM_UART_A:          // UART port receive command
    case DEV_NUM_UART_B:          // UART port receive high speed data
#ifdef DRIVER_ENABLE_UART
    	ret = uart_init(devNo);
#endif /* DRIVER_ENABLE_UART */
    	break;
    case DEV_NUM_TIMER:           // timer device ID
#ifdef DRIVER_ENABLE_TIMER
    	ret = timer_init(devNo);
#endif /* DRIVER_ENABLE_TIMER */
    	break;
    case DEV_NUM_FSYNC :
    case DEV_NUM_FSYNC_1:        // fsyn module for data out
	case DEV_NUM_FSYNC_2:        // fsyn module for data out
#ifdef DRIVER_ENABLE_FSYNC
    	ret = fsync_init(devNo);
#endif /*DRIVER_ENABLE_FSYNC*/
    	break;
    case DEV_NUM_SEC_PULSE:       // second pulse device ID
#ifdef DRIVER_ENABLE_SECOND_PULSE
    	ret = second_pulse_init(devNo);
#endif /*DRIVER_ENABLE_SECOND_PULSE */
    	break;
    case DEV_NUM_ETH:			 // Ethernet device ID
    	break;
    case DEV_NUM_DMA:             // DMA device ID
    	ret = dma_init(devNo);
    	break;
    case DEV_NUM_IIC:             // IIC controller device ID
#ifdef DRIVER_ENABLE_IIC
    	ret = iic_init(devNo);
#endif /* DRIVER_ENABLE_IIC */
    	break;
    case DEV_NUM_INTR:            // interrupt controller device ID
    	ret = intr_init(devNo);
    	break;
    case DEV_NUM_SPI_SEND:
    case DEV_NUM_SPI_RECV:
#ifdef DRIVER_ENABLE_SPI
    	ret = spi_init(devNo);
#endif /* DRIVER_ENABLE_SPI */
    	break;
    case DEV_NUM_SPI_NBYTE_SEND:
    case DEV_NUM_SPI_NBYTE_RECV:
#ifdef DRIVER_ENABLE_SPI_NBYTE
    	ret = spi_nbyte_init(devNo);
#endif /* DRIVER_ENABLE_SPI_NBYTE */
    	break;
    case DEV_NUM_AXI_SWITCH:
#ifdef DRIVER_ENABLE_AXI_SWITCH
    	ret = axi_switch_init(devNo);
#endif /* DRIVER_ENABLE_AXI_SWITCH */
    	break;
    case DEV_NUM_GPIO_0:          // GPIO Device initial
    case DEV_NUM_GPIO_1:
#ifdef DRIVER_ENABLE_GPIO
    	ret = gpio_init(devNo);
#endif /* DRIVER_ENABLE_SPI */
    	break;
    case DEV_NUM_UART_1M_MST2MST:   // uart 1M master 2 master channel
    case DEV_NUM_UART_1M_MST2SLV:   // uart 1M master 2 slave channel
    case DEV_NUM_UART_1M_SLV2MST:   // uart 1M slave 2 master channel
    case DEV_NUM_UART_1M_SLV2SLV:   // uart 1M slave 3 slave channel
    case DEV_NUM_UART_2M_MST:   // uart 2M master send device
    	ret = uart_highspeed_init(devNo);
#ifdef DRIVER_ENABLE_UART_2M
#endif /* DRIVER_ENABLE_UART_2M */
    	break;
    case DEV_NUM_USER_0:          // user define device0 ID
    case DEV_NUM_USER_1:          // user define device1 ID
    case DEV_NUM_USER_2:          // user define device2 ID
    case DEV_NUM_USER_3:          // user define device3 ID
#ifdef DRIVER_ENABLE_USER_IP
    	ret = user_ip_init(devNo);
#endif /* DRIVER_ENABLE_USER_IP */
		break;
	case DEV_NUM_INVALID:         // invalid device identification
		break;
    default:
    	break;
    }

	return ret;
}

/*
 * Function : ioDevWrite
 * 		write message to device
 * Parameter :
 * 		devNo : device ID
 * 		arg   : parameter will send to driver layer
 * 		len   : parameter's valid length
 * Return :
 * 		function execute status
 * */
int ioDevWrite(uint8_t devNo, uint8_t * arg, uint32_t len)
{
	int ret = IO_STS_SUCCESS;

    switch(devNo)
    {
    case DEV_NUM_CAN_A:	     	 // CAN A device ID
    case DEV_NUM_CAN_B:           // CAN B device ID
#ifdef DRIVER_ENABLE_CAN
    	ret = can_transfer(devNo, arg, len);
#endif /* DRIVER_ENABLE_CAN */
    	break;
    case DEV_NUM_UART_A:          // UART port receive command
    case DEV_NUM_UART_B:          // UART port receive high speed data
#ifdef DRIVER_ENABLE_UART
    	ret = uart_transfer(devNo, arg, len);
#endif /* DRIVER_ENABLE_UART */
    	break;
    case DEV_NUM_TIMER:           // timer device ID
    	break;
    case DEV_NUM_SEC_PULSE:       // second pulse device ID
    	break;
    case DEV_NUM_ETH:			 // Ethernet device ID
    	break;
    case DEV_NUM_DMA:             // DMA device ID
    	break;
    case DEV_NUM_IIC:             // IIC controller device ID
    	break;
    case DEV_NUM_INTR:            // interrupt controller device ID
    	break;
    case DEV_NUM_SPI_SEND:
#ifdef DRIVER_ENABLE_SPI
		ret = spi_write(devNo, arg, len);
#endif /* DRIVER_ENABLE_SPI */
		break;
	case DEV_NUM_SPI_RECV:
		break;
    case DEV_NUM_SPI_NBYTE_SEND:
#ifdef DRIVER_ENABLE_SPI_NBYTE
    	ret = spi_nbyte_write(devNo, arg, len);
#endif /* DRIVER_ENABLE_SPI_NBYTE */
    	break;
    case DEV_NUM_SPI_NBYTE_RECV:
    	break;
    case DEV_NUM_AXI_SWITCH:
#ifdef DRIVER_ENABLE_AXI_SWITCH
    	ret = axi_switch_write(devNo, arg, len);
#endif /* DRIVER_ENABLE_AXI_SWITCH */
    	break;
    case DEV_NUM_GPIO_0:          // GPIO Device write
    case DEV_NUM_GPIO_1:
#ifdef DRIVER_ENABLE_GPIO
    	ret = gpio_write(devNo, arg);
#endif //DRIVER_ENABLE_GPIO
    	break;
    case DEV_NUM_UART_1M_MST2MST:   // uart 1M master 2 master channel
	case DEV_NUM_UART_1M_MST2SLV:   // uart 1M master 2 slave channel
	case DEV_NUM_UART_1M_SLV2MST:   // uart 1M slave 2 master channel
	case DEV_NUM_UART_1M_SLV2SLV:   // uart 1M slave 3 slave channel
	case DEV_NUM_UART_2M_MST:   // uart 2M master send device
#ifdef DRIVER_ENABLE_UART_2M
		ret = uart_highspeed_transfer(devNo, arg, len);
#endif /* DRIVER_ENABLE_UART_2M */
    	break;
    case DEV_NUM_USER_0:          // user define device0 ID
    case DEV_NUM_USER_1:          // user define device1 ID
    case DEV_NUM_USER_2:          // user define device2 ID
    case DEV_NUM_USER_3:          // user define device3 ID
#ifdef DRIVER_ENABLE_USER_IP
    	ret = user_ip_write(devNo, arg, len);
#endif /* DRIVER_ENABLE_USER_IP */
    	break;
	case DEV_NUM_INVALID:
		break;
    default:
    	break;
    }

	return ret;
}

/*
 * Function : ioDevRead
 * 		read message from device
 * Parameter :
 * 		devNo : device ID
 * 		arg   : parameter will send to driver layer
 * 		len   : parameter's valid length
 * Return :
 * 		function execute status
 * */
int ioDevRead(uint8_t devNo, uint8_t * arg, uint32_t len)
{
	int ret = IO_STS_SUCCESS;

    switch(devNo)
    {
    case DEV_NUM_CAN_A:	    	 // CAN A device ID
    case DEV_NUM_CAN_B:           // CAN B device ID
    	break;
    case DEV_NUM_UART_A:          // UART port receive command
    case DEV_NUM_UART_B:          // UART port receive high speed data
    	break;
    case DEV_NUM_TIMER:           // timer device ID
#ifdef DRIVER_ENABLE_TIMER
    	ret = timer_read(devNo, arg, len);
#endif /* DRIVER_ENABLE_TIMER */
    	break;
    case DEV_NUM_SEC_PULSE:       // second pulse device ID
    	break;
    case DEV_NUM_ETH:			 // Ethernet device ID
    	break;
    case DEV_NUM_DMA:             // DMA device ID
    	break;
    case DEV_NUM_IIC:             // IIC controller device ID
    	break;
    case DEV_NUM_INTR:            // interrupt controller device ID
    	break;
    case DEV_NUM_SPI_SEND:
    	break;
    case DEV_NUM_SPI_RECV:
#ifdef DRIVER_ENABLE_SPI
		ret = spi_read(devNo, arg, len);
#endif /* DRIVER_ENABLE_SPI*/
    	break;
    case DEV_NUM_SPI_NBYTE_SEND:
    	break;
    case DEV_NUM_SPI_NBYTE_RECV:
#ifdef DRIVER_ENABLE_SPI_NBYTE
    	ret = spi_nbyte_read(devNo, arg, len);
#endif /* DRIVER_ENABLE_SPI_NBYTE */
    	break;
    case DEV_NUM_AXI_SWITCH:
#ifdef DRIVER_ENABLE_AXI_SWITCH
    	ret = axi_switch_read(devNo, arg, len);
#endif /* DRIVER_ENABLE_AXI_SWITCH */
    	break;
    case DEV_NUM_GPIO_0:          // GPIO Device write
    case DEV_NUM_GPIO_1:
#ifdef DRIVER_ENABLE_GPIO
    	ret = gpio_read(devNo, arg);
#endif /* DRIVER_ENABLE_GPIO */
    	break;
    case DEV_NUM_UART_1M_MST2MST:   // uart 1M master 2 master channel
	case DEV_NUM_UART_1M_MST2SLV:   // uart 1M master 2 slave channel
	case DEV_NUM_UART_1M_SLV2MST:   // uart 1M slave 2 master channel
	case DEV_NUM_UART_1M_SLV2SLV:   // uart 1M slave 3 slave channel
	case DEV_NUM_UART_2M_MST:   // uart 2M master send device
#ifdef DRIVER_ENABLE_UART_2M
#endif /* DRIVER_ENABLE_UART_2M */
    	break;
    case DEV_NUM_USER_0:          // user define device0 ID
    case DEV_NUM_USER_1:          // user define device1 ID
    case DEV_NUM_USER_2:          // user define device2 ID
    case DEV_NUM_USER_3:          // user define device3 ID
#ifdef DRIVER_ENABLE_USER_IP
    	ret = user_ip_read(devNo, arg, len);
#endif /* DRIVER_ENABLE_USER_IP */
    	break;
	case DEV_NUM_INVALID:
		break;
    default:
    	break;
    }

	return ret;
}

/*
 * Function : ioDevCtrl
 * 		control device do something else
 * Parameter :
 * 		devNo : device ID
 * 		arg   : parameter will send to driver layer
  * Return :
 * 		function execute status
 * */
int ioDevCtrl(uint8_t devNo, uint8_t * arg)
{
	int ret = IO_STS_SUCCESS;

    switch(devNo)
    {
    case DEV_NUM_CAN_A:	    	 // CAN A device ID
    case DEV_NUM_CAN_B:           // CAN B device ID
    	break;
    case DEV_NUM_UART_A:          // UART port receive command
    case DEV_NUM_UART_B:          // UART port receive high speed data
    	break;
    case DEV_NUM_TIMER:           // timer device ID
#ifdef DRIVER_ENABLE_TIMER
    	ret = timer_ctrl(devNo, arg);
#endif /* DRIVER_ENABLE_TIMER */
    	break;
    case DEV_NUM_FSYNC_1:        // fsyn module for data out
    case DEV_NUM_FSYNC_2:        // fsyn module for data out
    case DEV_NUM_FSYNC:          // user define device0 ID
#ifdef DRIVER_ENABLE_FSYNC
    	ret = fsync_ctrl(devNo, arg);
#endif /* DRIVER_ENABLE_FSYNC */
    	break;
    case DEV_NUM_SEC_PULSE:       // second pulse device ID
#ifdef DRIVER_ENABLE_SECOND_PULSE
    	ret = second_pulse_ctrl(devNo, arg);
#endif /* DRIVER_ENABLE_SECOND_PULSE */
    	break;
    case DEV_NUM_ETH:			 // Ethernet device ID
    	break;
    case DEV_NUM_DMA:             // DMA device ID
    	break;
    case DEV_NUM_IIC:             // IIC controller device ID
#ifdef DRIVER_ENABLE_IIC
    	ret = iic_ctrl(devNo, arg);
#endif /* DRIVER_ENABLE_CAN */
    	break;
    case DEV_NUM_INTR:            // interrupt controller device ID
    	ret = intr_ctrl(arg);
    	break;
    case DEV_NUM_SPI_SEND:
		break;
	case DEV_NUM_SPI_RECV:
		break;
    case DEV_NUM_AXI_SWITCH:
#ifdef DRIVER_ENABLE_AXI_SWITCH
    	ret = axi_switch_ctrl(devNo, arg);
#endif /* DRIVER_ENABLE_AXI_SWITCH */
    	break;
    case DEV_NUM_GPIO_0:          // GPIO Device write
    case DEV_NUM_GPIO_1:
#ifdef DRIVER_ENABLE_GPIO
    	ret = gpio_ctrl(devNo, arg);
#endif /* DRIVER_ENABLE_GPIO */
    	break;
    case DEV_NUM_UART_1M_MST2MST:   // uart 1M master 2 master channel
	case DEV_NUM_UART_1M_MST2SLV:   // uart 1M master 2 slave channel
	case DEV_NUM_UART_1M_SLV2MST:   // uart 1M slave 2 master channel
	case DEV_NUM_UART_1M_SLV2SLV:   // uart 1M slave 3 slave channel
	case DEV_NUM_UART_2M_MST:       // uart 2M master send device
#ifdef DRIVER_ENABLE_UART_2M
#endif /* DRIVER_ENABLE_UART_2M */
    	break;
    case DEV_NUM_USER_0:          // user define device0 ID
    	break;
    case DEV_NUM_USER_1:          // user define device1 ID
    	break;
    case DEV_NUM_USER_2:          // user define device2 ID
    	break;
    case DEV_NUM_USER_3:          // user define device3 ID
    	break;
	case DEV_NUM_INVALID:
		break;
    default:
    	break;
    }

	return ret;
}
/*************************End IODEV Layer Function Implementation ********************************/
/*
 * Function : platform_dev_init
 * 		initial platform device default value
 * Parameter : None
 * Return    : None
 * */
void platform_dev_init(void)
{
	uint32_t m32loop = 0;

	//initial platform device
	for(m32loop = 0; m32loop < MAX_DEV_NUM; m32loop++)
	{
		gPlatformDev.dev_info[m32loop] = &gDeviceInfo[m32loop];
		gPlatformDev.dev_info[m32loop]->dev_conf->dev_id = DEV_NUM_INVALID; // set a invalid device number
		gPlatformDev.dev_info[m32loop]->dev_iic_arg = (S_DEV_IIC_ARG*)malloc(sizeof(S_DEV_IIC_ARG));
		gPlatformDev.dev_info[m32loop]->platform_dev = &gPlatformDev;
	}

	//setup device driver function
#ifdef DRIVER_ENABLE_SPI
	gPlatformDev.spi_ops = &platform_dev_spi_ops;
#endif
#ifdef DRIVER_ENABLE_CAN
	gPlatformDev.can_ops = &platform_dev_can_ops;
#endif /* DRIVER_ENABLE_CAN */
#ifdef DRIVER_ENABLE_IIC
	gPlatformDev.iic_ops = &platform_dev_iic_ops;
#endif /*DRIVER_ENABLE_IIC*/
#ifdef DRIVER_ENABLE_DMA
	gPlatformDev.dma_ops = &platform_dev_dma_ops;
#endif /* DRIVER_ENABLE_CAN */
	return ;
}


void ioDevTest(void)
{
//#define __CAN_TEST__
#ifdef __CAN_TEST__
//	uint8_t databuf[0x100] = {0};
	uint8_t databuf[0x100] = {0};
	uint8_t m8len = 0;

	uint8_t m8loop = 0;

	for(m8len = 0; m8len < 0xff; m8len++)
	{
		databuf[m8len] = m8len;
		if((m8len % 11) >= 3)
		{
			if((m8len / 11) == 0)
			{
				databuf[0] = m8len - 3;
			}
			if((m8len / 11) > 0)
			{
				for(m8loop = 0; m8loop < (m8len / 11); m8loop++)
				{
					databuf[m8loop * 11] = 8;
				}

				databuf[m8loop * 11] = m8len - (m8loop) * 11 - 3;
			}

			ioDevWrite(DEV_NUM_CAN_A, databuf, m8len);
			ioDevWrite(DEV_NUM_CAN_B, databuf, m8len);
		}
	}
#endif

//#define __SPI_TEST__
#ifdef __SPI_TEST__
//	uint8_t databuf[0x100] = {0};
//	uint8_t m8len = 0;

	while(1)
	{
	for(m8len = 0; m8len < 0xff; m8len++)
	{
		databuf[m8len] = m8len;
		if(m8len > 0)
		{
			ioDevWrite(DEV_NUM_SPI_SEND, databuf, m8len);
		}
	}


	for(m8len = 0; m8len < 0xff; m8len++)
	{
		if(m8len > 0)
		{
			ioDevRead(DEV_NUM_SPI_RECV, databuf, m8len);
		}
	}
	}
#endif

//#define __GPIO_TEST__
#ifdef __GPIO_TEST__
	//set default receive master data
//	DATA_PATH_RESET();
	databuf[0] = 0;
	databuf[1] = 0;
	databuf[2] = 0;
	databuf[3] = 0;

	ioDevWrite(DEV_NUM_GPIO_0, databuf, 4);
//	DATA_PATH_RESET_RELEASE();
	//reset data path
	RESET_DATA_PATH();

//	DATA_PATH_RESET();
	databuf[0] = 0;
	databuf[1] = 0;
	databuf[2] = 0;
	databuf[3] = 0x1f;

	ioDevWrite(DEV_NUM_GPIO_0, databuf, 4);
//	DATA_PATH_RESET_RELEASE();
	//reset data path
	RESET_DATA_PATH();

	//set default receive master data
//	DATA_PATH_RESET();
	databuf[0] = 0;
	databuf[1] = 0;
	databuf[2] = 0;
	databuf[3] = 0;

	ioDevWrite(DEV_NUM_GPIO_0, databuf, 4);
//	DATA_PATH_RESET_RELEASE();
	//reset data path
	RESET_DATA_PATH();

#endif /*__GPIO_TEST__*/

//#define __SPI_NBYTE_TEST__
#ifdef __SPI_NBYTE_TEST__
	uint8_t databuf[0x100] = {0};
	uint8_t readdatabuf[0x100] = {0};
	uint8_t m8len = 0;

	for(m8len = 0; m8len < 0xff; m8len++)
	{
		databuf[m8len] = m8len;
		readdatabuf[m8len] = 0xff;
		if(m8len > 0)
		{
			ioDevWrite(DEV_NUM_SPI_NBYTE_SEND, databuf, m8len);
			ioDevRead(DEV_NUM_SPI_NBYTE_RECV, readdatabuf, m8len);
		}
	}
#endif

//#define __PROTOCOL_TEST__
#ifdef __PROTOCOL_TEST__
	uint32_t m8len = 0;
	uint32_t m32loop = 0;

	for(m8len = 0; m8len < ASS_DATA_LENGTH * 5; m8len++)
	{
		if(m8len % 4 == 0)
		{
			g8AssDataBuffer[1][m8len] = 0xd0 | (m8len / 4);
		}
		else
		{
			g8AssDataBuffer[1][m8len] = m8len;
		}
	}

	m8len = 0;
	for(m32loop = 0; m32loop < (ASS_DATA_LENGTH * 5) / 4; m32loop++)
	{
		if((g8AssDataBuffer[1][m32loop * 4] == 0xd0) || (g8AssDataBuffer[1][m32loop * 4] == 0xdf))
		{
			m8len++;
			continue;
		}
		g8AssDataBuffer[0][(m32loop - m8len) * 3 + 0] = g8AssDataBuffer[1][m32loop * 4 + 1];
		g8AssDataBuffer[0][(m32loop - m8len) * 3 + 1] = g8AssDataBuffer[1][m32loop * 4 + 2];
		g8AssDataBuffer[0][(m32loop - m8len) * 3 + 2] = g8AssDataBuffer[1][m32loop * 4 + 3];
	}
#endif

}

