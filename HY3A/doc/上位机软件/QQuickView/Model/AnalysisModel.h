#pragma once
#include "../../Include/CommonHeader.h"
#include "windows.h"
#include <QThread>

//#define USETHREAD
class QTimer;

class AnalysisModel : public QObject
{
	Q_OBJECT

public:
	AnalysisModel(CameraParams* oneData, std::map<QString, SaveParams>* mapIdToSaveParamTemp);
	~AnalysisModel();	

	virtual bool insertGraphicData(BYTE* dataTemp, int iW, int iH, int iPL); //��������                             
	virtual void showData();             //��ʾ����

	void setBitStart(int iStartBitTemp); //�������ݽ�����ʼλ	
	void changeModel(QString strModel); 
	bool isHaveNewData();
	bool isStopThread();

private:	
	QTimer* timer;
	InformationParams info_recvData;

	CameraParams* currentCameraData;//ͼ�񴰿�����
	HANDLE hThread;
	FILE* fileWrite;	
	int iStartBit;                  //���ݽ�����ʼλ
	bool bThreadStop;
	bool bAnalysisFinish;
	std::map<QString, SaveParams>*  mapIdToSaveParam;   //ID��Ӧ�洢����
	std::vector<QThread*> vecThread;
	void createBuffer();                  //����������

public slots:
	virtual void Slot_TransformData();   //����ת��   
	void Slot_TimerRecvNewData();

signals:
	void Signal_TransFormData();
	void Signal_SendInfor(InformationParams);
};
