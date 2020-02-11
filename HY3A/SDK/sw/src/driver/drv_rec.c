/*
 * rec.c
 *
 *  Created on: 2018年9月16日
 *      Author: Administrator
 */


#include "drv_rec.h"

void config_rec(u32 BaseAddr,u32 Ver ,u32 Hor)
{
	REC_WRITE_REG(BaseAddr+6*4,0x0000aaaa);	//给接受模块的逻辑复位

	REC_WRITE_REG(BaseAddr,Ver);      		//  接受的转换模块的行 数
	REC_WRITE_REG(BaseAddr+1*4,Hor);  		//  接受的转换模块的每行 时钟 数

	REC_WRITE_REG(BaseAddr+2*4,0x80000000); //接受 Datamover  的首 地址
	REC_WRITE_REG(BaseAddr+3*4,Hor*4);      //接受 Datamover  的像元数
	REC_WRITE_REG(BaseAddr+4*4,Ver);        //接受 Datamover 的行数
	REC_WRITE_REG(BaseAddr+5*4,2);          //接受 Datamover 的缓冲帧数

	REC_WRITE_REG(BaseAddr+12*4,Ver);       //接受 的输出 模块 的 总行数     ASS LINE+ DATA LINE
	REC_WRITE_REG(BaseAddr+13*4,Hor);       //接受 的输出 模块      TOTAL CLOCK NUB

	REC_WRITE_REG(BaseAddr+6*4,0x00005555); //给接受模块的逻辑复位结束
	REC_WRITE_REG(BaseAddr+6*4,0x00001111); // 释放软复位 的 控制器权给FPGA

	return ;
}

//复位fpga
void Reset_Fpage(u32 BaseAddr)
{
	u16 tmp =0;

	REC_WRITE_REG(BaseAddr+6*4,0x0000aaaa);//给接受模块的逻辑复位

	while(tmp++<1000);

	REC_WRITE_REG(BaseAddr+6*4,0x00005555);       //给接受模块的逻辑复位结束

	REC_WRITE_REG(BaseAddr+6*4,0x00001111);     // 释放软复位 的 控制器权给FPGA

	return ;
}

//模拟相机不同模式发数
void config_sendtype(u32 BaseAddr,u8 type)
{
	u32 temp ;

	if(type==1)
	{

		REC_WRITE_REG(Sim_Fsyn,0x00005555);     //停止  发送模拟数
		temp =Xil_In32(Sim_Fsyn+2*4);
		while(temp!=0x5555)
		{
			temp=Xil_In32(Sim_Fsyn+2*4);
		}
		temp =Xil_In32(Sim_Fsyn+2*4);
		while(temp!=0x5555)
		{
			temp=Xil_In32(Sim_Fsyn+2*4);
		}


		config_send(BaseAddr,SendVertical_1,SendHorizontal,SendType1);
		REC_WRITE_REG(Sim_Fsyn,0x0000aaaa);     //开始  发送模拟数


	}
	else if (type==2)
	{
		REC_WRITE_REG(Sim_Fsyn,0x00005555);     //停止  发送模拟数
		temp =Xil_In32(Sim_Fsyn+2*4);
		while(temp!=0x5555)
		{
			temp=Xil_In32(Sim_Fsyn+2*4);
		}
		temp =Xil_In32(Sim_Fsyn+2*4);
		while(temp!=0x5555)
		{
			temp=Xil_In32(Sim_Fsyn+2*4);
		}

		config_send(BaseAddr,SendVertical_2,SendHorizontal,SendType2);
		REC_WRITE_REG(Sim_Fsyn,0x0000aaaa);     //开始  发送模拟数
	}
	else if (type==3)
	{
		REC_WRITE_REG(Sim_Fsyn,0x00005555);     //停止  发送模拟数
		temp =Xil_In32(Sim_Fsyn+2*4);
		while(temp!=0x5555)
		{
			temp=Xil_In32(Sim_Fsyn+2*4);
		}
		temp =Xil_In32(Sim_Fsyn+2*4);
		while(temp!=0x5555)
		{
			temp=Xil_In32(Sim_Fsyn+2*4);
		}
		config_send(BaseAddr,SendVertical_3,SendHorizontal,SendType3);
		REC_WRITE_REG(Sim_Fsyn,0x0000aaaa);     //开始  发送模拟数
	}
}


void config_send(u32 BaseAddr,u32 Ver ,u32 Hor,u32 type)
{

	REC_WRITE_REG(BaseAddr+8*4,Ver-1);       //模拟发送  的 DATA  2 LINE OR 32 LINE  0R 2592 LINE

	REC_WRITE_REG(BaseAddr+9*4,Hor);       //模拟发送  的 LINE CLOCK NUB  (INCLUDE  ASS DATA AND 发送 时序 模块 )

	REC_WRITE_REG(BaseAddr+15*4,1);          	//模拟发送  的  REG 15  ASS LINE NUB

	REC_WRITE_REG(BaseAddr+10*4,Hor);       //模拟发送  的 ASS TOTAL CLOCK NUB

	REC_WRITE_REG(BaseAddr+11*4,type);       //模拟发送  的DATA TAG  0X0001
	REC_WRITE_REG(BaseAddr+16*4,(Ver-1)*Hor);    	//模拟发送  的 REG 16  DATA TOTAL CLOCK NUB(2*1153 OR 32*1153 OR 2592*1153)

	REC_WRITE_REG(BaseAddr+14*4,Ver);         	//模拟发送 模块  REG 14  ASS LINE+ DATA LINE

}


