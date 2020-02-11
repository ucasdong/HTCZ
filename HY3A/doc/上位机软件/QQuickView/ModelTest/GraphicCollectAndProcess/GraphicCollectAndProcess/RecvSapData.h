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
	BYTE byteCameraAdd;     //����ͨ����ַ
	int byteCameraID;       //�����ʶ
	DWORD dwOneDataLength;  //һ֡���ݵĳ���
	DWORD dwFrameCount;     //һ�δ����֡��
	DWORD dwPerPixLength;   //ÿ����Ԫ����
	//PickPkgContent* pickPkg;//�������
	PickPkg* pickPkg;//�������
	BYTE* recvData;         //��������
};

class RecvSapData:public QObject
{
	Q_OBJECT

public:
	RecvSapData();
	~RecvSapData();
	FILE *file;
	void initGrab();
	//�򴰿ڴ�������
	void setSubWnds(map<int, GraphicCollectAndProcess*>* mapTyep2Wnd, vector<oneCameraParams>* vecAllCamera);

public:
	//��������
	bool bStartOrStop;                             //��ʼ����ֹͣ�ɼ�
	SaveSettingWnd* saveWnd;                       //�洢����

	vector<BYTE> vecChannelAddress;                //ͨ����ַ������ʹ�ã�
	map<BYTE, CameraParams> mapChannelToCamParams; //ͨ����ʶ��Ӧ���������
	map<int, GraphicCollectAndProcess*> mapTypeToGraphicWnd;
	vector<oneCameraParams> vecAllCameraParams;
	map<QString, bool> mapTypeToShowState;        //����-��ʾ��������

	QString strReplayFile;
	int iCurrentModel;

	//2711ͼ��ɼ���ʽ
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
	 void slot_StartOrStopGrab(bool bFlag);//��ʼ����ֹͣ�ɼ�����
	 void slot_ReplayData(QString filePath, bool bStartOrStopTemp);
	 void slot_RecvDataMap(map<QString, bool>* mapTypeToShow);
	 void slot_SaveData();
	 void slot_ChangedModel(int iModel);
};

