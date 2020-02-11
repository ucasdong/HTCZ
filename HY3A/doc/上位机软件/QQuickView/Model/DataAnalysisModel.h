#pragma once

#include <QObject>
#include "GrabImageData.h"//自测创建数据
#include "AnalysisModel.h"
#include "../Driver/interface/CDriverOperator.h"


typedef unsigned char BYTE;

class QJsonArray;

class DataAnalysisModel : public QObject
{
	Q_OBJECT

public:
	DataAnalysisModel();
	~DataAnalysisModel();

public:
	//驱动回调函数
#ifdef SHOWTEST
	static void XferCallback(void *params);

	//添加相机数据(自测)
	void AddCameraList(vector<CameraParams*> vecAllCameraParams);
	void AddCameraList(vector<CameraParams*> vecAllCameraParams);
#endif // SHOWTEST	
	static void XferDriverCallback(CDriverBuffer *params, void* classParam);  //驱动调用回调函数

	//设置工程Config配置
	void SetProjectConfig(QJsonArray cameraConfig, std::string strDriverConfig);
	//初始化驱动和相机解析模块
	void InitDriverAndDataAnalysis();
	//销毁
	void DestoryDriverAndDataAnalysis();
	//开始采集数据
	void StartGrabData();  
	//停止采集数据
	void StopGrabData();
	//按相机ID获取数据
	CameraParams* GetDataByCameraID(QString strCamera_ID);
	//按Sensor_ID获取数据
	SensorParams* GetDataBySensor_ID(QString strSensor_ID);
	//按普段ID获取数据
	SpectralParams* GetDataBySpectralID(QString strSpectral_ID);
	
	//数据存储
	void SaveModelData(SaveParams saveParam);
	//驱动数据存储
	void SaveDriverData(emSaveType iSaveType, char* cPath);
	//停止驱动存储数据
	void StopDriverData(emSaveType iSaveType);
	//数据回放
	void ReplayDataStart(char* cPath);
	//数据回放停止
	void ReplayDataStop();
	//数据移位
	void setStartBits(int iStartBit);

public:
	//读取Config文件初始化相机Model
	void initConfig(QJsonArray cameraConfig);

//#ifdef SHOWTEST
//	GrabImageData* imageDriverData;                       //数据采集类
//
//#else
	//驱动
	CDriverOperator* imageDriverData;
	CDriverBuffer* driverBuffer;	
//#endif
	//QTimer timer;
	std::string strDriverJson;
	std::map<QString, CameraParams*> mapCameraParams;  //所有的相机参数
	std::map<QString, AnalysisModel*> mapAnalysisModel;//所有的相机解析
	std::map<QString, SaveParams>  mapIdToSaveParam;   //ID对应存储对象

	//std::map<QString, std::pair<bool, SaveData*>> mapIdToSaveClass;
public slots:
	void Slot_InformationToView(InformationParams info);

signals:
	void Signal_Information(InformationParams info);
};
