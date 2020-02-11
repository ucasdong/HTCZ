#include "AnalysisModel.h"
#include <QTimer>
#include <QDebug>
#ifdef USETHREAD
DWORD WINAPI analysisDataModel(void* groupBox)
{
	AnalysisModel* groupBoxTemp = static_cast<AnalysisModel*>(groupBox);
	if (groupBoxTemp != NULL)
	{
		while (1)
		{
			if (groupBoxTemp->isHaveNewData())
			{
				groupBoxTemp->Slot_TransformData();
			}
			if (groupBoxTemp->isStopThread())
			{
				break;
			}
		}
	}
	return 0;
}

#endif // USETHREAD



AnalysisModel::AnalysisModel(CameraParams* oneData, std::map<QString, SaveParams>* mapIdToSaveParamTemp)
{
	currentCameraData = oneData;
	mapIdToSaveParam = mapIdToSaveParamTemp;
	iStartBit = 0;

	bThreadStop = false;
	createBuffer();
	bAnalysisFinish = true;
	//fileWrite = fopen("d:/dataShow.raw", "w+b");
#ifdef USETHREAD
	hThread = CreateThread(NULL, 0, analysisDataModel, this, 0, NULL);
#else	
	connect(this, SIGNAL(Signal_TransFormData()), this, SLOT(Slot_TransformData()));
#endif USETHREAD
	timer = new QTimer;
	connect(timer, SIGNAL(timeout()), this, SLOT(Slot_TimerRecvNewData()));
	timer->start(currentCameraData->Camera_RecvNothingTime * 1000);
	info_recvData.strID = currentCameraData->Camera_ID;
	info_recvData.strName = currentCameraData->Camera_ID;
	info_recvData.strDescription = QStringLiteral("未收到数据");
	info_recvData.infoType = INFO_MODEL;
	info_recvData.infoFlag = INFO_INT;
	info_recvData.variantValue.setValue(0);
}

AnalysisModel::~AnalysisModel()
{
	timer->stop();
	delete timer;
	timer = NULL;
	//fclose(fileWrite);
	currentCameraData->BHaveNewData = false;

#ifdef USETHREAD
	bThreadStop = true;
	WaitForSingleObject(hThread, INFINITE);
#endif

	//释放相机缓存
	if (currentCameraData->cameraData != NULL)
	{
		delete currentCameraData->cameraData;
		currentCameraData->cameraData = NULL;
	}
	for (int i = 0; i < vecThread.size(); i++)
	{
		delete vecThread[i];
	}
	vecThread.clear();
	//释放辅助数据缓存(待加)
	//delete currentCameraData->camera_FZ_data;
	//currentCameraData->camera_FZ_data = NULL;	

	//释放Sensor缓存
	for (unordered_map<QString, SensorParams>::iterator itSensor = currentCameraData->mapSensor_Params.begin(); itSensor != currentCameraData->mapSensor_Params.end(); itSensor++)
	{
		delete itSensor->second.originalSensorData;
		itSensor->second.originalSensorData = NULL;

#ifdef SHOWTEST
		delete itSensor->second.analyticalSensorData;
		itSensor->second.analyticalSensorData = NULL;
#endif // SHOWTEST
		
		//释放谱段缓存
		for (unordered_map<QString, SpectralParams>::iterator itSpectral = itSensor->second.allSpectralParams.begin(); itSpectral != itSensor->second.allSpectralParams.end(); itSpectral++)
		{
		}
	}
}

