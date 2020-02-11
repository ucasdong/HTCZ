/*
 * drv_spi_nbyte.h
 *
 *  Created on: 2018Äê11ÔÂ14ÈÕ
 *      Author: 508
 */

#ifndef SRC_DRIVER_DRV_SPI_NBYTE_H_
#define SRC_DRIVER_DRV_SPI_NBYTE_H_

#include "driver.h"

#ifdef DRIVER_ENABLE_SPI_NBYTE

#define SPI_NBYTE_SUCCESS		 0
#define SPI_NBYTE_FAIL  		 1

#define SPI_NBYTE_SEND_BASEADDR  XPAR_MBLAZE_NBYTESPI_S00_AXI_SEND_BASEADDR
#define SPI_NBYTE_SEND_HIGHADDR  XPAR_MBLAZE_NBYTESPI_S00_AXI_SEND_HIGHADDR

#define SPI_NBYTE_REC_BASEADDR   XPAR_MBLAZE_NBYTESPI_S00_AXI_REC_BASEADDR
#define SPI_NBYTE_REC_HIGHADDR   XPAR_MBLAZE_NBYTESPI_S00_AXI_REC_HIGHADDR

#define LITE2RAM_SEND_BASEADDR	 XPAR_MBLAZE_LITE2RAM_S00_AXI_SEND_BASEADDR
#define LITE2RAM_REC_BASEADDR	 XPAR_MBLAZE_LITE2RAM_S00_AXI_REC_BASEADDR


#define CTRL_REG_WR_CNT          0x0
#define CTRL_REG_START_WR        0x4
#define CTRL_REG_RD_CNT 		 0x08
#define CTRL_REG_START_RD		 0x0c




int init_spi_nbyte(uint32_t regaddr);
void write_spi_nbyte(uint32_t reg_addr, uint8_t * data, uint32_t len);
void read_spi_nbyte(uint32_t reg_addr, uint8_t * data, uint32_t len);

#endif /* DRIVER_ENABLE_SPI_NBYTE */

#endif /* SRC_DRIVER_DRV_SPI_NBYTE_H_ */
