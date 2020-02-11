#pragma once

#include <QObject>
#include <vector>
using namespace std;

typedef unsigned char BYTE;

struct oneParamData
{
	QString strName;       //��������
	int iType;             //��������
	QString strBelongGroup;//������
	int iStartBit;         //��ʼ�ֽ�
	int iAnalysisBits;     //��ȡ�ֽ�
	QString strShow;       //����������ת�����ַ���
};

class CodeDataAnalysis : public QObject
{
	Q_OBJECT

public:
	CodeDataAnalysis();
	~CodeDataAnalysis();
	//�����ļ����
	void readDataFile(QString strfileName);                               //��ȡ���ݽ��������ļ�
	//void writeDataFile(QString strfileName, vector<oneParamData>& vecAllParam);
	//Json�����ļ����
	void readJsonDataFile(QString strfileName);                          //��ȡ���ݽ��������ļ�
	void writeJsonDataFile(QString strfileName, vector<oneParamData>& vecAllParam);


	//���ýӿ�
	vector<oneParamData>* getAllParamsData();                              //��ȡ���в�����Ϣ
	int getParamsCount();                                                  //��ȡ����������
	vector<QString> getTableHeader();                                      //��ȡ��ͷ
	void setTableHeader(vector<QString> vecHeader);                        //���ñ�ͷ
	bool findOneParamSetting(QString strParamName, oneParamData& oneParam);//����ĳһ������������Ϣ		
	BYTE* DecodePacket(BYTE *package, QString strName, int& packageLen);   //�������е�һ������
	bool GetBit(BYTE* package, int iStart, int iCount, BYTE& data);        //�������е�һ���ֽڳ������ڵĲ���
	BYTE* GetBits(BYTE* package, int iStart, int iCount, int* packageLen); //��������һ���ֽ����ϵĲ���

private:
	vector<oneParamData> allParamData;
	vector<QString> tableHeader;

	BYTE* GetBits(BYTE* package, oneParamData &one, int& packageLen);
	void movRight(BYTE* data, int iLen, int iCount);
};