bool AnalysisModel::insertGraphicData(BYTE* dataTemp, int iW, int iH, int iPL)
{
	if (bThreadStop)
	{
		return false;
	}
	int iCameraIndex = 0;
	for (int i = currentCameraData->Camera_ID_Index * currentCameraData->One_Pixel_Length; i < currentCameraData->Camera_ID_IndexLength * currentCameraData->One_Pixel_Length; i++)
	{
		iCameraIndex = iCameraIndex * 256;
		iCameraIndex += dataTemp[currentCameraData->Camera_ID_Index + i];
	}
	if (iCameraIndex != currentCameraData->Camera_Analysis_ID)
	{
		return false;
	}
	else
	{
		if ((currentCameraData->iDataIndexOfBuffer + 1) >= BUFFER_COUNT)
		{
			currentCameraData->iDataIndexOfBuffer = 0;
		}
		else
		{
			currentCameraData->iDataIndexOfBuffer++;
		}
		int iLen = currentCameraData->One_Frame_LineCount * currentCameraData->One_Frame_LineLength * currentCameraData->One_Pixel_Length;
		memcpy(currentCameraData->cameraData + currentCameraData->iDataIndexOfBuffer * iLen, dataTemp, iLen);
		qDebug() << "exOfBuffer * iLen, dataTemp, iLen);";
		currentCameraData->CurCameraData = currentCameraData->cameraData + currentCameraData->iDataIndexOfBuffer * iLen;
		std::map<QString, SaveParams>::iterator itSaveCamera = mapIdToSaveParam->find(currentCameraData->Camera_ID);
		if (itSaveCamera != mapIdToSaveParam->end())
		{
			bool bSaveFinishCamera = false;
			if (itSaveCamera->second.saveFlag == SAVEBYFRAME)
			{
				bSaveFinishCamera = true;
			}
			else if (itSaveCamera->second.saveFlag == SAVEBYTIME)
			{
				if (QDateTime::currentMSecsSinceEpoch() - itSaveCamera->second.iStartTime >= itSaveCamera->second.iSaveTimeCount * 1000)
				{
					bSaveFinishCamera = true;
				}
			}
			else if (itSaveCamera->second.saveFlag == SAVEBYLINES)
			{
				if (itSaveCamera->second.iStartLine + currentCameraData->One_Frame_LineCount >= itSaveCamera->second.iSaveLineCount)
				{
					iLen = (itSaveCamera->second.iSaveLineCount - itSaveCamera->second.iStartLine)
						* currentCameraData->One_Frame_LineLength * currentCameraData->One_Pixel_Length;
					bSaveFinishCamera = true;
				}
				else
				{
					itSaveCamera->second.iStartLine += currentCameraData->One_Frame_LineCount;
				}
			}
			if (!itSaveCamera->second.bInitFileFinish)
			{
				QThread* threadTemp = new QThread;
				vecThread.push_back(threadTemp);
				itSaveCamera->second.saveDataThread = new SaveData;
				itSaveCamera->second.saveDataThread->moveToThread(threadTemp);
				threadTemp->start();
				itSaveCamera->second.bInitFileFinish = true;
				//QString strFileName = itSaveCamera->second.strSavePath + QString("/") + itSaveSensor->first + QString("_")
				//	+ QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
				itSaveCamera->second.saveDataThread->initFile(itSaveCamera->second.strSavePath, 1);
			}
			itSaveCamera->second.saveDataThread->insertData(currentCameraData->CurCameraData, iLen, bSaveFinishCamera);
			if (bSaveFinishCamera)
			{
				delete itSaveCamera->second.saveDataThread;
				itSaveCamera->second.saveDataThread = NULL;
				mapIdToSaveParam->erase(itSaveCamera);
			}
		}
		currentCameraData->BHaveNewData = true;

#ifndef USETHREAD
		emit Signal_TransFormData();
#endif // !USETHREAD		
	}
	currentCameraData->iCurrentTime = QDateTime::currentMSecsSinceEpoch();
	return true;
}

