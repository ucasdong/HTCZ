#pragma once

#include <iostream>
using namespace std;

typedef unsigned char BYTE;

struct oneBroadCastData
{
	string   param_Name;     //�㲥����
	string   param_Head;     //֡ͷ
	string   param_I;        //֡���
	string   param_L;        //��Ч���ݳ���
	string   param_T;        //��ʶ����������+վ��ַ��ʶ��
	int      param_PerBagSize;//ÿ�����Ĵ�С
	int      param_AllBagCount;//���ĸ���
	int      param_BagCount; //��֡����������
	int      param_BagLength;//��֡���ȣ�ÿ���ֽ�����
};

//��ע���ݿ�㲥���ͣ�����չ��
enum ShangzhuTypeEnum
{
	LEFT_PROGRAM,   //����ӳ� ������ע
	RIGHT_PROGRAM,  //�Ұ��ӳ� ������ע
	LEFT_PARAM,     //����ӳ� ������ע
	RIGHT_PARAM��   //�Ұ��ӳ� ������ע
};

