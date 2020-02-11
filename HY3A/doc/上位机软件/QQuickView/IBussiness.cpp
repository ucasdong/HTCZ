#include "IBussiness.h"
#include "QBaseDispalyStrategy.h"
#include <QDebug>
#include <QJsonArray>

IBussiness::~IBussiness()
{
	delete m_pDataAnalysisModel;
}

void IBussiness::startCapture()
{
	for each (QBaseDispalyStrategy* pStrategy in m_pStrategyList)
	{
		pStrategy->setModelData(m_pDataAnalysisModel);
		pStrategy->start();
	}

	for each (QThread* pThread in m_pThreadList)
	{
		pThread->start();
	}
	m_pDataAnalysisModel->StartGrabData();
}

void IBussiness::stopCapture()
{
	m_pDataAnalysisModel->StopGrabData();
	unRegistDisplayView();
}

void IBussiness::replayFile(QString fileName)
{
	for each (QBaseDispalyStrategy* pStrategy in m_pStrategyList)
	{
		pStrategy->setModelData(m_pDataAnalysisModel);
		pStrategy->start();
	}

	for each (QThread* pThread in m_pThreadList)
	{
		pThread->start();
	}
	m_pDataAnalysisModel->ReplayDataStart(fileName.toLatin1().data());
}

void IBussiness::stopReplay(int iSaveType)
{
	m_pDataAnalysisModel->StopDriverData((emSaveType)iSaveType);
}

void IBussiness::saveFile(QString fileName)
{
	QDisplayDockWidget * pWidget = (QDisplayDockWidget*)sender();
	QJsonObject jObj = pWidget->getConfigure();
	QString sDataSourceType = jObj["DataSourceType"].toString();
	SaveParams saveParam;
	saveParam.strDataID = jObj["DataSourceID"].toString();
	saveParam.strSavePath = fileName;
	saveParam.saveFlag = SAVECURFRAME;

	if (sDataSourceType == "Camera")
	{
		saveParam.saveType = SAVE_CAMERA_DATA;
	}
	else if (sDataSourceType == "Sensor")
	{
		saveParam.saveType = SAVE_SENSOR_DATA;
	}
	else if (sDataSourceType == "Spectral")
	{
		saveParam.saveType = SAVE_SPECTRAL_DATA;
	}
	m_pDataAnalysisModel->SaveModelData(saveParam);
}

void IBussiness::calMTF()
{
	QDisplayDockWidget * pWidget = (QDisplayDockWidget*)sender();
	QJsonObject jObj = pWidget->getConfigure();
	QString sDataSourceID = jObj["DataSourceID"].toString();
	QString sDataSourceType = jObj["DataSourceType"].toString();

	//如果当前ID对应的MTF对象已经创建过，则直接显示并返回；
	if (ID_MTF.contains(sDataSourceID)) {
		ID_MTF[sDataSourceID]->showNormal();
		return;
	}
	//否则创建新窗口，并进行设置
	MTF* MTF_Dialog = new MTF;
	ID_MTF[sDataSourceID] = MTF_Dialog;

	MTF_Dialog->init_MTF(sDataSourceID, sDataSourceType);
	MTF_Dialog->showNormal();
	
}

void IBussiness::timerUpDateToDiJian()
{
	m_pSocketToDiJian->connectToHost("192.168.1.25", 5000);
	//}
	QStringList sendAData;
	QStringList sendCData;

	QVector<QByteArray> infos = m_pParsers.find("ALLSpectral").value()->parse(QByteArray((char*)m_pDataAnalysisModel->GetDataBySensor_ID("ALLSpectral")->CurOriginalSensorData, 12328 * 32 * 2));
	for (int i = 0; i < infos.size(); i++)
	{
		sendAData.append(QString::number(infos.at(i).toUInt()));
	}

	QVector<QByteArray> infosB = m_pParsers.find("HY3_GWHTTX").value()->parse(QByteArray((char*)m_pDataAnalysisModel->GetDataBySensor_ID("HY3_GWHTTX")->CurOriginalSensorData, 12328 * 32 * 2));
	for (int i = 0; i < infosB.size(); i++)
	{
		sendCData.append(QString::number(infosB.at(i).toUInt()));
	}

	QString sInfos = sendAData.join(",") + sendCData.join(",");

	qDebug() << sInfos;
	m_pSocketToDiJian->write(sInfos.toLocal8Bit().data(), sInfos.length());
}

void IBussiness::readSocket()
{
	QString sData = QString::fromLocal8Bit(m_pSocketToDiJian->readAll());
	SaveParams saveParam;
	saveParam.strDataID = "HY3";
	saveParam.strSavePath = sData.mid(sData.indexOf(":") + 1);
	saveParam.saveFlag = SAVECURFRAME;

	saveParam.saveType = SAVE_CAMERA_DATA;

	m_pDataAnalysisModel->SaveModelData(saveParam);

	//ui->picCollect->savePic(sData.mid(sData.indexOf(":") + 1));
}

