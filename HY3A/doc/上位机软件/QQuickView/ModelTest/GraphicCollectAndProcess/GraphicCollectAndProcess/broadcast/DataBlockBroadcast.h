#pragma once
#include "DataInit.h"
#include <vector>
#include <map>

class DataBlockBroadcast
{
public:
	DataBlockBroadcast();
	~DataBlockBroadcast();

	
	//vector<oneBroadCastData> allParamData;
	map<string, oneBroadCastData> mapAllData;

	BYTE SUM;                 //ÿһ�������һ���ַ�
	int iCount;               //�ļ���С
	BYTE* allProgramData;     //�ļ�����
	vector<BYTE*> vecData;    //���ɵ�֡����
	vector<BYTE*> GetOnePagData(string strName, BYTE type, BYTE VERSION, int iPag);//��ȡһ�����Ĺ㲥֡
	vector<BYTE*> GetSendData(string strName, BYTE type, BYTE VERSION);            //���ո�ʽ��ȡ�㲥֡
	void DestorySendData();   //��ȡ��㲥֡������
	void Sum(BYTE num);       //����ÿһ����SUM
	void GetTestISO(BYTE& num1, BYTE& num2,vector<BYTE>& vecTemp);//����ÿһ����ISO
	void isoTest2(BYTE& num1, BYTE& num2,vector<BYTE*> vecTemp);
	bool ReadSendDataFile(string strFileName, string& errInfo);   //��ȡ���㲥���͵���ע���ļ�
	void readBroadcastFile(string strFileName);                   //��ȡ����
	static vector<string> split(string str, string pattern);      //�ַ����и�

	BYTE TranslateCharToHex(char data);                           //char����ת����ʮ������

};

