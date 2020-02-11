#include "DataTable.h"
#include <QFile>
#include <QMouseEvent>
#include <QHBoxLayout>

DataTable::DataTable(std::map<QString, QSize>* cameraToFzSize)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);//�ޱ߿� 
	QString strPath = QApplication::applicationDirPath() + "\\style\\styles.qss";
	QFile qss(strPath);
	qss.open(QFile::ReadOnly);
	setStyleSheet(qss.readAll());
	qss.close();
	ui.ToolWidget->hide();
	cameraTypeToFZsize = cameraToFzSize;

	//readFZAnalysisFile();
	initWindows(cameraTypeToFZsize);
	bPress = false;
	iFzSataShowTime = FZDATA_SHOWTIME;
}

DataTable::~DataTable()
{
	for (std::map<QString, BYTE*>::iterator it = mapCameraToFZdata.begin(); it!=mapCameraToFZdata.end(); it++)
	{
		delete[] it->second;
		it->second = NULL;
	}
	mapCameraToFZdata.clear();
}

//��ʼ������
void DataTable::initWindows(std::map<QString, QSize>* cameraToFzSize)
{
	ui.widget->setContentsMargins(0, 0, 0, 0);

	QHBoxLayout* hBox = new QHBoxLayout;
	hBox->setContentsMargins(0, 0, 0, 0);

	for (std::map<QString, QSize>::iterator it = cameraToFzSize->begin(); it != cameraToFzSize->end();it++)
	{
		//�����ʾ�����ݳ�ʼ��
		QStringList strHHeader,strVHeader;
		std::vector<std::pair<QString, QString>> vecTemp;

		//ÿ�������������һ����������
		QString strPath = QApplication::applicationDirPath() + "\\dataFile\\dataFZ_" + it->first + ".json";
		CodeDataAnalysis* codeAnalysis = new CodeDataAnalysis;
		codeAnalysis->readJsonDataFile(strPath);
		mapCametraToAnaly.insert(std::pair<QString, CodeDataAnalysis*>(it->first, codeAnalysis));

		std::vector<oneParamData>* vecAllParam = codeAnalysis->getAllParamsData();
		for (std::vector<oneParamData>::iterator it = vecAllParam->begin(); it != vecAllParam->end(); it++)
		{
			//strVHeader << QString::fromLocal8Bit(it->strName.c_str());
			vecTemp.push_back(std::pair<QString, QString>(it->strName, QString("")));
		}
		mapShowData.insert(std::pair<QString, std::vector<std::pair<QString, QString>>>(it->first, vecTemp));

		//����ʼ��
		TableStringView* table = new TableStringView(2, vecTemp.size()); //��������ȡ�������ݽ����ļ���
		mapCametraToTable.insert(std::pair<QString, TableStringView*>(it->first, table));
		strHHeader << QStringLiteral("��������") << QStringLiteral("��ֵ");
		table->initHHeader(strHHeader);
		table->initVHeader(strVHeader);

		//ʵʱ�������ݻ�������ʼ��
		//int a = it->second.width() * it->second.height() * PIXLENGTH;
		BYTE* fzData = new BYTE[it->second.width() * it->second.height() * PIXLENGTH];
		memset(fzData, 0x00, it->second.width() * it->second.height() * PIXLENGTH);
		mapCameraToFZdata.insert(std::pair<QString, BYTE*>(it->first, fzData));

		hBox->addWidget(table);
	}
	ui.widget->setLayout(hBox);
}

