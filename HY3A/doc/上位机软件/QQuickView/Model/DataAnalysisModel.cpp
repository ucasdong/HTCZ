#include "DataAnalysisModel.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonObject>
#include <QDateTime>

DataAnalysisModel::DataAnalysisModel()
{
	//initConfig(strConfigFileName);
}

DataAnalysisModel::~DataAnalysisModel()
{
	DestoryDriverAndDataAnalysis();
}
//驱动回调函数
#ifdef SHOWTEST
void DataAnalysisModel::XferCallback(void *params)
{
	DataAnalysisModel *pDlg = static_cast<DataAnalysisModel *>(params);
	BYTE* dataPtr = NULL;
	pDlg->imageDriverData->GetAddress((void**)&dataPtr);

	for (std::map<QString, AnalysisModel*>::iterator itAnalysis = pDlg->mapAnalysisModel.begin(); itAnalysis != pDlg->mapAnalysisModel.end(); itAnalysis++)
	{
		bool bInsertFinish = itAnalysis->second->insertGraphicData(dataPtr, 3534, 430, 2);
		if (bInsertFinish)
		{
			break;
		}
	}
}
#endif

void DataAnalysisModel::XferDriverCallback(CDriverBuffer *params, void* classParam)
{
	DataAnalysisModel *pDlg = static_cast<DataAnalysisModel *>(classParam);
	void* dataPtr = NULL;
	params->GetAddress((void**)&dataPtr);
	int iWidth = params->GetWidth();
	int iHeight = params->GetHeight();
	int iPixLen = params->GetBytesPerPixel();
	for (std::map<QString, AnalysisModel*>::iterator itAnalysis = pDlg->mapAnalysisModel.begin(); itAnalysis != pDlg->mapAnalysisModel.end(); itAnalysis++)
	{
		bool bInsertFinish = itAnalysis->second->insertGraphicData((BYTE*)dataPtr, iWidth, iHeight, iPixLen);
		if (bInsertFinish)
		{
			break;
		}
	}
}

