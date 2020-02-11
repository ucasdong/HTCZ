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

	bool bStop;                                     //线程结束控制
	void initFile(QString strFileName, int iFlag);  //初始化文件,strFileName:文件名称，iFlag：读(2)或者写(1)标志
	void insertData(BYTE* dataTemp, DWORD dwLen, bool bFinish);   //插入要写的数据（包含申请内存，先初始化文件）
	bool readData(BYTE* dataTemp, DWORD dwLen);     //读取文件数据1（需要提前初始化文件）（当前使用）
	bool readData2(BYTE* dataTemp, DWORD dwLen);    //读取文件数据2（需要提前初始化文件）

	void resetReadFileSize(__int64 startSize);      //重置读文件计数
private:
	bool bInitFlag;               //文件初始化标志
	HANDLE hFile;                 //文件句柄
	__int64 dwWriteFileSize;      //写文件大小（用于计数）
	__int64 dwReadFileSize;       //读文件大小（用于计数）
	QMutex mutexLock;             //数据存取锁
	HANDLE hThread;               //线程句柄
	DWORD dwAllocationGranularity;//系统粒度
	std::vector<std::pair<BYTE*, DWORD>> vecDataBuffer;//需写入的数据
	int iFlagType;                //存储类型

signals:
	void signal_writeData();

public slots:
	void writeData();                              //写文件方式1,包含写完文件后内存释放
	void writeData2();                             //写文件方式2,包含写完文件后内存释放（每次写入的数据需要是512字节的倍数）
};
