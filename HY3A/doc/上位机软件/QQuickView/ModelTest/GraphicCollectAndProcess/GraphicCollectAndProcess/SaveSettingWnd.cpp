#include "SaveSettingWnd.h"
#include <QApplication>
#include <QFile>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>

SaveSettingWnd::SaveSettingWnd()
{
	ui.setupUi(this);

	setWindowFlags(Qt::FramelessWindowHint);//无边框 
	QString strPath = QApplication::applicationDirPath() + "\\style\\styles.qss";
	QFile qss(strPath);
	qss.open(QFile::ReadOnly);
	setStyleSheet(qss.readAll());
	qss.close();
	ui.maxButton->hide();
	ui.groupBox_LongTime->setChecked(Qt::Unchecked);
	ui.groupBox_FixedRow->setChecked(Qt::Unchecked);

	//相机选择
//	vector<oneWndData> oneScreenParams;//一个屏幕内的窗口配置
//	QHBoxLayout* hBox = new QHBoxLayout;
//	for (int i = 0; i < oneScreenTemp.oneScreenParams.size(); i++)
//	{
//		mapDataSaveFlag.insert(pair<QString, bool>(oneScreenTemp.oneScreenParams[i].WINDOW_ID, false));
//		QCheckBox* chBox = new QCheckBox(oneScreenTemp.oneScreenParams[i].WINDOW_Name);
//		mapCameraIdToCheckBox.insert(pair<QString, QCheckBox*>(oneScreenTemp.oneScreenParams[i].WINDOW_ID, chBox));
//		hBox->addWidget(chBox);
//	}
//	ui.groupBox_ChooseCamera->setLayout(hBox);
/*
	//谱段选择	
	QStringList strPDlist;
	QHBoxLayout* hBoxPD = new QHBoxLayout;
	hBoxPD->setContentsMargins(6, 24, 6, 6);
	strPDlist << QStringLiteral("原始数据存储") << QStringLiteral("分谱段存储");
	for (int i = 0; i < strPDlist.size(); i++)
	{
		QRadioButton* radioBtn = new QRadioButton(strPDlist[i]);
		mapPDIdToRadioBtn.insert(pair<QString, QRadioButton*>(strPDlist[i], radioBtn));
		mapPDFlag.insert(pair<QString, bool>(strPDlist[i], false));
		hBoxPD->addWidget(radioBtn);
		connect(radioBtn, SIGNAL(clicked()), this, SLOT(on_PDRadioButton_clicked()));
	}
	ui.groupBox_ChoosePD->setLayout(hBoxPD);
*/
	bSaveType = false;                  //存储类型：长时间存储/约定行数存储
	bStartOrStop = false;               //开始或者停止存储
	bPauseOrContinue = false;           //暂停或者继续
	//界面参数-谱段选择
	iPDFlag = 0;
	//界面参数-基本设置  
	bSaveSetting = false;               //存储设置是否成功
	strSavePath = "";                   //存储路径
	//iSaveFlag = 0;
	//界面参数-长时间存储设置
	strInformation1 = "";               //自定义说明部分
	dSaveTime = 0;                      //存储时间
	//界面参数-长时间存储设置
	iSaveCount = 0;                     //存储行数
	strInformation2 = "";               //自定义说明部分
	iFileType = 1;                      //文件格式

	//控制存储结束
	qintStartTime = 0;                  //开始存储时间
	iPauseCount = 0;                    //约定行数计数
	iPauseCountFZ = 0;                  //约定行数计数
/*
	//按普段存储
	strNameList << "P" << "B1" << "B2" << "B3" << "B4";
	P_Data = new BYTE[GFKJ_P_PERLINECOUNT * GFKJ_ARCVDATA_COUNT * GFKJ_PIXLENNTH];                                //p普段数据
	B1_Data = new BYTE[GFKJ_B_PERLINECOUNT * GFKJ_ARCVDATA_COUNT / 2 * GFKJ_PIXLENNTH];                               //b1普段数据
	B2_Data = new BYTE[GFKJ_B_PERLINECOUNT * GFKJ_ARCVDATA_COUNT / 2 * GFKJ_PIXLENNTH];                               //b2普段数据
	B3_Data = new BYTE[GFKJ_B_PERLINECOUNT * GFKJ_ARCVDATA_COUNT / 2 * GFKJ_PIXLENNTH];                               //b3普段数据
	B4_Data = new BYTE[GFKJ_B_PERLINECOUNT * GFKJ_ARCVDATA_COUNT / 2 * GFKJ_PIXLENNTH];                               //b4普段数据

	memset(P_Data, 0x00, GFKJ_P_PERLINECOUNT * GFKJ_ARCVDATA_COUNT * GFKJ_PIXLENNTH);
	memset(B1_Data, 0x00, GFKJ_B_PERLINECOUNT * GFKJ_ARCVDATA_COUNT / 2 * GFKJ_PIXLENNTH);
	memset(B2_Data, 0x00, GFKJ_B_PERLINECOUNT * GFKJ_ARCVDATA_COUNT / 2 * GFKJ_PIXLENNTH);
	memset(B3_Data, 0x00, GFKJ_B_PERLINECOUNT * GFKJ_ARCVDATA_COUNT / 2 * GFKJ_PIXLENNTH);
	memset(B4_Data, 0x00, GFKJ_B_PERLINECOUNT * GFKJ_ARCVDATA_COUNT / 2 * GFKJ_PIXLENNTH);
*/
	qintStartPauseTime = "";             //开始存储约定行时间
	//设置默认存储路径
	ui.lineEdit_savePath->setText("D:/data");
//	ui.lineEdit_FzPath->setText("D:/data/FzData");

	//辅助数据解析文件
	QString strFzIniPath = QApplication::applicationDirPath() + "\\dataFile\\dataFZ_D.json";
	codeAnalysis.readJsonDataFile(strFzIniPath);
}

