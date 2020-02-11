/*
 * drv_dma.h
 *
 *  Created on: 2018Äê10ÔÂ23ÈÕ
 *      Author: 508
 */

#ifndef SRC_DRV_DMA_H_
#define SRC_DRV_DMA_H_

#include "driver.h"

#ifdef DRIVER_ENABLE_DMA

/* device DMA operation functions define */
typedef struct dev_dma_ops
{
	/* DMA OPS */
	void (*init)( S_DEV_INFO * dev );
	void (*write)( S_DEV_INFO * dev );
	void (*read)( S_DEV_INFO * dev );
	void (*stop)( S_DEV_INFO * dev );
} S_DEV_DMA_OPS;

extern S_DEV_DMA_OPS platform_dev_dma_ops;

#endif /* DRIVER_ENABLE_DMA */

#endif /* SRC_DRV_DMA_H_ */
