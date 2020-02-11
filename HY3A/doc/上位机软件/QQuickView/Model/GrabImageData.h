#pragma once

#include <QObject>
#include "windows.h"

typedef void(*CallbackFunc)(void *params);//PickPkgContent* Info);

class GrabImageData : public QObject
{
	Q_OBJECT

public:
	GrabImageData(QObject *parent);
	~GrabImageData();

	void RegisterCallBack(CallbackFunc func, void* param);
	void StartGrab();
	void StopGrab();
	void StartSave(int iSaveType, char* cPath);
	void StopSave();
	void StartPlaybackGrab(char* cPath);

	bool getGrabFlag();
	bool getStopThreadFlag();
	void createGraphicData();                 //创建数据

	CallbackFunc  m_pFunc;
	void*         m_param;
	void GetAddress(void** pData);

private:
	bool bStartGrab; //开始或停止采集
	bool bStopThread;//结束线程
	unsigned char* originalDataAll;
	int iIndex;
	int iFrameCount;
	int iFrameLength;
	int iPixelLength;
	HANDLE hDBThread;
};
