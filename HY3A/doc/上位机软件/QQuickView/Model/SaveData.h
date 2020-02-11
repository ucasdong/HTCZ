#pragma once

#include <QObject>
#include <Windows.h>
#include <QDateTime>
#include <iostream>
#include <QMutex>
using namespace std;
typedef unsigned char BYTE;

#define USESIGNAL

class SaveData : public QObject
{
	Q_OBJECT

public:
	SaveData();
	~SaveData();

	bool bStop;                                     //�߳̽�������
	void initFile(QString strFileName, int iFlag);  //��ʼ���ļ�,strFileName:�ļ����ƣ�iFlag����(2)����д(1)��־
	void insertData(BYTE* dataTemp, DWORD dwLen, bool bFinish);   //����Ҫд�����ݣ����������ڴ棬�ȳ�ʼ���ļ���
	bool readData(BYTE* dataTemp, DWORD dwLen);     //��ȡ�ļ�����1����Ҫ��ǰ��ʼ���ļ�������ǰʹ�ã�
	bool readData2(BYTE* dataTemp, DWORD dwLen);    //��ȡ�ļ�����2����Ҫ��ǰ��ʼ���ļ���

	void resetReadFileSize(__int64 startSize);      //���ö��ļ�����
private:
	bool bInitFlag;               //�ļ���ʼ����־
	HANDLE hFile;                 //�ļ����
	__int64 dwWriteFileSize;      //д�ļ���С�����ڼ�����
	__int64 dwReadFileSize;       //���ļ���С�����ڼ�����
	QMutex mutexLock;             //���ݴ�ȡ��
	HANDLE hThread;               //�߳̾��
	DWORD dwAllocationGranularity;//ϵͳ����
	std::vector<std::pair<BYTE*, DWORD>> vecDataBuffer;//��д�������
	int iFlagType;                //�洢����

signals:
	void signal_writeData();

public slots:
	void writeData();                              //д�ļ���ʽ1,����д���ļ����ڴ��ͷ�
	void writeData2();                             //д�ļ���ʽ2,����д���ļ����ڴ��ͷţ�ÿ��д���������Ҫ��512�ֽڵı�����
};
