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
	void createGraphicData();                 //��������

	CallbackFunc  m_pFunc;
	void*         m_param;
	void GetAddress(void** pData);

private:
	bool bStartGrab; //��ʼ��ֹͣ�ɼ�
	bool bStopThread;//�����߳�
	unsigned char* originalDataAll;
	int iIndex;
	int iFrameCount;
	int iFrameLength;
	int iPixelLength;
	HANDLE hDBThread;
};
