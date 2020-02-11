#pragma once

#include <QObject>
#include "GrabImageData.h"//�Բⴴ������
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
	//�����ص�����
#ifdef SHOWTEST
	static void XferCallback(void *params);

	//����������(�Բ�)
	void AddCameraList(vector<CameraParams*> vecAllCameraParams);
	void AddCameraList(vector<CameraParams*> vecAllCameraParams);
#endif // SHOWTEST	
	static void XferDriverCallback(CDriverBuffer *params, void* classParam);  //�������ûص�����

	//���ù���Config����
	void SetProjectConfig(QJsonArray cameraConfig, std::string strDriverConfig);
	//��ʼ���������������ģ��
	void InitDriverAndDataAnalysis();
	//����
	void DestoryDriverAndDataAnalysis();
	//��ʼ�ɼ�����
	void StartGrabData();  
	//ֹͣ�ɼ�����
	void StopGrabData();
	//�����ID��ȡ����
	CameraParams* GetDataByCameraID(QString strCamera_ID);
	//��Sensor_ID��ȡ����
	SensorParams* GetDataBySensor_ID(QString strSensor_ID);
	//���ն�ID��ȡ����
	SpectralParams* GetDataBySpectralID(QString strSpectral_ID);
	
	//���ݴ洢
	void SaveModelData(SaveParams saveParam);
	//�������ݴ洢
	void SaveDriverData(emSaveType iSaveType, char* cPath);
	//ֹͣ�����洢����
	void StopDriverData(emSaveType iSaveType);
	//���ݻط�
	void ReplayDataStart(char* cPath);
	//���ݻط�ֹͣ
	void ReplayDataStop();
	//������λ
	void setStartBits(int iStartBit);

public:
	//��ȡConfig�ļ���ʼ�����Model
	void initConfig(QJsonArray cameraConfig);

//#ifdef SHOWTEST
//	GrabImageData* imageDriverData;                       //���ݲɼ���
//
//#else
	//����
	CDriverOperator* imageDriverData;
	CDriverBuffer* driverBuffer;	
//#endif
	//QTimer timer;
	std::string strDriverJson;
	std::map<QString, CameraParams*> mapCameraParams;  //���е��������
	std::map<QString, AnalysisModel*> mapAnalysisModel;//���е��������
	std::map<QString, SaveParams>  mapIdToSaveParam;   //ID��Ӧ�洢����

	//std::map<QString, std::pair<bool, SaveData*>> mapIdToSaveClass;
public slots:
	void Slot_InformationToView(InformationParams info);

signals:
	void Signal_Information(InformationParams info);
};