SaveSettingWnd::~SaveSettingWnd()
{
}

//插入要存储的数据
void SaveSettingWnd::insertSaveData(QString strName, BYTE* saveDataTemp, int iLen)
{
	/*
	if (iSaveFlag == 2)//save ccsds
	{
		//存储ccsds数据
		std::map<QString, SaveData*>::iterator itAllData = mapTypeToAllData.find(strName);
		if (itAllData == mapTypeToAllData.end())
		{
			SaveData* saveDataClass = new SaveData;
			QString strFileName = QDateTime::currentDateTime().toString("yyyyMMddhhmmss") +
				QString("-") + strName + strFileFormat;

			saveDataClass->initFile(strSavePath + QString("/") + strFileName, 1);

			mapTypeToAllData.insert(std::pair<QString, SaveData*>(strName, saveDataClass));
			saveDataClass->insertData(saveDataTemp, CCSDS_PKGSIZE * iLen * GFCB_PIXLENNTH);
		}
		else
		{
			itAllData->second->insertData(saveDataTemp, CCSDS_PKGSIZE * iLen * GFCB_PIXLENNTH);
		}
		return;
	}
	*/
	if (!bSaveSetting || !bStartOrStop || bPauseOrContinue/* || iSaveFlag == 0*/)//存储复选框、开始停止存储、暂停继续存储
	{
		return;
	}

	if (bSaveType)//长时间存储
	{
		if ((iPDFlag == 0 && mapTypeToAllData.size() == 0) || (iPDFlag == 1 && mapTypeToPuDuan.size() == 0))//第一次存储数据
		{
			qintStartTime = QDateTime::currentSecsSinceEpoch();//开始存储时间
		}
		else
		{
			if ((QDateTime::currentSecsSinceEpoch() - qintStartTime) > dSaveTime)//完成存储时间
			{
				on_pushButton_StartOrStop_clicked();//停止存储
				return;
			}
		}
		if (iPDFlag == 0)//原始数据存储
		{
			//存储所有数据
			std::map<QString, SaveData*>::iterator itAllData = mapTypeToAllData.find(strName);
			if (itAllData == mapTypeToAllData.end())
			{
				SaveData* saveDataClass = new SaveData;
				QString strFileName = QDateTime::currentDateTime().toString("yyyyMMddhhmmss") +
					QString("-") + strName + QString("-") + strInformation1 + strFileFormat;

				saveDataClass->initFile(strSavePath + QString("/") + strFileName, 1);

				mapTypeToAllData.insert(std::pair<QString, SaveData*>(strName, saveDataClass));
				saveDataClass->insertData(saveDataTemp, CCSDS_PKGSIZE * iLen * PIXLENGTH);
			}
			else
			{
				itAllData->second->insertData(saveDataTemp, CCSDS_PKGSIZE * iLen * PIXLENGTH);
			}
		}
	}
	else//约定行存储模式
	{
		if (iPauseCount >= iSaveCount)
		{
			on_pushButton_StartOrStop_clicked();//停止存储
			iPauseCount = 0;
			return;
		}
		int ISaveSize = iLen;
		if ((iSaveCount - iPauseCount) < iLen)
		{
			ISaveSize = iSaveCount - iPauseCount;
		}

		if (iPDFlag == 0)//存储原数据
		{
			//存储原数据
			std::map<QString, SaveData*>::iterator PauseData = mapTypeToPauseData.find(strName);
			if (PauseData == mapTypeToPauseData.end())
			{
				SaveData* saveDataClass = new SaveData;
				qintStartPauseTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
				QString strFileName = qintStartPauseTime + QString("-") + strName + QString("-") + strInformation2;

				QString strPauseCountFile = strSavePath + QString("/") + strFileName + strFileFormat;

				saveDataClass->initFile(strPauseCountFile, 1);

				mapTypeToPauseData.insert(std::pair<QString, SaveData*>(strName, saveDataClass));

				saveDataClass->insertData(saveDataTemp, ISaveSize * CCSDS_PKGSIZE * PIXLENGTH);
			}
			else
			{
				PauseData->second->insertData(saveDataTemp, ISaveSize * CCSDS_PKGSIZE * PIXLENGTH);
			}
		}
		iPauseCount += ISaveSize;
	}
}

