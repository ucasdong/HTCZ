/*
 * drv_spi.h
 *
 *  Created on: 2018Äê10ÔÂ31ÈÕ
 *      Author: 508
 */

#ifndef SRC_DRIVER_DRV_SPI_H_
#define SRC_DRIVER_DRV_SPI_H_

#include "driver.h"

#ifdef DRIVER_ENABLE_SPI

#define SPI_SUCCESS            0
#define SPI_FAIL               1

#define USE_SPI_32_BIT

/* device CAN operation functions define */
typedef struct dev_spi_ops
{
    /* CAN OPS */
    int (*init)( S_DEV_INFO * dev);
    int  (*write)( S_DEV_INFO * dev);
    int  (*read)( S_DEV_INFO * dev);
} S_DEV_SPI_OPS;

extern S_DEV_SPI_OPS platform_dev_spi_ops;

#endif /* DRIVER_ENABLE_SPI */

#endif /* SRC_DRIVER_DRV_SPI_H_ */