void IBussiness::configModelDriver(QJsonArray cameraConfig, std::string strDriverConfig)
{
	if (m_pDataAnalysisModel != nullptr)
	{
		delete m_pDataAnalysisModel;
	}

	m_pDataAnalysisModel = new DataAnalysisModel;

	m_pDataAnalysisModel->SetProjectConfig(cameraConfig, strDriverConfig);
	m_pDataAnalysisModel->InitDriverAndDataAnalysis();
	for each (QJsonValue jValue in cameraConfig.at(0)["Sensor_NamesAndSizes"].toArray())
	{
		QJsonObject obj = jValue.toObject();

		QFile loadFile(QCoreApplication::applicationDirPath() + "\\" + obj["assistDefine"].toString());

		if (!loadFile.open(QIODevice::ReadOnly))
		{
			qDebug() << "could't open projects json";
			return;
		}

		QByteArray allData = loadFile.readAll();
		loadFile.close();
		QString str = QString::fromUtf8(allData.data()).trimmed();
		str = str.replace('\n', "");
		str = str.replace('\t', "");

		QJsonParseError json_error;
		QJsonDocument jsonDoc(QJsonDocument::fromJson(str.toStdString().data(), &json_error));

		if (json_error.error != QJsonParseError::NoError)
		{
			qDebug() << "json error!" << json_error.errorString();
			return;
		}
		QJsonArray defines = jsonDoc.array();

		int nNextStartBit = 0;
		int curStartBit = 0;
		for each (QJsonValue var in defines)
		{
			QJsonObject objDefine = var.toObject();
			qDebug()<<var<< defines.size();
			if (objDefine.contains("start_bit"))
			{
				curStartBit = objDefine["start_bit"].toInt();
				nNextStartBit = curStartBit;
			}
			else
			{
				curStartBit = nNextStartBit;
				objDefine.insert("start_bit", curStartBit);
			}
			nNextStartBit = nNextStartBit + objDefine["start_bit"].toInt();
		}

		PayloadParser *pParser = new PayloadParser;

		pParser->setConfig(defines);
		m_pParsers.insert(obj["Sensor_ID"].toString(), pParser);
	}
	
}

void IBussiness::registDisplayView(QDisplayDockWidget * pWidget, QJsonObject configObj)
{
	pWidget->setConfigure(configObj);

	QBaseDispalyStrategy * pStrategy = new QCommonDispalyStrategy(pWidget);
	pStrategy->setDisplayWidget(pWidget);
	m_pStrategyList.append(pStrategy);
	QThread *pWorkThread = new QThread;
	pStrategy->moveToThread(pWorkThread);
	m_pThreadList.append(pWorkThread);

	connect(pStrategy, SIGNAL(updatePic(QPixmap)), pWidget, SLOT(recvPic(QPixmap)));
	connect(pStrategy, SIGNAL(updateTable(unsigned char*)), pWidget, SLOT(updateTable(unsigned char*)));
	connect(pStrategy, SIGNAL(updateLine(QList<QPointF>)), pWidget, SLOT(updateLine(QList<QPointF>)));
	connect(pWidget, SIGNAL(saveFileTrigger(QString)), this, SLOT(saveFile(QString)));
	connect(pWidget, &QDisplayDockWidget::cal_MTF_Trigger, this, &IBussiness::calMTF);
}

void IBussiness::unRegistDisplayView()
{
	for each (QBaseDispalyStrategy* pStrategy in m_pStrategyList)
	{
		pStrategy->stop();
		pStrategy->deleteLater();
	}
	for each (QThread* pThread in m_pThreadList)
	{
		pThread->quit();
		pThread->deleteLater();
	}
	m_pStrategyList.clear();
	m_pThreadList.clear();
}

void IBussiness::saveDataAfterCmd(QString savePath, QString cmdName, int nLoopIndex)
{
	//未开始采集时，不进行存图
	if (m_pDataAnalysisModel == nullptr) {
		return;
	}

	//for (std::map<QString, SenSorParams*>::iterator itCameraData = m_pDataAnalysisModel->mapCameraParams.begin(); itCameraData != m_pDataAnalysisModel->mapCameraParams.end(); itCameraData++)
	//{
	//	for (unordered_map<QString, CCD_Params>::iterator itCCD = itCameraData->second->mapCCD_Params.begin(); itCCD != itCameraData->second->mapCCD_Params.end(); itCCD++)
	//	{
	//		for (unordered_map<QString, SpectralParams>::iterator itSprectral = itCCD->second.allSpectralParams.begin(); itSprectral != itCCD->second.allSpectralParams.end(); itSprectral++)
	//		{
	//			SaveParams saveParam;
	//			saveParam.strDataID = itSprectral->second.strSpectral_ID;
	//			saveParam.strSavePath = QString("%1\\%2-%3%4.raw").arg(savePath).arg("cmdName").arg(itCCD->second.strCCD_ID + "_" + itSprectral->second.strSpectral_ID).arg(nLoopIndex);
	//			saveParam.saveType = SAVE_SPECTRAL_DATA;
	//			saveParam.saveFlag = SAVEBYFRAME;
	//			m_pDataAnalysisModel->SaveModelData(saveParam);
	//		}
	//	}
	//}

	for (std::map<QString, CameraParams*>::iterator itCameraData = m_pDataAnalysisModel->mapCameraParams.begin(); itCameraData != m_pDataAnalysisModel->mapCameraParams.end(); itCameraData++)
	{
		SaveParams saveParam;
		saveParam.strDataID = itCameraData->second->Camera_ID;
		saveParam.strSavePath = QString("%1/%2-TX-%4.raw").arg(savePath).arg(cmdName).arg(nLoopIndex);
		saveParam.saveType = SAVE_SPECTRAL_DATA;
		//saveParam.saveType = SAVE_CAMERA_DATA;
		//saveParam.saveFlag = SAVEBYFRAME;
		saveParam.saveFlag = SAVECURFRAME;
		m_pDataAnalysisModel->SaveModelData(saveParam);
	}
}

void IBussiness::initConnectionToDijian()
{
	m_pTimerToDiJian = new QTimer(this);
	connect(m_pTimerToDiJian, SIGNAL(timeout()), this, SLOT(timerUpDateToDiJian()));
	m_pTimerToDiJian->start(1000);
	m_pSocketToDiJian = new QTcpSocket;
	connect(m_pSocketToDiJian, SIGNAL(readyRead()), this, SLOT(readSocket()));
}