//int SaveSettingWnd::getSaveFlag()
//{
//	return iSaveFlag;
//}
/*
//约定文件拆分
void SaveSettingWnd::PausefilesSplit()
{
	QString strFileEnd = "";
	strFileEnd = strInformation2 + strFileFormat;
	for (std::map<QString, QString>::iterator itFile = mapPauseCountFile.begin(); itFile != mapPauseCountFile.end(); itFile++)
	{
		QString strPath = strSavePath + QString("/") + qintStartPauseTime + QString("-") + itFile->first;//路径+时间+相机标识
		//读数据
		SaveData* dataReadClass = new SaveData;
		dataReadClass->initFile(itFile->second, 2);

		BYTE* fzData = new BYTE[48 * 7];//一条辅助数据
		memset(fzData, 0x00, 48 * 7);
		BYTE* OneFullData = new BYTE[48 * GFKJ_ALLDATA_COUNT * GFKJ_PIXLENNTH];
		memset(OneFullData, 0x00, 48 * GFKJ_ALLDATA_COUNT * GFKJ_PIXLENNTH);
		bool bRead = dataReadClass->readData(OneFullData, 48 * GFKJ_ALLDATA_COUNT * GFKJ_PIXLENNTH);
		if (!bRead)//内容没有一个数据的长度结束
		{
			delete dataReadClass;
			dataReadClass = NULL;
			continue;
		}
		//读取一个完整的辅助数据（48次）
		for (int i = 0; i < 48; i++)
		{
			BYTE* bb = OneFullData + i * GFKJ_ALLDATA_COUNT * GFKJ_PIXLENNTH + 9;
			memcpy(fzData + i * 7, OneFullData + i * GFKJ_ALLDATA_COUNT * GFKJ_PIXLENNTH + 9, 7);
		}
		delete[] OneFullData;
		QString strJS = "";    //级数
		QString strZY = "";    //增益
		QString strJFSJ = "";  //积分时间
		QString strKB = "";    //KB参数
		QString strSFKG = "";  //算法开关

		oneParamData oneParam;
		//级数
		bool bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B1-B2-B3-B4-B5级数W0~W4"), oneParam);//成像级数
		if (bHave)
		{
			for (int i = 0; i < oneParam.iAnalysisBits; i++)
			{
				strJS += QString("%1").arg(fzData[oneParam.iStartBit + i], 2, 16, QLatin1Char('0'));
			}
		}
		//增益
		bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B1-B2-B3增益W0W1"), oneParam);//增益B1-B2-B3
		if (bHave)
		{
			for (int i = 0; i < oneParam.iAnalysisBits; i++)
			{
				strZY += QString("%1").arg(fzData[oneParam.iStartBit + i], 2, 16, QLatin1Char('0'));
			}
		}
		bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B4-B5增益W0W1"), oneParam);//增益B4-B5
		if (bHave)
		{
			for (int i = 0; i < oneParam.iAnalysisBits; i++)
			{
				strZY += QString("%1").arg(fzData[oneParam.iStartBit + i], 2, 16, QLatin1Char('0'));
			}
		}
		//积分时间
		bHave = codeAnalysis.findOneParamSetting(QStringLiteral("积分时间代码W0W1"), oneParam);//积分时间
		if (bHave)
		{
			for (int i = 0; i < oneParam.iAnalysisBits; i++)
			{
				strJFSJ += QString("%1").arg(fzData[oneParam.iStartBit + i], 2, 16, QLatin1Char('0'));
			}
		}
		//KB参数
		bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B1-B2-B3KB参数W0W1"), oneParam);//KB参数B1-B2-B3
		if (bHave)
		{
			for (int i = 0; i < oneParam.iAnalysisBits; i++)
			{
				strKB += QString("%1").arg(fzData[oneParam.iStartBit + i], 2, 16, QLatin1Char('0'));
			}
		}
		bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B4-B5KB参数W0W1"), oneParam);//KB参数B4-B5
		if (bHave)
		{
			for (int i = 0; i < oneParam.iAnalysisBits; i++)
			{
				strKB += QString("%1").arg(fzData[oneParam.iStartBit + i], 2, 16, QLatin1Char('0'));
			}
		}
		//算法开关
		bHave = codeAnalysis.findOneParamSetting(QStringLiteral("算法开关状态"), oneParam);//算法开关
		if (bHave)
		{
			for (int i = 0; i < oneParam.iAnalysisBits; i++)
			{
				strSFKG += QString("%1").arg(fzData[oneParam.iStartBit + i], 2, 16, QLatin1Char('0'));
			}
		}
		delete[] fzData;
		//文件名称
		//P普段
		QString strPname = strPath + QStringLiteral("-P谱段-") + strJS + QString("-") + strZY + QString("-") + strJFSJ + QString("-") + strKB + QString("-") + strSFKG + QString("-") + strFileEnd;
		//B1普段
		QString strB1name = strPath + QStringLiteral("-B1谱段-") + strJS + QString("-") + strZY + QString("-") + strJFSJ + QString("-") + strKB + QString("-") + strSFKG + QString("-") + strFileEnd;
		//B2普段
		QString strB2name = strPath + QStringLiteral("-B2谱段-") + strJS + QString("-") + strZY + QString("-") + strJFSJ + QString("-") + strKB + QString("-") + strSFKG + QString("-") + strFileEnd;
		//B3普段
		QString strB3name = strPath + QStringLiteral("-B3谱段-") + strJS + QString("-") + strZY + QString("-") + strJFSJ + QString("-") + strKB + QString("-") + strSFKG + QString("-") + strFileEnd;
		//B4普段
		QString strB4name = strPath + QStringLiteral("-B4谱段-") + strJS + QString("-") + strZY + QString("-") + strJFSJ + QString("-") + strKB + QString("-") + strSFKG + QString("-") + strFileEnd;

		//创建文件
		SaveData* dataWritePClass = new SaveData;
		dataWritePClass->initFile(strPname, 1);
		SaveData* dataWriteB1Class = new SaveData;
		dataWriteB1Class->initFile(strB1name, 1);
		SaveData* dataWriteB2Class = new SaveData;
		dataWriteB2Class->initFile(strB2name, 1);
		SaveData* dataWriteB3Class = new SaveData;
		dataWriteB3Class->initFile(strB3name, 1);
		SaveData* dataWriteB4Class = new SaveData;
		dataWriteB4Class->initFile(strB4name, 1);

		//重置读文件计数（重头开始读）
		dataReadClass->resetReadFileSize(0);
		while (1)
		{
			BYTE OneSaveData[GFKJ_ALLDATA_COUNT * GFKJ_PIXLENNTH];
			memset(OneSaveData, 0x00, GFKJ_ALLDATA_COUNT * GFKJ_PIXLENNTH);
			bool bRead = dataReadClass->readData(OneSaveData, GFKJ_ALLDATA_COUNT * GFKJ_PIXLENNTH);
			if (!bRead)
			{
				break;
			}
			if (OneSaveData[4] % 2 == 0)//偶数行
			{
				dataWritePClass->insertData(&OneSaveData[GFKJ_FZ_COUNT], GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH);
				dataWriteB1Class->insertData(&OneSaveData[GFKJ_FZ_COUNT + GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH], GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH);
				dataWriteB2Class->insertData(&OneSaveData[GFKJ_FZ_COUNT + GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH + GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH], GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH);
			}
			else//偶数行
			{
				dataWritePClass->insertData(&OneSaveData[GFKJ_FZ_COUNT], GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH);
				dataWriteB3Class->insertData(&OneSaveData[GFKJ_FZ_COUNT + GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH], GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH);
				dataWriteB4Class->insertData(&OneSaveData[GFKJ_FZ_COUNT + GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH + GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH], GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH);
			}
		}
		delete dataReadClass;
		delete dataWritePClass;
		delete dataWriteB1Class;
		delete dataWriteB2Class;
		delete dataWriteB3Class;
		delete dataWriteB4Class;

		dataReadClass = NULL;
		dataWritePClass = NULL;
		dataWriteB1Class = NULL;
		dataWriteB2Class = NULL;
		dataWriteB3Class = NULL;
		dataWriteB4Class = NULL;
// 		//删除源文件
// 		QFile file(itFile->second);
// 		file.remove();
	}
}

QString SaveSettingWnd::getFZFileName(BYTE* dataTemp)
{
	QString strJS = "";    //级数
	QString strZY = "";    //增益
	QString strJFSJ = "";  //积分时间
	QString strKB = "";    //KB参数
	QString strSFKG = "";  //算法开关

	oneParamData oneParam;
	//级数
	bool bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B1-B2-B3-B4-B5级数W0~W4"), oneParam);//成像级数
	if (bHave)
	{
		for (int i = 0; i < oneParam.iAnalysisBits / 2; i++)
		{
			strJS += QString("%1").arg(dataTemp[oneParam.iStartBit / 2 + i], 2, 16, QLatin1Char('0'));
		}
	}
	//增益
	bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B1-B2-B3增益W0W1"), oneParam);//增益B1-B2-B3
	if (bHave)
	{
		for (int i = 0; i < oneParam.iAnalysisBits / 2; i++)
		{
			strZY += QString("%1").arg(dataTemp[oneParam.iStartBit / 2 + i], 2, 16, QLatin1Char('0'));
		}
	}
	bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B4-B5增益W0W1"), oneParam);//增益B4-B5
	if (bHave)
	{
		for (int i = 0; i < oneParam.iAnalysisBits / 2; i++)
		{
			strZY += QString("%1").arg(dataTemp[oneParam.iStartBit / 2 + i], 2, 16, QLatin1Char('0'));
		}
	}
	//积分时间
	bHave = codeAnalysis.findOneParamSetting(QStringLiteral("积分时间代码W0W1"), oneParam);//积分时间
	if (bHave)
	{
		for (int i = 0; i < oneParam.iAnalysisBits / 2; i++)
		{
			strJFSJ += QString("%1").arg(dataTemp[oneParam.iStartBit / 2 + i], 2, 16, QLatin1Char('0'));
		}
	}
	//KB参数
	bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B1-B2-B3KB参数W0W1"), oneParam);//KB参数B1-B2-B3
	if (bHave)
	{
		for (int i = 0; i < oneParam.iAnalysisBits / 2; i++)
		{
			strKB += QString("%1").arg(dataTemp[oneParam.iStartBit / 2 + i], 2, 16, QLatin1Char('0'));
		}
	}
	bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B4-B5KB参数W0W1"), oneParam);//KB参数B4-B5
	if (bHave)
	{
		for (int i = 0; i < oneParam.iAnalysisBits / 2; i++)
		{
			strKB += QString("%1").arg(dataTemp[oneParam.iStartBit / 2 + i], 2, 16, QLatin1Char('0'));
		}
	}
	//算法开关
	bHave = codeAnalysis.findOneParamSetting(QStringLiteral("算法开关状态"), oneParam);//算法开关
	if (bHave)
	{
		for (int i = 0; i < oneParam.iAnalysisBits / 2; i++)
		{
			strSFKG += QString("%1").arg(dataTemp[oneParam.iStartBit / 2 + i], 2, 16, QLatin1Char('0'));
		}
	}
	
	QString strName = strJS + QString("-") 
		+ strZY + QString("-")
		+ strJFSJ + QString("-")
		+ strKB + QString("-")
		+ strSFKG;
	return strName;
}
*/
void SaveSettingWnd::on_closeButton_clicked()
{
	this->close();
}