//初始化驱动和相机解析模块
void DataAnalysisModel::InitDriverAndDataAnalysis()
{
	//创建驱动
#ifdef SHOWTEST
	//自测
	imageDriverData = new GrabImageData(NULL);//自测
	imageDriverData->RegisterCallBack(XferCallback, this);
#else
	//驱动
	imageDriverData = new CDriverOperator();
	int a =imageDriverData->InitOperator(strDriverJson);
	imageDriverData->RegisterRecvBufferCallback(XferDriverCallback, this);
#endif // SHOWTEST

	//创建相机解析对象
	for (std::map<QString, CameraParams*>::iterator itCameraParam = mapCameraParams.begin(); itCameraParam != mapCameraParams.end(); itCameraParam++)   //所有的相机参数)
	{
		AnalysisModel* analysisModel = new AnalysisModel(itCameraParam->second, &mapIdToSaveParam);
		mapAnalysisModel.insert(std::pair<QString, AnalysisModel*>(itCameraParam->first, analysisModel));   //所有的相机解析
		connect(analysisModel, SIGNAL(Signal_SendInfor(InformationParams)), this, SLOT(Slot_InformationToView(InformationParams)));
	}
}
//销毁
void DataAnalysisModel::DestoryDriverAndDataAnalysis()
{
	//销毁驱动层
	imageDriverData->StopGrab();
	delete imageDriverData;
	imageDriverData = NULL;
	
	//销毁数据解析层
	for (std::map<QString, AnalysisModel*>::iterator itModel = mapAnalysisModel.begin(); itModel != mapAnalysisModel.end(); itModel++)
	{
		delete itModel->second;
		itModel->second = NULL;
	}
	mapAnalysisModel.clear();

	for (std::map<QString, CameraParams*>::iterator itCamera = mapCameraParams.begin(); itCamera != mapCameraParams.end(); itCamera++)
	{
		delete itCamera->second;
		itCamera->second = NULL;
	}
	mapCameraParams.clear();
}
//开始采集数据
void DataAnalysisModel::StartGrabData()
{
	imageDriverData->StartGrab();
	//imageDriverData->StartSave(1, "F:\DATA.RAW");
}
//停止采集数据
void DataAnalysisModel::StopGrabData()
{
	imageDriverData->StopGrab();
}
//按相机ID获取数据
CameraParams* DataAnalysisModel::GetDataByCameraID(QString strCamera_ID)
{
	std::map<QString, CameraParams*>::iterator itCameraData = mapCameraParams.find(strCamera_ID);   //所有的相机参数
	if (itCameraData != mapCameraParams.end())
	{
		return itCameraData->second;
	}
	return NULL;
}
//按Sensor_ID获取数据
SensorParams* DataAnalysisModel::GetDataBySensor_ID(QString strSensor_ID)
{
	for (std::map<QString, CameraParams*>::iterator itCameraData = mapCameraParams.begin(); itCameraData != mapCameraParams.end(); itCameraData++)
	{
		unordered_map<QString, SensorParams>::iterator itSensor = itCameraData->second->mapSensor_Params.find(strSensor_ID);
		if (itSensor != itCameraData->second->mapSensor_Params.end())
		{
			return &itSensor->second;
		}
	}
	return NULL;
}
//按普段ID获取数据
SpectralParams* DataAnalysisModel::GetDataBySpectralID(QString strSpectral_ID)
{
	for (std::map<QString, CameraParams*>::iterator itCameraData = mapCameraParams.begin(); itCameraData != mapCameraParams.end(); itCameraData++)
	{
		for (unordered_map<QString, SensorParams>::iterator itSensor = itCameraData->second->mapSensor_Params.begin(); itSensor != itCameraData->second->mapSensor_Params.end(); itSensor++)
		{
			unordered_map<QString, SpectralParams>::iterator itSprectral = itSensor->second.allSpectralParams.find(strSpectral_ID);
			if (itSprectral != itSensor->second.allSpectralParams.end())
			{
				return &itSprectral->second;
			}
		}
	}
	return NULL;
}

#ifdef SHOWTEST
void DataAnalysisModel::AddCameraList(vector<CameraParams*> vecAllCameraParams)
{
	for (vector<CameraParams*>::iterator itCamera = vecAllCameraParams.begin(); itCamera != vecAllCameraParams.end(); itCamera++)
	{
		mapCameraParams.insert(std::pair<QString, CameraParams*>((*itCamera)->Camera_ID, *itCamera));
	}
}
#endif

void DataAnalysisModel::SetProjectConfig(QJsonArray cameraConfig, std::string strDriverConfig)
{
	//配置传递给驱动
	strDriverJson = strDriverConfig;
	//初始化解析
	initConfig(cameraConfig);
}