void AnalysisModel::Slot_TransformData()
{
	if (bAnalysisFinish && currentCameraData->BHaveNewData)
	{
		bAnalysisFinish = false;
		int iFrameCount = currentCameraData->One_Frame_LineCount;
		int iFrameLength = currentCameraData->One_Frame_LineLength;
		int iPixelLen = currentCameraData->One_Pixel_Length;
//		int iGraphicIdIndex = currentCameraData->Graphic_ID_Index;
//		int iGraphicIdlength = currentCameraData->Graphic_ID_IndexLength;

		int iFrameLines = currentCameraData->One_Frame_IncludeGraphicLines;
		//int iFZLength = currentCameraData->One_Line_FZ_Length;
		//int iGraphicLength = currentCameraData->One_Line_GraphicLength;

		int iIndex = currentCameraData->iDataIndexOfBuffer;
		BYTE* oneFrameData = currentCameraData->cameraData + iIndex * iFrameCount * iFrameLength * iPixelLen;

		for (unordered_map<QString, SensorParams>::iterator itSensorParam = currentCameraData->mapSensor_Params.begin(); itSensorParam != currentCameraData->mapSensor_Params.end(); itSensorParam++)
		{
			int iGraphicIndex = 0;
			for (int i = 0; i < currentCameraData->Sensor_ID_IndexLength * iPixelLen; i++)
			{
				iGraphicIndex = iGraphicIndex * 256;
				//int iNum = (currentCameraData->Graphic_ID_Index + itGraphic->second.iGraphicDataStart) * iPixelLen + i;
				iGraphicIndex += oneFrameData[(currentCameraData->Sensor_ID_Index + itSensorParam->second.iSensor_Sensor_StartByte) * iPixelLen + i];
			}
			if (iGraphicIndex != itSensorParam->second.iSensor_Analysis_ID)//图像标识不一致
			{
				continue;
			}
			int iLen = itSensorParam->second.Sensor_Size.width() * itSensorParam->second.Sensor_Size.height();
			BYTE* originalGraphicData = itSensorParam->second.originalSensorData + itSensorParam->second.iAnalysisIndex * iLen * iPixelLen;
			itSensorParam->second.CurOriginalSensorData = originalGraphicData;
			memcpy(originalGraphicData, oneFrameData + itSensorParam->second.iSensor_Sensor_StartByte * iPixelLen, iLen * iPixelLen);
			std::map<QString, SaveParams>::iterator itSaveSensor = mapIdToSaveParam->find(itSensorParam->first);
			if (itSaveSensor != mapIdToSaveParam->end())
			{
				bool bSaveFinishSensor = false;
				if (itSaveSensor->second.saveFlag == SAVEBYFRAME)
				{
					bSaveFinishSensor = true;
				}
				else if (itSaveSensor->second.saveFlag == SAVEBYTIME)
				{
					if (QDateTime::currentMSecsSinceEpoch() - itSaveSensor->second.iStartTime >= itSaveSensor->second.iSaveTimeCount * 1000)
					{
						bSaveFinishSensor = true;
					}
				}
				else if (itSaveSensor->second.saveFlag == SAVEBYLINES)
				{
					if (itSaveSensor->second.iStartLine + itSensorParam->second.Sensor_Size.height() >= itSaveSensor->second.iSaveLineCount)
					{
						iLen = (itSaveSensor->second.iSaveLineCount - itSaveSensor->second.iStartLine) * itSensorParam->second.Sensor_Size.width();
						bSaveFinishSensor = true;
					}
					else
					{
						itSaveSensor->second.iStartLine += itSensorParam->second.Sensor_Size.height();
					}
				}
				if (!itSaveSensor->second.bInitFileFinish)
				{
					QThread* threadTemp = new QThread;
					vecThread.push_back(threadTemp);
					itSaveSensor->second.saveDataThread = new SaveData;
					itSaveSensor->second.saveDataThread->moveToThread(threadTemp);
					threadTemp->start();
					itSaveSensor->second.bInitFileFinish = true;
					//QString strFileName = itSaveSensor->second.strSavePath + QString("/") + itSaveSensor->first + QString("_")
					//	+ QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
					itSaveSensor->second.saveDataThread->initFile(itSaveSensor->second.strSavePath, 1);
				}
				itSaveSensor->second.saveDataThread->insertData(originalGraphicData, iLen * iPixelLen, bSaveFinishSensor);
				if (bSaveFinishSensor)
				{
					delete itSaveSensor->second.saveDataThread;
					itSaveSensor->second.saveDataThread = NULL;
					mapIdToSaveParam->erase(itSaveSensor);
				}
			}
			//按普段解析数据
			for (unordered_map<QString, SpectralParams>::iterator itSpectral = itSensorParam->second.allSpectralParams.begin(); itSpectral != itSensorParam->second.allSpectralParams.end(); itSpectral++)
			{
				int iSpeWidth = itSpectral->second.iSpectral_Width;
				int iSpeHeight = itSpectral->second.iSpectral_Height;
				int iStartByte = itSpectral->second.iSpectral_StartByte;
				iLen = iSpeWidth * iSpeHeight;
				BYTE* oriSpectral = itSpectral->second.originalSpectralData + itSensorParam->second.iAnalysisIndex * iLen * iPixelLen;
				itSpectral->second.CurOriginalSpectralData = oriSpectral;

				for (int iBegin = 0; iBegin < iSpeHeight; iBegin++)
				{
					memcpy(oriSpectral + iBegin * iSpeWidth,
						originalGraphicData + (iBegin * itSensorParam->second.Sensor_Size.width() + iStartByte) * iPixelLen, iSpeWidth * iPixelLen);
				}
				//按谱段存储
				std::map<QString, SaveParams>::iterator itSaveSpectral = mapIdToSaveParam->find(itSpectral->first);
				if (itSaveSpectral != mapIdToSaveParam->end())
				{
					bool bSaveFinishSpectral = false;
					if (itSaveSpectral->second.saveFlag == SAVEBYFRAME)
					{
						bSaveFinishSpectral = true;
					}
					if (itSaveSpectral->second.saveFlag == SAVEBYTIME)
					{
						if (QDateTime::currentMSecsSinceEpoch() - itSaveSpectral->second.iStartTime >= itSaveSpectral->second.iSaveTimeCount * 1000)
						{
							bSaveFinishSpectral = true;
						}
					}
					else if (itSaveSpectral->second.saveFlag == SAVEBYLINES)
					{
						if (itSaveSpectral->second.iStartLine + iSpeHeight >= itSaveSpectral->second.iSaveLineCount)
						{
							iLen = (itSaveSpectral->second.iSaveLineCount - itSaveSpectral->second.iStartLine) * iSpeWidth;
							bSaveFinishSpectral = true;
						}
						else
						{
							itSaveSpectral->second.iStartLine += iSpeHeight;
						}
					}
					if (!itSaveSpectral->second.bInitFileFinish)
					{
						QThread* threadTemp = new QThread;
						vecThread.push_back(threadTemp);
						itSaveSpectral->second.saveDataThread = new SaveData;
						itSaveSpectral->second.saveDataThread->moveToThread(threadTemp);
						threadTemp->start();
						itSaveSpectral->second.bInitFileFinish = true;

						//QString strFileName = itSaveSpectral->second.strSavePath + QString("/") + itSaveSpectral->first + QString("_")
						//	+ QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
						itSaveSpectral->second.saveDataThread->initFile(itSaveSpectral->second.strSavePath, 1);
					}
					itSaveSpectral->second.saveDataThread->insertData(originalGraphicData, iLen * iPixelLen, bSaveFinishSpectral);
					if (bSaveFinishSpectral)
					{
						delete itSaveSpectral->second.saveDataThread;
						itSaveSpectral->second.saveDataThread = NULL;
						mapIdToSaveParam->erase(itSaveSpectral);
					}
				}
			}
#ifdef SHOWTEST
			BYTE* analysisGraphicData = itSensorParam->second.analyticalSensorData + itSensorParam->second.iAnalysisIndex * iLen;
			itSensorParam->second.CurAnalyticalSensorData = analysisGraphicData;

			int dataA = 0;
			int dataB = 0;
			for (int iFrame = 0; iFrame < itSensorParam->second.Sensor_Size.height(); iFrame++)
			{
				BYTE* oneOriginalData = originalGraphicData + iFrame * itSensorParam->second.Sensor_Size.width() * iPixelLen;
				BYTE* oneAnalysisData = analysisGraphicData + iFrame * itSensorParam->second.Sensor_Size.width();

				for (int iBegin = 0; iBegin < iFrameLines * itSensorParam->second.Sensor_Size.width(); iBegin++)//图像数据
				{
					dataA = *(oneOriginalData + iBegin * iPixelLen + 1);
					dataB = *(oneOriginalData + iBegin * iPixelLen + 0);
					*(oneAnalysisData + iBegin) = ((dataB << 8) + dataA) >> iStartBit;
				}
			}
			QImage Pimage(analysisGraphicData, itSensorParam->second.Sensor_Size.width(), itSensorParam->second.Sensor_Size.height(), itSensorParam->second.Sensor_Size.width(), QImage::Format_Indexed8);
			itSensorParam->second.pixMap = QPixmap::fromImage(Pimage);
#endif // SHOWTEST			

			if ((itSensorParam->second.iAnalysisIndex + 1) >= BUFFER_COUNT)
			{
				itSensorParam->second.iAnalysisIndex = 0;
			}
			else
			{
				itSensorParam->second.iAnalysisIndex++;
			}
		}
		//fwrite((char*)(originalGraphicData), 1, iFrameCount * iFrameLength * iPixelLen, fileWrite);
		/*
		for (map<int, CurGraphicData>::iterator it = mapGraphicData.begin(); it != mapGraphicData.end(); it++)
		{
			if (!it->second.bHaveNewData)
			{
				continue;
			}
			else
			{
				it->second.bHaveNewData = false;
			}
			int iCurrentIndex = it->second.iCurrentIndex;
			//char index = iCurrentIndex;
			BYTE* originalDataTemp = it->second.originalData + iCurrentIndex * iFrameCount * iFrameLength * iPixelLen;//原数据
			BYTE* originalGraphicData = it->second.originalGriphicData + it->second.iAnalysisIndex * iFrameLength * it->second.iGraphicHeight * iPixelLen;
			BYTE* analysisGraphicData = it->second.analyticalGriphicData + it->second.iAnalysisIndex * iFrameLength * it->second.iGraphicHeight;
			memset(originalGraphicData, 0x00, iFrameLength * it->second.iGraphicHeight * iPixelLen);
			memset(analysisGraphicData, 0x00, iFrameLength * it->second.iGraphicHeight);

			// 		int iDataIndex = 0;
			// 		for (int iFrame = 0; iFrame < iFrameCount; iFrame++)
			// 		{
			// 			memcpy(originalGraphicData + iDataIndex,
			// 				originalDataTemp + iFrame * it->second.iGraphicWidth * iPixelLen,
			// 				it->second.iGraphicWidth * iPixelLen);
			// 
			// 			iDataIndex += iFrameLength * iPixelLen;
			// 		}
					//fwrite((char*)(originalGraphicData), 1, iFrameCount * iFrameLength * iPixelLen, fileWrite);

			int iDataIndex = iFrameLength * it->second.iGraphicHeight * iPixelLen;
			bool bCopyFinish = false;
			for (int iFrame = iFrameCount - 1; iFrame >= 0; iFrame--)
			{
				iDataIndex -= iFrameLength * iPixelLen;
				if (iDataIndex < 0)
				{
					bCopyFinish = true;
					break;
				}
				memcpy(originalGraphicData + iDataIndex,
					originalDataTemp + iFrame * it->second.iGraphicWidth * iPixelLen,
					it->second.iGraphicWidth * iPixelLen);
			}

			if (!bCopyFinish)
			{
				int iIndexLast = iCurrentIndex - 1;
				if (iIndexLast < 0)
				{
					iIndexLast = BUFFER_COUNT - 1;
				}

				originalDataTemp = it->second.originalData + iIndexLast * iFrameCount * iFrameLength * iPixelLen;//上一包数据

				for (int iFrame = iFrameCount - 1; iFrame >= 0; iFrame--)
				{
					iDataIndex -= iFrameLength * iPixelLen;
					if (iDataIndex < 0)
					{
						bCopyFinish = true;
						break;
					}
					memcpy(originalGraphicData + iDataIndex,
						originalDataTemp + iFrame * it->second.iGraphicWidth * iPixelLen,
						it->second.iGraphicWidth * iPixelLen);
				}
			}
			//fwrite((char*)(originalGraphicData), 1, iFrameCount * iFrameLength * iPixelLen, fileWrite);
			if (iFlagType != 2)
			{
				int dataA = 0;
				int dataB = 0;
				for (int iFrame = 0; iFrame < it->second.iGraphicHeight; iFrame++)
				{
					BYTE* oneOriginalData = originalGraphicData + iFrame * iFrameLength * iPixelLen;
					BYTE* oneAnalysisData = analysisGraphicData + iFrame * iFrameLength;

					for (int iFZBegin = 0; iFZBegin < iFZLength; iFZBegin++)//辅助数据
					{
						*(oneAnalysisData + iFZBegin) = *(oneOriginalData + 2 * iFZBegin + HIGHTORLOW);
					}
					for (int iBegin = 0; iBegin < iFrameLines * (it->second.iGraphicWidth - iFZLength); iBegin++)//图像数据
					{
						dataA = *(oneOriginalData + (iFZLength + iBegin) * iPixelLen + 1);
						dataB = *(oneOriginalData + (iFZLength + iBegin) * iPixelLen + 0);
						*(oneAnalysisData + iFZLength + iBegin) = ((dataB << 8) + dataA) >> iStartBit;
					}
				}
			}

			if (iFlagType == 1)
			{
				QImage Pimage(analysisGraphicData, iFrameLength, it->second.iGraphicHeight, iFrameLength, QImage::Format_Indexed8);
				it->second.pixMap = QPixmap::fromImage(Pimage);
				// 			map<int, QPixmap>::iterator itPixelmap = mapPixMap.find(it->first);
				// 			if (itPixelmap == mapPixMap.end())
				// 			{
				// 				mapPixMap.insert(std::pair<int, QPixmap>(it->first, QPixmap::fromImage(Pimage)));
				// 			}
				// 			else
				// 			{
				// 				itPixelmap->second = QPixmap::fromImage(Pimage);
				// 			}
			}
			it->second.iAnalysisIndex++;
			if (it->second.iAnalysisIndex >= BUFFER_COUNT)
			{
				it->second.iAnalysisIndex = 0;
			}
		}
		*/
		bAnalysisFinish = true;
	}
	currentCameraData->BHaveNewData = false;
}