void SaveSettingWnd::on_minButton_clicked()
{
	this->showMinimized();
}

void SaveSettingWnd::on_maxButton_clicked()
{
	if (this->isMaximized())
	{
		this->showNormal();
	}
	else
	{
		this->showMaximized();
	}
}

//选择存储路径
void SaveSettingWnd::on_pushButton_ChoosePath_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QApplication::applicationDirPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir.isEmpty())
	{
		QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("未选择存储目录"), QMessageBox::Ok);
		return;
	}
	ui.lineEdit_savePath->setText(dir);
}
/*
//选择辅助数据存储路径
void SaveSettingWnd::on_pushButton_ChoosePathFz_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QApplication::applicationDirPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir.isEmpty())
	{
		QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("未选择存储目录"), QMessageBox::Ok);
		return;
	}
	ui.lineEdit_FzPath->setText(dir);
}
*/
//点击保存设置
void SaveSettingWnd::on_pushButton_sure_clicked()
{
	bSaveSetting = false;
	//基本设置
	strSavePath = ui.lineEdit_savePath->text();
	if (strSavePath.isEmpty())
	{
		QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("未选择数据存储目录"), QMessageBox::Ok);
		return;
	}
	else
	{
		//当前路径不存在的话创建路径
		QDir dirSavePath(strSavePath);
		if (!dirSavePath.exists())
		{
			bool bOk = dirSavePath.mkpath(strSavePath);
			if (!bOk)
			{
				QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("创建数据存储目录失败"), QMessageBox::Ok);
				return;
			}
		}
	}
	strFileFormat = ".RAW";
	
	//选择相机
	for (map<QString, QCheckBox*>::iterator itCheck = mapCameraIdToCheckBox.begin(); itCheck != mapCameraIdToCheckBox.end(); itCheck++)
	{
		map<QString, bool>::iterator itFlag = mapDataSaveFlag.find(itCheck->first);
		if (itFlag != mapDataSaveFlag.end())
		{
			if (itCheck->second->isChecked())
			{
				itFlag->second = true;
			}
			else
			{
				itFlag->second = false;
			}
		}
	}

	bool blongTimeMode = ui.groupBox_LongTime->isChecked();
	bool bFixedRowMode = ui.groupBox_FixedRow->isChecked();
	if (!blongTimeMode && !bFixedRowMode)
	{
		QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("未选择存储模式"), QMessageBox::Ok);
		return;
	}
	if (ui.groupBox_LongTime->isChecked())//长时间存储模式
	{
		bSaveType = true;
		dSaveTime = ui.lineEdit_saveTime->text().toDouble();//存储时间
		strInformation1 = ui.lineEdit_information1->text(); //文件名自定义说明
	}
	else//约定行存储模式
	{
		bSaveType = false;
		iSaveCount = ui.lineEdit_saveCount->text().toInt();//存储行数
		strInformation2 = ui.lineEdit_information2->text();//文件名自定义说明
	}
	
	QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("设置保存成功"), QMessageBox::Ok);
	bSaveSetting = true;
}