//数据存储
void DataAnalysisModel::SaveModelData(SaveParams saveParam)
{
	if (saveParam.saveType != SAVE_CAMERA_DATA && saveParam.saveType != SAVE_SENSOR_DATA && saveParam.saveType != SAVE_SPECTRAL_DATA)
	{
		return;
	}
	if (saveParam.saveFlag == SAVECURFRAME)
	{
		//按相机ID获取数据
		BYTE* saveData = NULL;
		int iLen = 0;
		
		CameraParams* cameraParam = GetDataByCameraID(saveParam.strDataID);
		if (cameraParam != NULL)
		{
			saveData = cameraParam->CurCameraData;
			iLen = cameraParam->One_Frame_LineCount * cameraParam->One_Frame_LineLength * cameraParam->One_Pixel_Length;
		}
		//按Sensor_ID获取数据
		SensorParams* sensorData = GetDataBySensor_ID(saveParam.strDataID);
		if (sensorData != NULL)
		{
			saveData = sensorData->CurOriginalSensorData;
			iLen = sensorData->Sensor_Size.width() * sensorData->Sensor_Size.height() * 2;			
		}
		//按普段ID获取数据
		SpectralParams* spectral = GetDataBySpectralID(saveParam.strDataID);
		if (spectral != NULL)
		{
			saveData = spectral->CurOriginalSpectralData;
			iLen = spectral->iSpectral_Width * spectral->iSpectral_Height * 2;
		}

		if (iLen > 0)
		{
			SaveData* saveDataThread = new SaveData;     //文件存储对象
			saveDataThread->initFile(saveParam.strSavePath, 1);
			saveDataThread->insertData(cameraParam->CurCameraData, iLen, true);
			delete saveDataThread;
			saveDataThread = NULL;
		}
	}
	else
	{
		std::map<QString, SaveParams>::iterator itSaveParam = mapIdToSaveParam.find(saveParam.strDataID);   //ID对应存储对象
		if (itSaveParam == mapIdToSaveParam.end())
		{
			saveParam.saveDataThread = NULL;
			saveParam.bInitFileFinish = false;
			if (saveParam.saveFlag == SAVEBYTIME)
			{
				saveParam.iStartTime = QDateTime::currentMSecsSinceEpoch();
			}
			else if (saveParam.saveFlag == SAVEBYLINES)
			{
				saveParam.iStartLine = 0;
			}
			mapIdToSaveParam.insert(std::pair<QString, SaveParams>(saveParam.strDataID, saveParam));
		}
	}	
}

//驱动数据存储
void DataAnalysisModel::SaveDriverData(emSaveType iSaveType, char* cPath)
{
	imageDriverData->StartSave(iSaveType, cPath);
}

//停止驱动存储数据
void DataAnalysisModel::StopDriverData(emSaveType iSaveType)
{
	imageDriverData->StopSave(iSaveType);
}

void DataAnalysisModel::ReplayDataStart(char* cPath)
{
	imageDriverData->StartPlaybackGrab(cPath);
}

void DataAnalysisModel::ReplayDataStop()
{
	imageDriverData->StopPlaybackGrab();
}

void DataAnalysisModel::setStartBits(int iStartBit)
{
	for (std::map<QString, AnalysisModel*>::iterator itAnalysis = mapAnalysisModel.begin(); itAnalysis != mapAnalysisModel.end(); itAnalysis++)
	{
		itAnalysis->second->setBitStart(iStartBit);
	}
}

