/*
 * rec.c
 *
 *  Created on: 2018��9��16��
 *      Author: Administrator
 */


#include "drv_rec.h"

void config_rec(u32 BaseAddr,u32 Ver ,u32 Hor)
{
	REC_WRITE_REG(BaseAddr+6*4,0x0000aaaa);	//������ģ����߼���λ

	REC_WRITE_REG(BaseAddr,Ver);      		//  ���ܵ�ת��ģ����� ��
	REC_WRITE_REG(BaseAddr+1*4,Hor);  		//  ���ܵ�ת��ģ���ÿ�� ʱ�� ��

	REC_WRITE_REG(BaseAddr+2*4,0x80000000); //���� Datamover  ���� ��ַ
	REC_WRITE_REG(BaseAddr+3*4,Hor*4);      //���� Datamover  ����Ԫ��
	REC_WRITE_REG(BaseAddr+4*4,Ver);        //���� Datamover ������
	REC_WRITE_REG(BaseAddr+5*4,2);          //���� Datamover �Ļ���֡��

	REC_WRITE_REG(BaseAddr+12*4,Ver);       //���� ����� ģ�� �� ������     ASS LINE+ DATA LINE
	REC_WRITE_REG(BaseAddr+13*4,Hor);       //���� ����� ģ��      TOTAL CLOCK NUB

	REC_WRITE_REG(BaseAddr+6*4,0x00005555); //������ģ����߼���λ����
	REC_WRITE_REG(BaseAddr+6*4,0x00001111); // �ͷ���λ �� ������Ȩ��FPGA

	return ;
}

//��λfpga
void Reset_Fpage(u32 BaseAddr)
{
	u16 tmp =0;

	REC_WRITE_REG(BaseAddr+6*4,0x0000aaaa);//������ģ����߼���λ

	while(tmp++<1000);

	REC_WRITE_REG(BaseAddr+6*4,0x00005555);       //������ģ����߼���λ����

	REC_WRITE_REG(BaseAddr+6*4,0x00001111);     // �ͷ���λ �� ������Ȩ��FPGA

	return ;
}

//ģ�������ͬģʽ����
void config_sendtype(u32 BaseAddr,u8 type)
{
	u32 temp ;

	if(type==1)
	{

		REC_WRITE_REG(Sim_Fsyn,0x00005555);     //ֹͣ  ����ģ����
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
		REC_WRITE_REG(Sim_Fsyn,0x0000aaaa);     //��ʼ  ����ģ����


	}
	else if (type==2)
	{
		REC_WRITE_REG(Sim_Fsyn,0x00005555);     //ֹͣ  ����ģ����
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
		REC_WRITE_REG(Sim_Fsyn,0x0000aaaa);     //��ʼ  ����ģ����
	}
	else if (type==3)
	{
		REC_WRITE_REG(Sim_Fsyn,0x00005555);     //ֹͣ  ����ģ����
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
		REC_WRITE_REG(Sim_Fsyn,0x0000aaaa);     //��ʼ  ����ģ����
	}
}


void config_send(u32 BaseAddr,u32 Ver ,u32 Hor,u32 type)
{

	REC_WRITE_REG(BaseAddr+8*4,Ver-1);       //ģ�ⷢ��  �� DATA  2 LINE OR 32 LINE  0R 2592 LINE

	REC_WRITE_REG(BaseAddr+9*4,Hor);       //ģ�ⷢ��  �� LINE CLOCK NUB  (INCLUDE  ASS DATA AND ���� ʱ�� ģ�� )

	REC_WRITE_REG(BaseAddr+15*4,1);          	//ģ�ⷢ��  ��  REG 15  ASS LINE NUB

	REC_WRITE_REG(BaseAddr+10*4,Hor);       //ģ�ⷢ��  �� ASS TOTAL CLOCK NUB

	REC_WRITE_REG(BaseAddr+11*4,type);       //ģ�ⷢ��  ��DATA TAG  0X0001
	REC_WRITE_REG(BaseAddr+16*4,(Ver-1)*Hor);    	//ģ�ⷢ��  �� REG 16  DATA TOTAL CLOCK NUB(2*1153 OR 32*1153 OR 2592*1153)

	REC_WRITE_REG(BaseAddr+14*4,Ver);         	//ģ�ⷢ�� ģ��  REG 14  ASS LINE+ DATA LINE

}


