#pragma once

#include <QObject>
#include <vector>
using namespace std;

typedef unsigned char BYTE;

struct oneParamData
{
	QString strName;       //参数名称
	int iType;             //数据类型
	QString strBelongGroup;//所属组
	int iStartBit;         //起始字节
	int iAnalysisBits;     //读取字节
	QString strShow;       //解析的数据转换成字符串
};

class CodeDataAnalysis : public QObject
{
	Q_OBJECT

public:
	CodeDataAnalysis();
	~CodeDataAnalysis();
	//配置文件相关
	void readDataFile(QString strfileName);                               //读取数据解析配置文件
	//void writeDataFile(QString strfileName, vector<oneParamData>& vecAllParam);
	//Json配置文件相关
	void readJsonDataFile(QString strfileName);                          //读取数据解析配置文件
	void writeJsonDataFile(QString strfileName, vector<oneParamData>& vecAllParam);


	//调用接口
	vector<oneParamData>* getAllParamsData();                              //获取所有参数信息
	int getParamsCount();                                                  //获取参数总行数
	vector<QString> getTableHeader();                                      //获取表头
	void setTableHeader(vector<QString> vecHeader);                        //设置表头
	bool findOneParamSetting(QString strParamName, oneParamData& oneParam);//查找某一条参数配置信息		
	BYTE* DecodePacket(BYTE *package, QString strName, int& packageLen);   //解析包中的一个参数
	bool GetBit(BYTE* package, int iStart, int iCount, BYTE& data);        //解析包中的一个字节长度以内的参数
	BYTE* GetBits(BYTE* package, int iStart, int iCount, int* packageLen); //解析包中一个字节以上的参数

private:
	vector<oneParamData> allParamData;
	vector<QString> tableHeader;

	BYTE* GetBits(BYTE* package, oneParamData &one, int& packageLen);
	void movRight(BYTE* data, int iLen, int iCount);
};
