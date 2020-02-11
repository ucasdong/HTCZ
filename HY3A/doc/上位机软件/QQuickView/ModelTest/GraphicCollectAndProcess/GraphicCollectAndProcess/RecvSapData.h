#pragma once

#include <vector>
#include <iostream>
#include "SapClassBasic.h"
//#include "PickPkgContent.h"
#include "PickPkg.h"
#include "GraphicCollectAndProcess.h"
#include "Common.h"
#include "SaveSettingWnd.h"
#include <QMutex>

struct CameraParams
{
	BYTE byteCameraAdd;     //虚拟通道地址
	int byteCameraID;       //相机标识
	DWORD dwOneDataLength;  //一帧数据的长度
	DWORD dwFrameCount;     //一次传入的帧数
	DWORD dwPerPixLength;   //每个像元长度
	//PickPkgContent* pickPkg;//解包对象
	PickPkg* pickPkg;//解包对象
	BYTE* recvData;         //接收数据
};

class RecvSapData:public QObject
{
	Q_OBJECT

public:
	RecvSapData();
	~RecvSapData();
	FILE *file;
	void initGrab();
	//向窗口传递数据
	void setSubWnds(map<int, GraphicCollectAndProcess*>* mapTyep2Wnd, vector<oneCameraParams>* vecAllCamera);

public:
	//公共部分
	bool bStartOrStop;                             //开始或者停止采集
	SaveSettingWnd* saveWnd;                       //存储设置

	vector<BYTE> vecChannelAddress;                //通道地址（遍历使用）
	map<BYTE, CameraParams> mapChannelToCamParams; //通道标识对应的相机数据
	map<int, GraphicCollectAndProcess*> mapTypeToGraphicWnd;
	vector<oneCameraParams> vecAllCameraParams;
	map<QString, bool> mapTypeToShowState;        //类型-显示或者隐藏

	QString strReplayFile;
	int iCurrentModel;

	//2711图像采集方式
	SapAcquisition*    m_Acq_2711;
	SapBuffer*         m_Buffer_2711;
	SapTransfer*       m_Xfer_2711;
	bool               run_thread_2711;

	void init_grab_2711();
	void SplitBuffer_2711();
	static void XferCallback_2711(SapXferCallbackInfo *pInfo);
	static void Test_2711(void *params, int index);

	void Grab_2711();
	void Stop_2711();
	//int  ReBoot(DWORD verInfo);
	BOOL CreateObjects_2711();
	BOOL DestroyObjects_2711();
// 
 public slots:
	 void slot_StartOrStopGrab(bool bFlag);//开始或者停止采集数据
	 void slot_ReplayData(QString filePath, bool bStartOrStopTemp);
	 void slot_RecvDataMap(map<QString, bool>* mapTypeToShow);
	 void slot_SaveData();
	 void slot_ChangedModel(int iModel);
};