//读取Config文件初始化相机数据
void DataAnalysisModel::initConfig(QJsonArray cameraConfig)
{
	mapCameraParams.clear();
	
	QJsonParseError error;
	foreach(QJsonValue cameraValue, cameraConfig)
	{
		QJsonObject objCamera = cameraValue.toObject();

		CameraParams* oneCameraData = new CameraParams;
		oneCameraData->Camera_ID = objCamera.value("Camera_ID").toString();
		oneCameraData->Camera_Analysis_ID = objCamera.value("Camera_Analysis_ID").toString().toInt(nullptr, 16);
		oneCameraData->Camera_ID_Index = objCamera.value("Camera_ID_Index").toInt();
		oneCameraData->Camera_ID_IndexLength = objCamera.value("Camera_ID_IndexLength").toInt();
		oneCameraData->Camera_Name = objCamera.value("Camera_Name").toString();
		oneCameraData->Camera_AnalysisType = objCamera.value("Camera_AnalysisType").toInt();

		QJsonArray modelArray = objCamera.value("Camera_Model").toArray();
		foreach(QJsonValue modelValue, cameraConfig)
		{
			QJsonObject objModel = cameraValue.toObject();
			QString strModelName = objModel.value("Model_Name").toString();
			int iModelLength = objModel.value("Model_length").toInt();
			oneCameraData->Camera_Model.insert(std::pair<QString, int>(strModelName, iModelLength));
		}
		oneCameraData->Camera_RecvNothingTime = objCamera.value("Camera_RecvNothingTime").toInt();
		oneCameraData->One_Frame_LineCount = objCamera.value("One_Frame_LineCount").toInt();
		oneCameraData->One_Frame_LineLength = objCamera.value("One_Frame_LineLength").toInt();
		oneCameraData->One_Frame_IncludeGraphicLines = objCamera.value("One_Frame_IncludeGraphicLines").toInt();
		oneCameraData->One_Line_FZ_Length = objCamera.value("One_Line_FZ_Length").toInt();
		oneCameraData->One_FZ_Lines = objCamera.value("One_FZ_Lines").toInt();
		oneCameraData->One_Line_GraphicLength = objCamera.value("One_Line_GraphicLength").toInt();
		oneCameraData->One_Pixel_Length = objCamera.value("One_Pixel_Length").toInt();
		oneCameraData->Data_EndianFlag = objCamera.value("Data_EndianFlag").toInt();
		oneCameraData->Data_SkipBits = objCamera.value("Data_SkipBits").toInt();

		QJsonArray layArray = objCamera.value("Sensor_Lay").toArray();
		foreach(QJsonValue layValue, layArray)
		{
			oneCameraData->Graphic_Lay.push_back(layValue.toInt());
		}
		oneCameraData->Sensor_ID_Index = objCamera.value("Sensor_ID_Index").toInt();
		oneCameraData->Sensor_ID_IndexLength = objCamera.value("Sensor_ID_IndexLength").toInt();
		oneCameraData->Sensor_Count = objCamera.value("Sensor_Count").toInt();
		oneCameraData->Sensor_NumDiff = objCamera.value("Sensor_NumDiff").toInt();
		QJsonArray SensorArray = objCamera.value("Sensor_NamesAndSizes").toArray();
		foreach(QJsonValue ccdValue, SensorArray)
		{
			SensorParams oneSensorTemp;

			QJsonObject objSensor = ccdValue.toObject();
			oneSensorTemp.strSensor_ID = objSensor.value("Sensor_ID").toString();
			oneSensorTemp.iSensor_Analysis_ID = objSensor.value("Sensor_Analysis_ID").toString().toInt(nullptr, 16);
			oneSensorTemp.iSensor_Sensor_StartByte = objSensor.value("Sensor_StartByte").toInt();
			oneSensorTemp.strSensor_Name = objSensor.value("Sensor_Name").toString();
			oneSensorTemp.Sensor_Size.setWidth(objSensor.value("Sensor_Width").toInt());
			oneSensorTemp.Sensor_Size.setHeight(objSensor.value("Sensor_Height").toInt());
			oneSensorTemp.Sensor_FZ_Width = objSensor.value("Sensor_FZ_Width").toInt();
			oneSensorTemp.Sensor_FZ_Height = objSensor.value("Sensor_FZ_Height").toInt();

			QJsonArray SpectralArray = objSensor.value("Sensor_SpectralList").toArray();
			foreach(auto SpectralValue, SpectralArray)
			{
				SpectralParams oneSpectralTemp;
				QJsonObject objSpectral = SpectralValue.toObject();
				oneSpectralTemp.strSpectral_ID = objSpectral.value("Spectral_ID").toString();
				oneSpectralTemp.strSpectral_Name = objSpectral.value("Spectral_Name").toString();
				oneSpectralTemp.iSpectral_StartByte = objSpectral.value("Spectral_StartByte").toInt();
				oneSpectralTemp.iSpectral_Width = objSpectral.value("Spectral_Width").toInt();
				oneSpectralTemp.iSpectral_Height = objSpectral.value("Spectral_Height").toInt();
				oneSensorTemp.allSpectralParams.insert(std::pair<QString, SpectralParams>(oneSpectralTemp.strSpectral_ID, oneSpectralTemp));
			}
			oneCameraData->mapSensor_Params.insert(std::make_pair(oneSensorTemp.strSensor_ID, oneSensorTemp));
		}

		mapCameraParams.insert(std::pair<QString, CameraParams*>(oneCameraData->Camera_ID, oneCameraData));
	}
}

void DataAnalysisModel::Slot_InformationToView(InformationParams info)
{
	emit Signal_Information(info);
}