//点击开始存储
void SaveSettingWnd::on_pushButton_StartOrStop_clicked()
{
	if (!bSaveSetting)
	{
		QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("未完成存储设置"), QMessageBox::Ok);
		return;
	}
	QString strText = ui.pushButton_StartOrStop->text().trimmed();
	if (strText == QStringLiteral("开始存储"))
	{
		ui.pushButton_StartOrStop->setText(QStringLiteral("  停止存储  "));
		bStartOrStop = true;
		//iSaveFlag = 1;
	}
	else
	{
		ui.pushButton_StartOrStop->setText(QStringLiteral("  开始存储  "));
		//iSaveFlag = 0;
		bStartOrStop = false;
		for (std::map<QString, SaveData*>::iterator itAllData = mapTypeToAllData.begin(); itAllData != mapTypeToAllData.end(); itAllData++)//存储数据
		{
			delete itAllData->second;
			itAllData->second = NULL;
		}
		mapTypeToAllData.clear();

		for (std::map<QString, std::map<QString, SaveData*>>::iterator itPDData = mapTypeToPuDuan.begin(); itPDData != mapTypeToPuDuan.end(); itPDData++)  //存储辅助数据
		{
			for (std::map<QString, SaveData*>::iterator oneData = itPDData->second.begin(); oneData != itPDData->second.end(); oneData++)
			{
				delete oneData->second;
				oneData->second = NULL;
			}			
		}
		mapTypeToPuDuan.clear();
		
		for (std::map<QString, SaveData*>::iterator itFzData = mapTypeToFZData.begin(); itFzData != mapTypeToFZData.end(); itFzData++)  //存储辅助数据
		{
			delete itFzData->second;
			itFzData->second = NULL;
		}
		mapTypeToFZData.clear();

		for (std::map<QString, SaveData*>::iterator itPause = mapTypeToPauseData.begin(); itPause != mapTypeToPauseData.end(); itPause++) //存储约定行数数据
		{
			delete itPause->second;
			itPause->second = NULL;
		}
		mapTypeToPauseData.clear();
	}
	//控制暂停继续按钮
	ui.pushButton_PauseOrContinue->setText(QStringLiteral("  暂停存储  "));
	bPauseOrContinue = false;
}