void AnalysisModel::Slot_TimerRecvNewData()
{
	if (QDateTime::currentDateTime().currentMSecsSinceEpoch() - currentCameraData->iCurrentTime > currentCameraData->Camera_RecvNothingTime * 1000)
	{
		int iData = info_recvData.variantValue.toInt();
		info_recvData.variantValue.setValue(iData - 1);
	}
	else
	{
		info_recvData.variantValue.setValue(0);
	}
	emit Signal_SendInfor(info_recvData);
}

void AnalysisModel::showData()
{
	/*
	if (iFlagType == 1)
	{
		for (unordered_map<int, SensorParams>::iterator itSensorParam = currentCameraData->mapSensor_Params.begin(); itSensorParam != currentCameraData->mapSensor_Params.end(); itSensorParam++)
		{
			map<int, QImageView*>::iterator itGraphic = mapNameToGraphic.find(itSensorParam->first);
			if (itGraphic != mapNameToGraphic.end())
			{
				int iIndex = itSensorParam->second.iAnalysisIndex - 1;
				if (iIndex < 0)
				{
					iIndex = BUFFER_COUNT - 1;
				}
				int iCount = iIndex * itSensorParam->second.Sensor_Size.width() * itSensorParam->second.Sensor_Size.height();
				BYTE* dataShow = itSensorParam->second.analyticalSensorData + iCount;
				itGraphic->second->recvData(dataShow, &itSensorParam->second.pixMap);
			}
		}
	}
	else if (iFlagType == 2)
	{
		for (unordered_map<int, SensorParams>::iterator itSensorParam = currentCameraData->mapSensor_Params.begin(); itSensorParam != currentCameraData->mapSensor_Params.end(); itSensorParam++)
		{
			map<int, TableView*>::iterator itTable = mapNameToTable.find(itSensorParam->first);
			if (itTable != mapNameToTable.end())
			{
				int iIndex = itSensorParam->second.iAnalysisIndex - 1;
				if (iIndex < 0)
				{
					iIndex = BUFFER_COUNT - 1;
				}
					
				int iCount = iIndex * itSensorParam->second.Sensor_Size.width() * itSensorParam->second.Sensor_Size.height() * currentCameraData->One_Pixel_Length;
				itTable->second->recvTableData(itSensorParam->second.originalSensorData + iCount);
			}
		}	
	}
	else if(iFlagType == 3)
	{
		for (unordered_map<int, SensorParams>::iterator itSensorParam = currentCameraData->mapSensor_Params.begin(); itSensorParam != currentCameraData->mapSensor_Params.end(); itSensorParam++)
		{
			map<int, CurveWidget*>::iterator itCurve = mapNameToCurve.find(itSensorParam->first);
			if (itCurve != mapNameToCurve.end())
			{
				int iIndex = itSensorParam->second.iAnalysisIndex - 1;
				if (iIndex < 0)
				{
					iIndex = BUFFER_COUNT - 1;
				}
				int iCount = iIndex * itSensorParam->second.Sensor_Size.width() * itSensorParam->second.Sensor_Size.height();
				itCurve->second->recvData(itSensorParam->first, itSensorParam->second.analyticalSensorData + iCount);
			}
		}
	}
	*/
}

