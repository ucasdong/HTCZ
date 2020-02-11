/*
 * drv_spi_nbyte.c
 *
 *  Created on: 2018Äê11ÔÂ14ÈÕ
 *      Author: 508
 */


#include "drv_spi_nbyte.h"

#ifdef DRIVER_ENABLE_SPI_NBYTE

int init_spi_nbyte(uint32_t regaddr)
{
	int ret = SPI_NBYTE_SUCCESS;

	return ret;
}

void write_spi_nbyte(uint32_t reg_addr, uint8_t * data, uint32_t len)
{
	uint32_t m32loop = 0;

	//fill source data to BRAM
	for(m32loop = 0; m32loop < len; m32loop++)
	{
		WRITE_REG(LITE2RAM_SEND_BASEADDR, data[m32loop]);
//		WRITE_REG(LITE2RAM_SEND_BASEADDR, 0xaaaaaaaa);
		WRITE_REG(LITE2RAM_SEND_BASEADDR + 4, m32loop);
	}
#if 1
//	uint8_t buf[512] = {0};
	//check BRAM data
//	for(m32loop = 0; m32loop < len; m32loop++)
//	{
//		WRITE_REG(LITE2RAM_SEND_BASEADDR + 8, m32loop);
//		buf[m32loop] = READ_REG(LITE2RAM_SEND_BASEADDR + 0xc);
//
//		if(buf[m32loop] != data[m32loop])
//		{
//			break;
//		}
//	}
#endif
	//fill spi nbyte control register
	WRITE_REG(reg_addr + CTRL_REG_WR_CNT, len);     // total send byte
	WRITE_REG(reg_addr + CTRL_REG_START_WR, len);   // start send timing

	return ;
}

void read_spi_nbyte(uint32_t reg_addr, uint8_t * data, uint32_t len)
{
	uint32_t m32loop = 0;

	//fill spi nbyte control register
	WRITE_REG(reg_addr + CTRL_REG_RD_CNT , len);     // total receive byte
	WRITE_REG(reg_addr + CTRL_REG_START_RD, len);    // start receive timing

	//read BRAM data
	for(m32loop = 0; m32loop < len; m32loop++)
	{
		WRITE_REG(LITE2RAM_REC_BASEADDR + 8, 4 * m32loop);
		data[m32loop] = READ_REG(LITE2RAM_REC_BASEADDR + 0xc);
	}

	return ;
}


#endif /* DRIVER_ENABLE_SPI_NBYTE */