//点击暂停存储
void SaveSettingWnd::on_pushButton_PauseOrContinue_clicked()
{
	if (!bStartOrStop)
	{
		return;
	}
	QString strText = ui.pushButton_PauseOrContinue->text().trimmed();
	if (strText == QStringLiteral("暂停存储"))
	{
		ui.pushButton_PauseOrContinue->setText(QStringLiteral("  继续存储  "));
		//ui.pushButton_StartOrStop->setIcon(QIcon(":/GraphicCollectAndProcess/image/startCollect.png"));
		bPauseOrContinue = true;

	}
	else
	{
		ui.pushButton_PauseOrContinue->setText(QStringLiteral("  暂停存储  "));
		bPauseOrContinue = false;
	
	}
}

//点击长时间存储模式
void SaveSettingWnd::on_groupBox_LongTime_clicked(bool bCheck)
{
	if (bCheck)
	{
		ui.groupBox_FixedRow->setChecked(false);
	}
}

//点击约定行数模式
void SaveSettingWnd::on_groupBox_FixedRow_clicked(bool bCheck)
{
	if (bCheck)
	{
		ui.groupBox_LongTime->setChecked(false);
	}
}

void SaveSettingWnd::on_PDRadioButton_clicked()
{
	QRadioButton* btn = qobject_cast<QRadioButton*>(sender());
	if (btn)
	{ 
		if (btn->text().trimmed() == QStringLiteral("原始数据存储"))
		{
			iPDFlag = 0;
		}
		else if (btn->text().trimmed() == QStringLiteral("分谱段存储"))
		{
			iPDFlag = 1;
		}
	}
}
/*
void SaveSettingWnd::on_pushButton_SaveCCSDS_clicked()
{
	if (ui.pushButton_SaveCCSDS->text().contains(QStringLiteral("停止")))
	{
		ui.pushButton_SaveCCSDS->setText(QStringLiteral(" 存储CCSDS "));
		iSaveFlag = 0;
		for (std::map<QString, SaveData*>::iterator itAllData = mapTypeToAllData.begin(); itAllData != mapTypeToAllData.end(); itAllData++)//存储数据
		{
			delete itAllData->second;
			itAllData->second = NULL;
		}
		mapTypeToAllData.clear();
	}
	else
	{
		ui.pushButton_SaveCCSDS->setText(QStringLiteral(" 停止存储  "));
		iSaveFlag = 2;
	}

}
*/
void SaveSettingWnd::mousePressEvent(QMouseEvent *event)
{
	m_pressPos = event->pos();
	bPress = false;
	if ((event->button() == Qt::LeftButton) && m_pressPos.x() < (this->width() - 80) && m_pressPos.y() < (ui.topWidget->height()))
	{
		//鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
		bPress = true;
	}
}

void SaveSettingWnd::mouseMoveEvent(QMouseEvent *event)
{
	if (bPress)
	{
		//鼠标相对于屏幕的位置
		QPoint move_pos = event->globalPos();
		this->move(move_pos - m_pressPos);
	}
}

void SaveSettingWnd::mouseReleaseEvent(QMouseEvent *event)
{
	bPress = false;
}
