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

	virtual bool insertGraphicData(BYTE* dataTemp, int iW, int iH, int iPL); //接收数据                             
	virtual void showData();             //显示数据

	void setBitStart(int iStartBitTemp); //设置数据解析起始位	
	void changeModel(QString strModel); 
	bool isHaveNewData();
	bool isStopThread();

private:	
	QTimer* timer;
	InformationParams info_recvData;

	CameraParams* currentCameraData;//图像窗口配置
	HANDLE hThread;
	FILE* fileWrite;	
	int iStartBit;                  //数据解析起始位
	bool bThreadStop;
	bool bAnalysisFinish;
	std::map<QString, SaveParams>*  mapIdToSaveParam;   //ID对应存储对象
	std::vector<QThread*> vecThread;
	void createBuffer();                  //创建缓冲区

public slots:
	virtual void Slot_TransformData();   //数据转换   
	void Slot_TimerRecvNewData();

signals:
	void Signal_TransFormData();
	void Signal_SendInfor(InformationParams);
};