//��������
void DataTable::updateData(QString strCmeraType, BYTE* dataTemp)
{
	if (bPress)
	{
		return;
	}
	if (iFzSataShowTime <= 0)
	{
		iFzSataShowTime = FZDATA_SHOWTIME;
	}
	else if (iFzSataShowTime < FZDATA_SHOWTIME)
	{
		iFzSataShowTime--;
		return;
	}
	iFzSataShowTime--;

	std::map<QString, BYTE*>::iterator itDataFind = mapCameraToFZdata.find(strCmeraType);            //������Ͷ�Ӧ�ĸ�������
	//�������ݶ��У�����ɼ����߷ֳ������߷��в�������У����
	if (strCmeraType.contains("12"))//����ɼ�
	{
		if (itDataFind != mapCameraToFZdata.end())
		{
			BYTE* dataFZ = itDataFind->second;
			memcpy(dataFZ, dataTemp, 17 * PIXLENGTH);//0-16��
			memcpy(dataFZ + 17 * PIXLENGTH + dataTemp[24] * 31 * PIXLENGTH, &dataTemp[17 * PIXLENGTH], 31 * PIXLENGTH);//17-47��
		}
	}
	else if (strCmeraType.contains("21") || strCmeraType.contains("22"))//�߷ֳ������߷��в�
	{
		if (itDataFind != mapCameraToFZdata.end())
		{
			int iNum = 17 * PIXLENGTH + dataTemp[24] * 155 * PIXLENGTH;
			BYTE* dataFZ = itDataFind->second;
			memcpy(dataFZ, dataTemp, 17 * PIXLENGTH);//0-16��
			memcpy(dataFZ + 17 * PIXLENGTH, dataTemp + 17 * PIXLENGTH, 105 * PIXLENGTH);//17-121��(�������105)
			memcpy(dataFZ + 17 * PIXLENGTH + 105 * PIXLENGTH + dataTemp[24] * 50 * PIXLENGTH, &dataTemp[17 * PIXLENGTH], 50 * PIXLENGTH);//122-417��
		}
	}
	else if (strCmeraType.contains("31"))//����У��
	{
		if (itDataFind != mapCameraToFZdata.end())
		{
			BYTE* dataFZ = itDataFind->second;
			memcpy(dataFZ, dataTemp, 12 * PIXLENGTH);//0-11��
			memcpy(dataFZ + 12 * PIXLENGTH + dataTemp[14] * 29 * PIXLENGTH, &dataTemp[12 * PIXLENGTH], 29 * PIXLENGTH);//12-40��
		}
	}
	else
	{
		//��������ֻ��һ�У��������ָ������������ǰ��
		if (itDataFind != mapCameraToFZdata.end())
		{
			std::map<QString, QSize>::iterator itSize = cameraTypeToFZsize->find(strCmeraType);          //������Ͷ�Ӧ�������ݳ���
			if (itSize != cameraTypeToFZsize->end())
			{
				BYTE* dataFZ = itDataFind->second;
				memcpy(dataFZ, dataTemp, itSize->second.width() * itSize->second.height() * PIXLENGTH);
			}
		}
	}
	
// 	else
// 	{
// 		return;
// 	}
	//BYTE* dataFZq = itDataFind->second;
	std::map<QString, std::vector<std::pair<QString, QString>>>::iterator itShowData = mapShowData.find(strCmeraType);  //���Ҫ��ʾ������
	if (itShowData != mapShowData.end())
	{
		updateShowVecData(strCmeraType, itShowData->second, itDataFind->second);
	}

	BYTE* dataT = itDataFind->second;
	std::map<QString, TableStringView*>::iterator itTableFind = mapCametraToTable.find(strCmeraType);
	if (itTableFind != mapCametraToTable.end())
	{
		itTableFind->second->changeValue(&itShowData->second);
	}
}

//�������������ļ�
// void DataTable::readFZAnalysisFile()
// {
// // 	QString strPath = QApplication::applicationDirPath() + "\\dataFile\\dataFZ_ZXGL.ini";//ini��ʽ����
// // 	codeAnalysis.readDataFile(strPath);//ini��ʽ����
// 	QString strPath = QApplication::applicationDirPath() + "\\dataFile\\dataFZ_ZXGL.json";
// 	codeAnalysis.readJsonDataFile(strPath);
// }

//���½�����е�����
void DataTable::updateShowVecData(QString strName, std::vector<std::pair<QString, QString>>& vecData, BYTE* dataTemp)
{
	std::map<QString, CodeDataAnalysis*>::iterator itCodeAnalysis = mapCametraToAnaly.find(strName);//������Ͷ�Ӧ�Ľ�������
	if (itCodeAnalysis == mapCametraToAnaly.end())
	{
		return;
	}
	for(std::vector<std::pair<QString, QString>>::iterator it = vecData.begin(); it != vecData.end(); it++)
	{
		oneParamData oneParam;
		bool bHave = itCodeAnalysis->second->findOneParamSetting(it->first, oneParam);
		QString str = "";
		if (bHave)
		{
			for (int i = 0; i < oneParam.iAnalysisBits; i++)
			{
				str += QString("%1").arg(dataTemp[oneParam.iStartBit + i], 2, 16, QLatin1Char('0'));
			}
		}
		it->second = str;
	}
}

void DataTable::on_closeButton_clicked()
{
	this->close();
}

void DataTable::on_minButton_clicked()
{
	this->showMinimized();
}

void DataTable::on_maxButton_clicked()
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

void DataTable::mousePressEvent(QMouseEvent *event)
{
	m_pressPos = event->pos();
	bPress = false;
	if ((event->button() == Qt::LeftButton) && m_pressPos.x() < (this->width() - 80) && m_pressPos.y() < (ui.topWidget->height()))
	{
		//�������ڴ����λ�ã�����ʹ��event->globalPos() - this->pos()��
		bPress = true;
	}
}

void DataTable::mouseMoveEvent(QMouseEvent *event)
{
	if (bPress)
	{
		//����������Ļ��λ��
		QPoint move_pos = event->globalPos();
		this->move(move_pos - m_pressPos);
	}
}

void DataTable::mouseReleaseEvent(QMouseEvent *event)
{
	bPress = false;
}