void AnalysisModel::setBitStart(int iStartBitTemp)
{
	iStartBit = iStartBitTemp;
}


void AnalysisModel::changeModel(QString strModel)
{
	/*
	map<QString, int>::iterator itModel = currentCameraData->Camera_Model.find(strModel);
	if (itModel != currentCameraData->Camera_Model.end())
	{
		for (unordered_map<QString, SensorParams>::iterator itSensorParam = currentCameraData->mapSensor_Params.begin(); itSensorParam != currentCameraData->mapSensor_Params.end(); itSensorParam++)
		{
			itSensorParam->second.Sensor_Size.setWidth(itModel->second);
		}
	}
	*/
}

bool AnalysisModel::isHaveNewData()
{
	return currentCameraData->BHaveNewData;
}

bool AnalysisModel::isStopThread()
{
	return bThreadStop;
}

void AnalysisModel::createBuffer()
{
	int iFrameCount = currentCameraData->One_Frame_LineCount;
	int iFrameLength = currentCameraData->One_Frame_LineLength;
	int iPixelLen = currentCameraData->One_Pixel_Length;

	BYTE* dataCamera = new BYTE[iFrameCount * iFrameLength * iPixelLen * BUFFER_COUNT];
	memset(dataCamera, 0x00, iFrameCount * iFrameLength * iPixelLen * BUFFER_COUNT);
	
	currentCameraData->BHaveNewData = false;
	currentCameraData->iCurrentTime = 0;
	currentCameraData->iDataIndexOfBuffer = 0;
	currentCameraData->CurCameraData = dataCamera;
	currentCameraData->cameraData = dataCamera;
	currentCameraData->camera_FZ_data = NULL;//辅助数据待加	
	currentCameraData->iDataIndexOfBuffer = 0;

	/*
	CameraData cameraDataTemp;
	cameraDataTemp.strCamera_ID = currentCameraData->Camera_ID;
	cameraDataTemp.iCamera_Analysis_ID = currentCameraData->Camera_Analysis_ID;
	cameraDataTemp.cameraData = dataCamera;
	cameraDataTemp.camera_FZ_data = NULL;//辅助数据待加	
	cameraDataTemp.BHaveNewData = false;	
	cameraDataTemp.iDataIndexOfBuffer = 0;
	
	//mapCameraIDToBuffer.insert(std::pair<int, CameraData>(currentCameraData->Camera_Analysis_ID, cameraDataTemp));//相机数据缓冲区
	*/
	for (unordered_map<QString, SensorParams>::iterator itSensorParam = currentCameraData->mapSensor_Params.begin(); itSensorParam != currentCameraData->mapSensor_Params.end(); itSensorParam++)
	{
		/*
		CurGraphicSensorData oneSensorData;
		oneSensorData.strSensor_ID = currentCameraData->Sensor_NamesAndSizes[i].strSensor_ID;
		oneSensorData.iSensor_Analysis_ID = currentCameraData->Sensor_NamesAndSizes[i].iSensor_Analysis_ID;		
		oneSensorData.strSensor_Name = currentCameraData->Sensor_NamesAndSizes[i].strSensor_Name;
		oneSensorData.iSensor_DataStartByte = currentCameraData->Sensor_NamesAndSizes[i].iSensor_Sensor_StartByte;
		int iId = currentCameraData->Sensor_NamesAndSizes[i].iSensor_Analysis_ID;
		int iGraphicW = currentCameraData->Sensor_NamesAndSizes[i].Sensor_Size.width();
		int iGraphicH = currentCameraData->Sensor_NamesAndSizes[i].Sensor_Size.height();
		oneSensorData.iSensor_Width = iGraphicW;
		oneSensorData.iSensor_Height = iGraphicH;
		oneSensorData.iSensor_FZ_Length = currentCameraData->One_Line_FZ_Length;
		oneSensorData.iSensor_FZ_Lines = currentCameraData->One_FZ_Lines;
		oneSensorData.originalSensorData = tempOriginalData;
		oneSensorData.analyticalSensorData = tempAnalysisData;
		oneSensorData.iAnalysisIndex = 0;
		QImage Pimage(tempAnalysisData, iGraphicW, iGraphicH, iGraphicW, QImage::Format_Indexed8);
		oneSensorData.pixMap = QPixmap::fromImage(Pimage);
		mapGraphicData.insert(std::pair<int, CurGraphicSensorData>(iId, oneSensorData));
		*/
		int iGraphicW = itSensorParam->second.Sensor_Size.width();
		int iGraphicH = itSensorParam->second.Sensor_Size.height();
		BYTE* tempOriginalData = new BYTE[iGraphicW * iGraphicH * iPixelLen * BUFFER_COUNT];
		memset(tempOriginalData, 0x00, iGraphicW * iGraphicH * iPixelLen * BUFFER_COUNT);		
		itSensorParam->second.CurOriginalSensorData = tempOriginalData;
		itSensorParam->second.originalSensorData = tempOriginalData;

#ifdef SHOWTEST
		BYTE* tempAnalysisData = new BYTE[iGraphicW * iGraphicH * BUFFER_COUNT];
		memset(tempAnalysisData, 0x00, iGraphicW * iGraphicH * BUFFER_COUNT);
		itSensorParam->second.CurAnalyticalSensorData = tempAnalysisData;
		itSensorParam->second.analyticalSensorData = tempAnalysisData;
		QImage Pimage(tempAnalysisData, iGraphicW, iGraphicH, iGraphicW, QImage::Format_Indexed8);
		itSensorParam->second.pixMap = QPixmap::fromImage(Pimage);
#endif // SHOWTEST

		itSensorParam->second.iAnalysisIndex = 0;
		for (unordered_map<QString, SpectralParams>::iterator itSpectral = itSensorParam->second.allSpectralParams.begin(); itSpectral != itSensorParam->second.allSpectralParams.end(); itSpectral++)
		{
			int iSpectralW = itSpectral->second.iSpectral_Width;
			int iSpectralH = itSpectral->second.iSpectral_Height;
			BYTE* spectralTemp1 = new BYTE[iSpectralW * iSpectralH * iPixelLen * BUFFER_COUNT];			
			memset(spectralTemp1, 0x00, iSpectralW * iSpectralH * iPixelLen * BUFFER_COUNT);
			itSpectral->second.originalSpectralData = spectralTemp1;			
			itSpectral->second.CurOriginalSpectralData = spectralTemp1;		

#ifdef SHOWTEST
			BYTE* spectralTemp2 = new BYTE[iSpectralW * iSpectralH * BUFFER_COUNT];
			memset(spectralTemp2, 0x00, iSpectralW * iSpectralH * BUFFER_COUNT);
			itSpectral->second.analyticalSpectralData = spectralTemp2;
			itSpectral->second.CurAnalyticalSpectralData = spectralTemp2;
#endif
		}
	}	
}
