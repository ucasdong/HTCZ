#pragma once

#include <QObject>
#include <QString>
#include <functional>
#include <QSize>
#include <iostream>
#include <QPixmap>
#include <unordered_map> 
#include "../Model/SaveData.h"
#include <QVariant>

using namespace std;
typedef unsigned char BYTE;

#define BUFFER_COUNT        5

//#define SHOWTEST

enum displayDec
{
	Hex = 16,
	Dec = 10
};

enum SaveType
{
	SAVE_PACKAGE_DATA,    //�洢������
	SAVE_ORIGINAL_DATA,   //�洢ͼ��ԭʼ����
	SAVE_CAMERA_DATA,     //�洢�������
	SAVE_SENSOR_DATA,     //�洢Sensor����
	SAVE_SPECTRAL_DATA    //�洢�׶�����	
};

enum SaveFlag
{
	SAVECURFRAME,          //�洢��ǰ֡
	SAVEBYFRAME,          //��֡�洢
	SAVEBYTIME,           //��ʱ��洢
	SAVEBYLINES           //�������洢
};

namespace std {
	template<> struct hash<QString> {
		std::size_t operator()(const QString& s) const {
			return qHash(s);
		}
	};
}

enum InformationType
{
	INFO_DRIVER,         //driver��Ϣ
	INFO_MODEL           //model��Ϣ
};

enum InformationFlag
{
	INFO_STRING,          //�ַ�����Ϣ
	INFO_DOUBLE,          //������
	INFO_INT,             //����
};

struct InformationParams
{
	QString strID;                //��ϢID
	QString strName;              //��Ϣ����
	QString strDescription;       //��Ϣ����
	InformationType infoType;     //��Ϣ����(driver��Ϣ/model��Ϣ)
	InformationFlag infoFlag;     //���ݸ�ʽ
	QVariant variantValue;        //����ֵ	
};

struct SaveParams
{
	QString strDataID;            //�洢����ID
	QString strSavePath;          //�洢·��
	SaveType saveType;            //�洢����
	SaveFlag saveFlag;            //�洢��־
	qint64 iSaveTimeCount;        //�洢ʱ�䳤�ȣ���ʱ��洢��
	qint64 iSaveLineCount;        //�洢�������������洢��

	bool bInitFileFinish;         //�Ƿ�����ļ���ʼ��
	qint64 iStartTime;            //��ʼʱ��	
	qint64 iStartLine;            //��ʼ����	
	SaveData* saveDataThread;     //�ļ��洢����
};

struct SpectralParams
{
	QString strSpectral_ID;
	QString strSpectral_Name;
	int iSpectral_StartByte;	
	int iSpectral_Width;
	int iSpectral_Height;

	BYTE* CurOriginalSpectralData;//�׶���ʾ����
	BYTE* originalSpectralData;

#ifdef SHOWTEST
	BYTE* CurAnalyticalSpectralData;
	BYTE* analyticalSpectralData;
#endif // SHOWTEST
};

struct SensorParams
{
	QString strSensor_ID;          //Sensor����ID
	int iSensor_Analysis_ID;       //Sensor���ݽ���ID
	int iSensor_Sensor_StartByte;  //Sensor����IDƫ��
	QString strSensor_Name;        //Sensor��������
	QSize Sensor_Size;             //Sensor���ݴ�С
	int Sensor_FZ_Width;           //Sensor��������
	int Sensor_FZ_Height;          //Sensor��������

	BYTE* CurOriginalSensorData;     //��ǰ��ʾ��Sensor����
	BYTE* originalSensorData;

#ifdef SHOWTEST
	BYTE* CurAnalyticalSensorData;   //��ǰ�������Sensor���ݣ�16λƫ�ƣ�
	BYTE* analyticalSensorData;
#endif // SHOWTEST
	
	int iAnalysisIndex;
	QPixmap pixMap;
	unordered_map<QString, SpectralParams> allSpectralParams;//Sensor�ն���Ϣ
};

static uint swapI16(unsigned short v)
{
	return ((v & 0x00FF) << 8) |
		((v & 0xFF00) >> 8);
}

struct CameraParams
{
	QString Camera_ID;                          //���ID
	int Camera_Analysis_ID;                     //�������ID         
	int Camera_ID_Index;                        //���ID_INDEX
	int Camera_ID_IndexLength;                  //�����ʶ��ռ�ֽڳ���
	QString Camera_Name;                        //�������
	int Camera_AnalysisType;                    //������ݽ�������
	map<QString, int> Camera_Model;             //���ģʽ
	int Camera_RecvNothingTime;                 //���δ�յ�����ʱ�䣨������view����״̬��
	vector<int> Graphic_Lay;                    //ͼ�񲼾�
	int Sensor_ID_Index;                        //Sensor����
	int Sensor_ID_IndexLength;                  //Sensor������ռ�ֽڳ���
	int Sensor_Count;                           //Sensor����
	int Sensor_NumDiff;                         //Sensor��ID��ֵ��ֵ
	unordered_map<QString, SensorParams> mapSensor_Params;
	int One_Frame_LineCount;                    //һ֡ͼ������
	int One_Frame_LineLength;                   //һ��ͼ�����ݳ���
	int One_Frame_IncludeGraphicLines;          //֡���а���ͼ������
	int One_Line_FZ_Length;                     //һ���������ݵĳ���
	int One_FZ_Lines;                           //������������
	int One_Line_GraphicLength;                 //һ��ͼ�����ݳ���
	int One_Pixel_Length;                       //һ����Ԫ��ռ�ֽڸ���
	int Data_EndianFlag;                        //��С�˱�־��0����ˣ�1��С��
	int Data_SkipBits;                          //����ƫ��

	//���ݽ���ʹ��
	BYTE* CurCameraData;                        //��ǰ�����ʾ����
	BYTE* cameraData;                           //������ݻ�����
	BYTE* camera_FZ_data;                       //����������ݻ�����
	bool BHaveNewData;                          //�Ƿ����µ�����
	qint64 iCurrentTime;                        //��ǰ������ʱ�䣬�����ж��Ƿ�������
	int iDataIndexOfBuffer;
};

struct oneScreenParams
{
	QSize oneScreenLayout;                     //��Ļ����
	map<QString, int> Camera_ALLModel;         //���д���ģʽ
	vector<CameraParams*> vecCameraParams;     //һ����Ļ�ڵĴ�������
};

class CommonHeader : public QObject
{
	Q_OBJECT

public:
	CommonHeader(QObject *parent);
	~CommonHeader();
};
