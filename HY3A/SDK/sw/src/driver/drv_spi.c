/*
 * drv_spi.c
 *
 *  Created on: 2018Äê10ÔÂ31ÈÕ
 *      Author: 508
 */


#include "drv_spi.h"

#ifdef DRIVER_ENABLE_SPI

static void write_spi(uint32_t reg_addr, uint8_t * data, uint32_t len)
{
	uint32_t m32loop = 0;
	uint8_t  m8subloop = 0;

	uint32_t m32lower_data = 0;
	uint32_t m32uper_data  = 0;

	if(len >= 8)
	{
		for(m32loop = 0; m32loop < len / 8; m32loop++)
		{
			m32lower_data = 0;
			m32uper_data  = 0;

			for(m8subloop = 0; m8subloop < 4; m8subloop++)
			{
				m32lower_data = (m32lower_data << 8) | data[8 * m32loop + m8subloop];
			}

			for(m8subloop = 0; m8subloop < 4; m8subloop++)
			{
				m32uper_data = (m32uper_data << 8) | data[8 * m32loop + 4 + m8subloop];
			}
#ifdef USE_SPI_32_BIT
			WRITE_REG(reg_addr, m32lower_data);
			WRITE_REG(reg_addr + 4, 0);
			WRITE_REG(reg_addr, m32uper_data);
			WRITE_REG(reg_addr + 4, 0);
#else
			WRITE_REG(reg_addr, m32lower_data);
			WRITE_REG(reg_addr + 4, m32uper_data);
#endif
		}
	}

	if(len != (m32loop * 8))
	{
		m32lower_data = 0;
		m32uper_data  = 0;
		if((len - (m32loop * 8)) > 4)
		{
			for(m8subloop = 0; m8subloop < 4; m8subloop++)
			{
				m32lower_data = (m32lower_data << 8) | data[8 * m32loop + m8subloop];
			}
			for(m8subloop = 0; m8subloop < (len - (m32loop * 8) - 4); m8subloop++)
			{
				m32uper_data = (m32uper_data << 8) | data[8 * m32loop + 4 + m8subloop];
			}
//			for(; m8subloop < 4; m8subloop++)
//			{
//				m32uper_data = (m32uper_data << 8);
//			}
		}
		else
		{
			for(m8subloop = 0; m8subloop < (len - (m32loop * 8)); m8subloop++)
			{
				m32lower_data = (m32lower_data << 8) | data[8 * m32loop + m8subloop];
			}
//			for(; m8subloop < 4; m8subloop++)
//			{
//				m32lower_data = (m32lower_data << 8);
//			}
		}

#ifdef USE_SPI_32_BIT
			WRITE_REG(reg_addr, m32lower_data);
			WRITE_REG(reg_addr + 4, 0);
			WRITE_REG(reg_addr, m32uper_data);
			WRITE_REG(reg_addr + 4, 0);
#else
			WRITE_REG(reg_addr, m32lower_data);
			WRITE_REG(reg_addr + 4, m32uper_data);
#endif
	}

	return ;
}

static void read_spi(uint32_t reg_addr, uint8_t * data, uint32_t len)
{
	uint32_t m32loop = 0;

	uint32_t m32lower_data = 0;
	uint32_t m32uper_data  = 0;

	if(len >= 8)
	{
		for(m32loop = 0; m32loop < len / 8; m32loop++)
		{
			m32lower_data = READ_REG(reg_addr + 0xc);
			m32uper_data  = READ_REG(reg_addr + 0x10);

			data[8 * m32loop + 0] = (m32lower_data >> 24) & 0xff;
			data[8 * m32loop + 1] = (m32lower_data >> 16) & 0xff;
			data[8 * m32loop + 2] = (m32lower_data >>  8) & 0xff;
			data[8 * m32loop + 3] = (m32lower_data      ) & 0xff;

			data[8 * m32loop + 4] = (m32uper_data >> 24) & 0xff;
			data[8 * m32loop + 5] = (m32uper_data >> 16) & 0xff;
			data[8 * m32loop + 6] = (m32uper_data >>  8) & 0xff;
			data[8 * m32loop + 7] = (m32uper_data      ) & 0xff;
		}
	}

	if(len != (m32loop * 8))
	{
		if((len - (m32loop * 8)) > 4)
		{
			m32lower_data = READ_REG(reg_addr + 0xc);
			m32uper_data  = READ_REG(reg_addr + 0x10);

			data[8 * m32loop + 0] = (m32lower_data >> 24) & 0xff;
			data[8 * m32loop + 1] = (m32lower_data >> 16) & 0xff;
			data[8 * m32loop + 2] = (m32lower_data >>  8) & 0xff;
			data[8 * m32loop + 3] = (m32lower_data      ) & 0xff;

			data[8 * m32loop + 4] = (m32uper_data >> 24) & 0xff;
			data[8 * m32loop + 5] = (m32uper_data >> 16) & 0xff;
			data[8 * m32loop + 6] = (m32uper_data >>  8) & 0xff;
			data[8 * m32loop + 7] = (m32uper_data      ) & 0xff;
		}
		else
		{
			m32lower_data = READ_REG(reg_addr + 0xc);

			data[8 * m32loop + 0] = (m32lower_data >> 24) & 0xff;
			data[8 * m32loop + 1] = (m32lower_data >> 16) & 0xff;
			data[8 * m32loop + 2] = (m32lower_data >>  8) & 0xff;
			data[8 * m32loop + 3] = (m32lower_data      ) & 0xff;
		}
	}
}

/* platform device SPI operation functions implementation */
int dev_spi_init(S_DEV_INFO * dev)
{
	int ret = SPI_SUCCESS;

	return ret;
}

int dev_spi_write(S_DEV_INFO * dev)
{
	int ret = SPI_SUCCESS;

	//parameter check
	if(dev->dev_conf->dev_id == DEV_NUM_INVALID)
	{
		MST_IO_MSG( "%s: SPI Transfer fail, device No %d\n", __func__, dev->dev_conf->dev_id);
		return SPI_FAIL;
	}

	//parameter check
	if(dev->dev_spi_arg->src == NULL)
	{
		MST_IO_MSG( "%s: CAN Transfer fail, source data invalid\n", __func__);
		return SPI_FAIL;
	}
	else
	{
		write_spi(dev->dev_conf->base_addr, dev->dev_spi_arg->src, dev->dev_spi_arg->len);
	}
//	drv_can_transmit(dev->dev_conf->base_addr, dev->dev_can_arg->src, dev->dev_can_arg->len);

	return ret;
}

int dev_spi_read(S_DEV_INFO * dev)
{
	int ret = SPI_SUCCESS;

	//parameter check
	if(dev->dev_conf->dev_id == DEV_NUM_INVALID)
	{
		MST_IO_MSG( "%s: SPI Transfer fail, device No %d\n", __func__, dev->dev_conf->dev_id);
		return SPI_FAIL;
	}

	//parameter check
	if(dev->dev_spi_arg->len == 0 || dev->dev_spi_arg->src == NULL)
	{
		MST_IO_MSG( "%s: SPI Read fail, source data invalid\n", __func__);
		return SPI_FAIL;
	}
	else
	{
		read_spi(dev->dev_conf->base_addr, dev->dev_spi_arg->src, dev->dev_spi_arg->len);
	}
	return ret;
}
/* end */
/* can driver function define */
S_DEV_SPI_OPS platform_dev_spi_ops = {
		.init  = dev_spi_init,
		.write = dev_spi_write,
		.read  = dev_spi_read,
};

#endif /* DRIVER_ENABLE_SPI */
