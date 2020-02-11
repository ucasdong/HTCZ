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

	BYTE SUM;                 //每一个包最后一个字符
	int iCount;               //文件大小
	BYTE* allProgramData;     //文件内容
	vector<BYTE*> vecData;    //生成的帧序列
	vector<BYTE*> GetOnePagData(string strName, BYTE type, BYTE VERSION, int iPag);//获取一个包的广播帧
	vector<BYTE*> GetSendData(string strName, BYTE type, BYTE VERSION);            //按照格式获取广播帧
	void DestorySendData();   //获取完广播帧后销毁
	void Sum(BYTE num);       //计算每一包的SUM
	void GetTestISO(BYTE& num1, BYTE& num2,vector<BYTE>& vecTemp);//计算每一包的ISO
	void isoTest2(BYTE& num1, BYTE& num2,vector<BYTE*> vecTemp);
	bool ReadSendDataFile(string strFileName, string& errInfo);   //读取待广播发送的上注快文件
	void readBroadcastFile(string strFileName);                   //读取配置
	static vector<string> split(string str, string pattern);      //字符串切割

	BYTE TranslateCharToHex(char data);                           //char类型转换成十六进制

};

