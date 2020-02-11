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

	setWindowFlags(Qt::FramelessWindowHint);//�ޱ߿� 
	QString strPath = QApplication::applicationDirPath() + "\\style\\styles.qss";
	QFile qss(strPath);
	qss.open(QFile::ReadOnly);
	setStyleSheet(qss.readAll());
	qss.close();
	ui.maxButton->hide();
	ui.groupBox_LongTime->setChecked(Qt::Unchecked);
	ui.groupBox_FixedRow->setChecked(Qt::Unchecked);

	//���ѡ��
//	vector<oneWndData> oneScreenParams;//һ����Ļ�ڵĴ�������
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
	//�׶�ѡ��	
	QStringList strPDlist;
	QHBoxLayout* hBoxPD = new QHBoxLayout;
	hBoxPD->setContentsMargins(6, 24, 6, 6);
	strPDlist << QStringLiteral("ԭʼ���ݴ洢") << QStringLiteral("���׶δ洢");
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
	bSaveType = false;                  //�洢���ͣ���ʱ��洢/Լ�������洢
	bStartOrStop = false;               //��ʼ����ֹͣ�洢
	bPauseOrContinue = false;           //��ͣ���߼���
	//�������-�׶�ѡ��
	iPDFlag = 0;
	//�������-��������  
	bSaveSetting = false;               //�洢�����Ƿ�ɹ�
	strSavePath = "";                   //�洢·��
	//iSaveFlag = 0;
	//�������-��ʱ��洢����
	strInformation1 = "";               //�Զ���˵������
	dSaveTime = 0;                      //�洢ʱ��
	//�������-��ʱ��洢����
	iSaveCount = 0;                     //�洢����
	strInformation2 = "";               //�Զ���˵������
	iFileType = 1;                      //�ļ���ʽ

	//���ƴ洢����
	qintStartTime = 0;                  //��ʼ�洢ʱ��
	iPauseCount = 0;                    //Լ����������
	iPauseCountFZ = 0;                  //Լ����������
/*
	//���նδ洢
	strNameList << "P" << "B1" << "B2" << "B3" << "B4";
	P_Data = new BYTE[GFKJ_P_PERLINECOUNT * GFKJ_ARCVDATA_COUNT * GFKJ_PIXLENNTH];                                //p�ն�����
	B1_Data = new BYTE[GFKJ_B_PERLINECOUNT * GFKJ_ARCVDATA_COUNT / 2 * GFKJ_PIXLENNTH];                               //b1�ն�����
	B2_Data = new BYTE[GFKJ_B_PERLINECOUNT * GFKJ_ARCVDATA_COUNT / 2 * GFKJ_PIXLENNTH];                               //b2�ն�����
	B3_Data = new BYTE[GFKJ_B_PERLINECOUNT * GFKJ_ARCVDATA_COUNT / 2 * GFKJ_PIXLENNTH];                               //b3�ն�����
	B4_Data = new BYTE[GFKJ_B_PERLINECOUNT * GFKJ_ARCVDATA_COUNT / 2 * GFKJ_PIXLENNTH];                               //b4�ն�����

	memset(P_Data, 0x00, GFKJ_P_PERLINECOUNT * GFKJ_ARCVDATA_COUNT * GFKJ_PIXLENNTH);
	memset(B1_Data, 0x00, GFKJ_B_PERLINECOUNT * GFKJ_ARCVDATA_COUNT / 2 * GFKJ_PIXLENNTH);
	memset(B2_Data, 0x00, GFKJ_B_PERLINECOUNT * GFKJ_ARCVDATA_COUNT / 2 * GFKJ_PIXLENNTH);
	memset(B3_Data, 0x00, GFKJ_B_PERLINECOUNT * GFKJ_ARCVDATA_COUNT / 2 * GFKJ_PIXLENNTH);
	memset(B4_Data, 0x00, GFKJ_B_PERLINECOUNT * GFKJ_ARCVDATA_COUNT / 2 * GFKJ_PIXLENNTH);
*/
	qintStartPauseTime = "";             //��ʼ�洢Լ����ʱ��
	//����Ĭ�ϴ洢·��
	ui.lineEdit_savePath->setText("D:/data");
