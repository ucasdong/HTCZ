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
	SAVE_PACKAGE_DATA,    //存储包数据
	SAVE_ORIGINAL_DATA,   //存储图像原始数据
	SAVE_CAMERA_DATA,     //存储相机数据
	SAVE_SENSOR_DATA,     //存储Sensor数据
	SAVE_SPECTRAL_DATA    //存储谱段数据	
};

enum SaveFlag
{
	SAVECURFRAME,          //存储当前帧
	SAVEBYFRAME,          //按帧存储
	SAVEBYTIME,           //按时间存储
	SAVEBYLINES           //按行数存储
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
	INFO_DRIVER,         //driver信息
	INFO_MODEL           //model信息
};

enum InformationFlag
{
	INFO_STRING,          //字符串信息
	INFO_DOUBLE,          //浮点数
	INFO_INT,             //整数
};

struct InformationParams
{
	QString strID;                //信息ID
	QString strName;              //信息名称
	QString strDescription;       //信息描述
	InformationType infoType;     //信息类型(driver信息/model信息)
	InformationFlag infoFlag;     //数据格式
	QVariant variantValue;        //数据值	
};

struct SaveParams
{
	QString strDataID;            //存储对象ID
	QString strSavePath;          //存储路径
	SaveType saveType;            //存储类型
	SaveFlag saveFlag;            //存储标志
	qint64 iSaveTimeCount;        //存储时间长度（长时间存储）
	qint64 iSaveLineCount;        //存储行数（按行数存储）

	bool bInitFileFinish;         //是否完成文件初始化
	qint64 iStartTime;            //开始时间	
	qint64 iStartLine;            //开始行数	
	SaveData* saveDataThread;     //文件存储对象
};

struct SpectralParams
{
	QString strSpectral_ID;
	QString strSpectral_Name;
	int iSpectral_StartByte;	
	int iSpectral_Width;
	int iSpectral_Height;

	BYTE* CurOriginalSpectralData;//谱段显示数据
	BYTE* originalSpectralData;

#ifdef SHOWTEST
	BYTE* CurAnalyticalSpectralData;
	BYTE* analyticalSpectralData;
#endif // SHOWTEST
};

struct SensorParams
{
	QString strSensor_ID;          //Sensor数据ID
	int iSensor_Analysis_ID;       //Sensor数据解析ID
	int iSensor_Sensor_StartByte;  //Sensor数据ID偏移
	QString strSensor_Name;        //Sensor数据名称
	QSize Sensor_Size;             //Sensor数据大小
	int Sensor_FZ_Width;           //Sensor辅助数据
	int Sensor_FZ_Height;          //Sensor辅助数据

	BYTE* CurOriginalSensorData;     //当前显示的Sensor数据
	BYTE* originalSensorData;

#ifdef SHOWTEST
	BYTE* CurAnalyticalSensorData;   //当前解析后的Sensor数据（16位偏移）
	BYTE* analyticalSensorData;
#endif // SHOWTEST
	
	int iAnalysisIndex;
	QPixmap pixMap;
	unordered_map<QString, SpectralParams> allSpectralParams;//Sensor普段信息
};

static uint swapI16(unsigned short v)
{
	return ((v & 0x00FF) << 8) |
		((v & 0xFF00) >> 8);
}

struct CameraParams
{
	QString Camera_ID;                          //相机ID
	int Camera_Analysis_ID;                     //相机解析ID         
	int Camera_ID_Index;                        //相机ID_INDEX
	int Camera_ID_IndexLength;                  //相机标识所占字节长度
	QString Camera_Name;                        //相机名称
	int Camera_AnalysisType;                    //相机数据解析类型
	map<QString, int> Camera_Model;             //相机模式
	int Camera_RecvNothingTime;                 //相机未收到数据时间（用于向view返回状态）
	vector<int> Graphic_Lay;                    //图像布局
	int Sensor_ID_Index;                        //Sensor索引
	int Sensor_ID_IndexLength;                  //Sensor索引所占字节长度
	int Sensor_Count;                           //Sensor个数
	int Sensor_NumDiff;                         //Sensor—ID数值差值
	unordered_map<QString, SensorParams> mapSensor_Params;
	int One_Frame_LineCount;                    //一帧图像行数
	int One_Frame_LineLength;                   //一行图像数据长度
	int One_Frame_IncludeGraphicLines;          //帧行中包含图像行数
	int One_Line_FZ_Length;                     //一条辅助数据的长度
	int One_FZ_Lines;                           //辅助数据条数
	int One_Line_GraphicLength;                 //一行图像数据长度
	int One_Pixel_Length;                       //一个像元所占字节个数
	int Data_EndianFlag;                        //大小端标志，0：大端，1：小端
	int Data_SkipBits;                          //数据偏移

	//数据解析使用
	BYTE* CurCameraData;                        //当前相机显示数据
	BYTE* cameraData;                           //相机数据缓冲区
	BYTE* camera_FZ_data;                       //相机辅助数据缓冲区
	bool BHaveNewData;                          //是否有新的数据
	qint64 iCurrentTime;                        //当前来数据时间，用于判断是否有新数
	int iDataIndexOfBuffer;
};

struct oneScreenParams
{
	QSize oneScreenLayout;                     //屏幕布局
	map<QString, int> Camera_ALLModel;         //所有窗口模式
	vector<CameraParams*> vecCameraParams;     //一个屏幕内的窗口配置
};

class CommonHeader : public QObject
{
	Q_OBJECT

public:
	CommonHeader(QObject *parent);
	~CommonHeader();
};
