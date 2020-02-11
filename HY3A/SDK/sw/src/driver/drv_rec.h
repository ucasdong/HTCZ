/*
 * rec.h
 *
 *  Created on: 2018Äê9ÔÂ16ÈÕ
 *      Author: Administrator
 */

#ifndef SRC_DRV_REC_H_
#define SRC_DRV_REC_H_

#include "driver.h"

//macro define start

#define REC_WRITE_REG(addr, val)\
{\
	*(volatile uint32_t*)(addr) = val;\
}

#define REC_READ_REG(addr)\
{\
	*(volatile uint32_t*)(addr)\
}


#define Sim_Fsyn                       0x41000000
#define Sec_Fsyn                       0x42000000
#define AXI_liteReg                    0x43000000



#define SendHorizontal               1153
#define SendVertical_1               2593
#define SendVertical_2               3*1
#define SendVertical_3               33*1
#define SendType1                    01
#define SendType2                    02
#define SendType3                    03



//macro define end


//function declare
void config_rec(u32 BaseAddr,u32 Ver ,u32 Hor);
void config_send(u32 BaseAddr,u32 Ver ,u32 Hor,u32 type);
void config_sendtype(u32 BaseAddr,u8 type);
void Reset_Fpage(u32 BaseAddr);

#endif /* SRC_DRV_REC_H_ */