//	ui.lineEdit_FzPath->setText("D:/data/FzData");

	//�������ݽ����ļ�
	QString strFzIniPath = QApplication::applicationDirPath() + "\\dataFile\\dataFZ_D.json";
	codeAnalysis.readJsonDataFile(strFzIniPath);
}

SaveSettingWnd::~SaveSettingWnd()
{
}

//����Ҫ�洢������
void SaveSettingWnd::insertSaveData(QString strName, BYTE* saveDataTemp, int iLen)
{
	/*
	if (iSaveFlag == 2)//save ccsds
	{
		//�洢ccsds����
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
	if (!bSaveSetting || !bStartOrStop || bPauseOrContinue/* || iSaveFlag == 0*/)//�洢��ѡ�򡢿�ʼֹͣ�洢����ͣ�����洢
	{
		return;
	}

	if (bSaveType)//��ʱ��洢
	{
		if ((iPDFlag == 0 && mapTypeToAllData.size() == 0) || (iPDFlag == 1 && mapTypeToPuDuan.size() == 0))//��һ�δ洢����
		{
			qintStartTime = QDateTime::currentSecsSinceEpoch();//��ʼ�洢ʱ��
		}
		else
		{
			if ((QDateTime::currentSecsSinceEpoch() - qintStartTime) > dSaveTime)//��ɴ洢ʱ��
			{
				on_pushButton_StartOrStop_clicked();//ֹͣ�洢
				return;
			}
		}
		if (iPDFlag == 0)//ԭʼ���ݴ洢
		{
			//�洢��������
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
	else//Լ���д洢ģʽ
	{
		if (iPauseCount >= iSaveCount)
		{
			on_pushButton_StartOrStop_clicked();//ֹͣ�洢
			iPauseCount = 0;
			return;
		}
		int ISaveSize = iLen;
		if ((iSaveCount - iPauseCount) < iLen)
		{
			ISaveSize = iSaveCount - iPauseCount;
		}

		if (iPDFlag == 0)//�洢ԭ����
		{
			//�洢ԭ����
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
//Լ���ļ����
void SaveSettingWnd::PausefilesSplit()
{
	QString strFileEnd = "";
	strFileEnd = strInformation2 + strFileFormat;
	for (std::map<QString, QString>::iterator itFile = mapPauseCountFile.begin(); itFile != mapPauseCountFile.end(); itFile++)
	{
		QString strPath = strSavePath + QString("/") + qintStartPauseTime + QString("-") + itFile->first;//·��+ʱ��+�����ʶ
		//������
		SaveData* dataReadClass = new SaveData;
		dataReadClass->initFile(itFile->second, 2);

		BYTE* fzData = new BYTE[48 * 7];//һ����������
		memset(fzData, 0x00, 48 * 7);
		BYTE* OneFullData = new BYTE[48 * GFKJ_ALLDATA_COUNT * GFKJ_PIXLENNTH];
		memset(OneFullData, 0x00, 48 * GFKJ_ALLDATA_COUNT * GFKJ_PIXLENNTH);
		bool bRead = dataReadClass->readData(OneFullData, 48 * GFKJ_ALLDATA_COUNT * GFKJ_PIXLENNTH);
		if (!bRead)//����û��һ�����ݵĳ��Ƚ���
		{
			delete dataReadClass;
			dataReadClass = NULL;
			continue;
		}
		//��ȡһ�������ĸ������ݣ�48�Σ�
		for (int i = 0; i < 48; i++)
		{
			BYTE* bb = OneFullData + i * GFKJ_ALLDATA_COUNT * GFKJ_PIXLENNTH + 9;
			memcpy(fzData + i * 7, OneFullData + i * GFKJ_ALLDATA_COUNT * GFKJ_PIXLENNTH + 9, 7);
		}
		delete[] OneFullData;
		QString strJS = "";    //����
		QString strZY = "";    //����
		QString strJFSJ = "";  //����ʱ��
		QString strKB = "";    //KB����
		QString strSFKG = "";  //�㷨����

		oneParamData oneParam;
		//����
		bool bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B1-B2-B3-B4-B5����W0~W4"), oneParam);//������
		if (bHave)
		{
			for (int i = 0; i < oneParam.iAnalysisBits; i++)
			{
				strJS += QString("%1").arg(fzData[oneParam.iStartBit + i], 2, 16, QLatin1Char('0'));
			}
		}
		//����
		bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B1-B2-B3����W0W1"), oneParam);//����B1-B2-B3
		if (bHave)
		{
			for (int i = 0; i < oneParam.iAnalysisBits; i++)
			{
				strZY += QString("%1").arg(fzData[oneParam.iStartBit + i], 2, 16, QLatin1Char('0'));
			}
		}
		bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B4-B5����W0W1"), oneParam);//����B4-B5
		if (bHave)
		{
			for (int i = 0; i < oneParam.iAnalysisBits; i++)
			{
				strZY += QString("%1").arg(fzData[oneParam.iStartBit + i], 2, 16, QLatin1Char('0'));
			}
		}
		//����ʱ��
		bHave = codeAnalysis.findOneParamSetting(QStringLiteral("����ʱ�����W0W1"), oneParam);//����ʱ��
		if (bHave)
		{
			for (int i = 0; i < oneParam.iAnalysisBits; i++)
			{
				strJFSJ += QString("%1").arg(fzData[oneParam.iStartBit + i], 2, 16, QLatin1Char('0'));
			}
		}
		//KB����
		bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B1-B2-B3KB����W0W1"), oneParam);//KB����B1-B2-B3
		if (bHave)
		{
			for (int i = 0; i < oneParam.iAnalysisBits; i++)
			{
				strKB += QString("%1").arg(fzData[oneParam.iStartBit + i], 2, 16, QLatin1Char('0'));
			}
		}
		bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B4-B5KB����W0W1"), oneParam);//KB����B4-B5
		if (bHave)
		{
			for (int i = 0; i < oneParam.iAnalysisBits; i++)
			{
				strKB += QString("%1").arg(fzData[oneParam.iStartBit + i], 2, 16, QLatin1Char('0'));
			}
		}
		//�㷨����
		bHave = codeAnalysis.findOneParamSetting(QStringLiteral("�㷨����״̬"), oneParam);//�㷨����
		if (bHave)
		{
			for (int i = 0; i < oneParam.iAnalysisBits; i++)
			{
				strSFKG += QString("%1").arg(fzData[oneParam.iStartBit + i], 2, 16, QLatin1Char('0'));
			}
		}
		delete[] fzData;
		//�ļ�����
		//P�ն�
		QString strPname = strPath + QStringLiteral("-P�׶�-") + strJS + QString("-") + strZY + QString("-") + strJFSJ + QString("-") + strKB + QString("-") + strSFKG + QString("-") + strFileEnd;
		//B1�ն�
		QString strB1name = strPath + QStringLiteral("-B1�׶�-") + strJS + QString("-") + strZY + QString("-") + strJFSJ + QString("-") + strKB + QString("-") + strSFKG + QString("-") + strFileEnd;
		//B2�ն�
		QString strB2name = strPath + QStringLiteral("-B2�׶�-") + strJS + QString("-") + strZY + QString("-") + strJFSJ + QString("-") + strKB + QString("-") + strSFKG + QString("-") + strFileEnd;
		//B3�ն�
		QString strB3name = strPath + QStringLiteral("-B3�׶�-") + strJS + QString("-") + strZY + QString("-") + strJFSJ + QString("-") + strKB + QString("-") + strSFKG + QString("-") + strFileEnd;
		//B4�ն�
		QString strB4name = strPath + QStringLiteral("-B4�׶�-") + strJS + QString("-") + strZY + QString("-") + strJFSJ + QString("-") + strKB + QString("-") + strSFKG + QString("-") + strFileEnd;

		//�����ļ�
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

		//���ö��ļ���������ͷ��ʼ����
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
			if (OneSaveData[4] % 2 == 0)//ż����
			{
				dataWritePClass->insertData(&OneSaveData[GFKJ_FZ_COUNT], GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH);
				dataWriteB1Class->insertData(&OneSaveData[GFKJ_FZ_COUNT + GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH], GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH);
				dataWriteB2Class->insertData(&OneSaveData[GFKJ_FZ_COUNT + GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH + GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH], GFKJ_B_PERLINECOUNT * GFKJ_PIXLENNTH);
			}
			else//ż����
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
// 		//ɾ��Դ�ļ�
// 		QFile file(itFile->second);
// 		file.remove();
	}
}

QString SaveSettingWnd::getFZFileName(BYTE* dataTemp)
{
	QString strJS = "";    //����
	QString strZY = "";    //����
	QString strJFSJ = "";  //����ʱ��
	QString strKB = "";    //KB����
	QString strSFKG = "";  //�㷨����

	oneParamData oneParam;
	//����
	bool bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B1-B2-B3-B4-B5����W0~W4"), oneParam);//������
	if (bHave)
	{
		for (int i = 0; i < oneParam.iAnalysisBits / 2; i++)
		{
			strJS += QString("%1").arg(dataTemp[oneParam.iStartBit / 2 + i], 2, 16, QLatin1Char('0'));
		}
	}
	//����
	bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B1-B2-B3����W0W1"), oneParam);//����B1-B2-B3
	if (bHave)
	{
		for (int i = 0; i < oneParam.iAnalysisBits / 2; i++)
		{
			strZY += QString("%1").arg(dataTemp[oneParam.iStartBit / 2 + i], 2, 16, QLatin1Char('0'));
		}
	}
	bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B4-B5����W0W1"), oneParam);//����B4-B5
	if (bHave)
	{
		for (int i = 0; i < oneParam.iAnalysisBits / 2; i++)
		{
			strZY += QString("%1").arg(dataTemp[oneParam.iStartBit / 2 + i], 2, 16, QLatin1Char('0'));
		}
	}
	//����ʱ��
	bHave = codeAnalysis.findOneParamSetting(QStringLiteral("����ʱ�����W0W1"), oneParam);//����ʱ��
	if (bHave)
	{
		for (int i = 0; i < oneParam.iAnalysisBits / 2; i++)
		{
			strJFSJ += QString("%1").arg(dataTemp[oneParam.iStartBit / 2 + i], 2, 16, QLatin1Char('0'));
		}
	}
	//KB����
	bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B1-B2-B3KB����W0W1"), oneParam);//KB����B1-B2-B3
	if (bHave)
	{
		for (int i = 0; i < oneParam.iAnalysisBits / 2; i++)
		{
			strKB += QString("%1").arg(dataTemp[oneParam.iStartBit / 2 + i], 2, 16, QLatin1Char('0'));
		}
	}
	bHave = codeAnalysis.findOneParamSetting(QStringLiteral("B4-B5KB����W0W1"), oneParam);//KB����B4-B5
	if (bHave)
	{
		for (int i = 0; i < oneParam.iAnalysisBits / 2; i++)
		{
			strKB += QString("%1").arg(dataTemp[oneParam.iStartBit / 2 + i], 2, 16, QLatin1Char('0'));
		}
	}
	//�㷨����
	bHave = codeAnalysis.findOneParamSetting(QStringLiteral("�㷨����״̬"), oneParam);//�㷨����
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

//ѡ��洢·��
void SaveSettingWnd::on_pushButton_ChoosePath_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QApplication::applicationDirPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir.isEmpty())
	{
		QMessageBox::warning(NULL, QStringLiteral("��ʾ"), QStringLiteral("δѡ��洢Ŀ¼"), QMessageBox::Ok);
		return;
	}
	ui.lineEdit_savePath->setText(dir);
}
/*
//ѡ�������ݴ洢·��
void SaveSettingWnd::on_pushButton_ChoosePathFz_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QApplication::applicationDirPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir.isEmpty())
	{
		QMessageBox::warning(NULL, QStringLiteral("��ʾ"), QStringLiteral("δѡ��洢Ŀ¼"), QMessageBox::Ok);
		return;
	}
	ui.lineEdit_FzPath->setText(dir);
}
*/
//�����������
void SaveSettingWnd::on_pushButton_sure_clicked()
{
	bSaveSetting = false;
	//��������
	strSavePath = ui.lineEdit_savePath->text();
	if (strSavePath.isEmpty())
	{
		QMessageBox::warning(NULL, QStringLiteral("��ʾ"), QStringLiteral("δѡ�����ݴ洢Ŀ¼"), QMessageBox::Ok);
		return;
	}
	else
	{
		//��ǰ·�������ڵĻ�����·��
		QDir dirSavePath(strSavePath);
		if (!dirSavePath.exists())
		{
			bool bOk = dirSavePath.mkpath(strSavePath);
			if (!bOk)
			{
				QMessageBox::warning(NULL, QStringLiteral("��ʾ"), QStringLiteral("�������ݴ洢Ŀ¼ʧ��"), QMessageBox::Ok);
				return;
			}
		}
	}
	strFileFormat = ".RAW";
	
	//ѡ�����
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
		QMessageBox::warning(NULL, QStringLiteral("��ʾ"), QStringLiteral("δѡ��洢ģʽ"), QMessageBox::Ok);
		return;
	}
	if (ui.groupBox_LongTime->isChecked())//��ʱ��洢ģʽ
	{
		bSaveType = true;
		dSaveTime = ui.lineEdit_saveTime->text().toDouble();//�洢ʱ��
		strInformation1 = ui.lineEdit_information1->text(); //�ļ����Զ���˵��
	}
	else//Լ���д洢ģʽ
	{
		bSaveType = false;
		iSaveCount = ui.lineEdit_saveCount->text().toInt();//�洢����
		strInformation2 = ui.lineEdit_information2->text();//�ļ����Զ���˵��
	}
	
	QMessageBox::warning(NULL, QStringLiteral("��ʾ"), QStringLiteral("���ñ���ɹ�"), QMessageBox::Ok);
	bSaveSetting = true;
}

//�����ʼ�洢
void SaveSettingWnd::on_pushButton_StartOrStop_clicked()
{
	if (!bSaveSetting)
	{
		QMessageBox::warning(NULL, QStringLiteral("��ʾ"), QStringLiteral("δ��ɴ洢����"), QMessageBox::Ok);
		return;
	}
	QString strText = ui.pushButton_StartOrStop->text().trimmed();
	if (strText == QStringLiteral("��ʼ�洢"))
	{
		ui.pushButton_StartOrStop->setText(QStringLiteral("  ֹͣ�洢  "));
		bStartOrStop = true;
		//iSaveFlag = 1;
	}
	else
	{
		ui.pushButton_StartOrStop->setText(QStringLiteral("  ��ʼ�洢  "));
		//iSaveFlag = 0;
		bStartOrStop = false;
		for (std::map<QString, SaveData*>::iterator itAllData = mapTypeToAllData.begin(); itAllData != mapTypeToAllData.end(); itAllData++)//�洢����
		{
			delete itAllData->second;
			itAllData->second = NULL;
		}
		mapTypeToAllData.clear();

		for (std::map<QString, std::map<QString, SaveData*>>::iterator itPDData = mapTypeToPuDuan.begin(); itPDData != mapTypeToPuDuan.end(); itPDData++)  //�洢��������
		{
			for (std::map<QString, SaveData*>::iterator oneData = itPDData->second.begin(); oneData != itPDData->second.end(); oneData++)
			{
				delete oneData->second;
				oneData->second = NULL;
			}			
		}
		mapTypeToPuDuan.clear();
		
		for (std::map<QString, SaveData*>::iterator itFzData = mapTypeToFZData.begin(); itFzData != mapTypeToFZData.end(); itFzData++)  //�洢��������
		{
			delete itFzData->second;
			itFzData->second = NULL;
		}
		mapTypeToFZData.clear();

		for (std::map<QString, SaveData*>::iterator itPause = mapTypeToPauseData.begin(); itPause != mapTypeToPauseData.end(); itPause++) //�洢Լ����������
		{
			delete itPause->second;
			itPause->second = NULL;
		}
		mapTypeToPauseData.clear();
	}
	//������ͣ������ť
	ui.pushButton_PauseOrContinue->setText(QStringLiteral("  ��ͣ�洢  "));
	bPauseOrContinue = false;
}

//�����ͣ�洢
void SaveSettingWnd::on_pushButton_PauseOrContinue_clicked()
{
	if (!bStartOrStop)
	{
		return;
	}
	QString strText = ui.pushButton_PauseOrContinue->text().trimmed();
	if (strText == QStringLiteral("��ͣ�洢"))
	{
		ui.pushButton_PauseOrContinue->setText(QStringLiteral("  �����洢  "));
		//ui.pushButton_StartOrStop->setIcon(QIcon(":/GraphicCollectAndProcess/image/startCollect.png"));
		bPauseOrContinue = true;

	}
	else
	{
		ui.pushButton_PauseOrContinue->setText(QStringLiteral("  ��ͣ�洢  "));
		bPauseOrContinue = false;
	
	}
}

//�����ʱ��洢ģʽ
void SaveSettingWnd::on_groupBox_LongTime_clicked(bool bCheck)
{
	if (bCheck)
	{
		ui.groupBox_FixedRow->setChecked(false);
	}
}

//���Լ������ģʽ
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
		if (btn->text().trimmed() == QStringLiteral("ԭʼ���ݴ洢"))
		{
			iPDFlag = 0;
		}
		else if (btn->text().trimmed() == QStringLiteral("���׶δ洢"))
		{
			iPDFlag = 1;
		}
	}
}
/*
void SaveSettingWnd::on_pushButton_SaveCCSDS_clicked()
{
	if (ui.pushButton_SaveCCSDS->text().contains(QStringLiteral("ֹͣ")))
	{
		ui.pushButton_SaveCCSDS->setText(QStringLiteral(" �洢CCSDS "));
		iSaveFlag = 0;
		for (std::map<QString, SaveData*>::iterator itAllData = mapTypeToAllData.begin(); itAllData != mapTypeToAllData.end(); itAllData++)//�洢����
		{
			delete itAllData->second;
			itAllData->second = NULL;
		}
		mapTypeToAllData.clear();
	}
	else
	{
		ui.pushButton_SaveCCSDS->setText(QStringLiteral(" ֹͣ�洢  "));
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
		//�������ڴ����λ�ã�����ʹ��event->globalPos() - this->pos()��
		bPress = true;
	}
}

void SaveSettingWnd::mouseMoveEvent(QMouseEvent *event)
{
	if (bPress)
	{
		//����������Ļ��λ��
		QPoint move_pos = event->globalPos();
		this->move(move_pos - m_pressPos);
	}
}

void SaveSettingWnd::mouseReleaseEvent(QMouseEvent *event)
{
	bPress = false;
}
