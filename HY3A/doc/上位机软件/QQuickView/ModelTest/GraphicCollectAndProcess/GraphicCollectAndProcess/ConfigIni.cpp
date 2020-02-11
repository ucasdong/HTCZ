#include "ConfigIni.h"
#include <QApplication>
#include <string>
#include <fstream>
#include <algorithm>
#include <QFile>
#include <QDateTime>
#include <QJsonDocument>
#include <QDebug>

#include "json.hpp"
using json = nlohmann::json;

ConfigIni * ConfigIni::m_instance = NULL;
QMutex ConfigIni::m_mt;

ofstream outLog;

ConfigIni::ConfigIni()
{
	m_instance = NULL;
	QString strPath = QString("d:/log/") + QString::number(QDateTime::currentSecsSinceEpoch()) + QString(".txt");
	outLog.open(strPath.toLocal8Bit(), ios::app);
	if (!outLog.is_open())
	{
		//return;
	}
// 	readConfigInit();//读取ini配置
// 	readYaoKongInit();//读取ini配置
	readJsonConfigInit();
	readJsonYaoKongInit();
	readYaoCeInit();
}

ConfigIni::~ConfigIni()
{
	outLog.close();
	delInstance();
}

//读取Json配置文件
void ConfigIni::readJsonConfigInit()
{
	mapScreenParams.clear();
	QString strPathTemp = QApplication::applicationDirPath() + QStringLiteral("/config.json");
	QFile file(strPathTemp);
	if (!file.open(QFile::ReadOnly)) 
	{
		qDebug() << "No File";
		return;
	}
	QString json = file.readAll();
	file.close();

	QJsonParseError error;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toUtf8(), &error);
	if (error.error != QJsonParseError::NoError)
	{
		qDebug() << "jsonDocument error";
		return;
	}
	if (!jsonDocument.isObject())
	{
		qDebug() << "jsonDocument is not Object";
		return;
	}

	QVariantMap results = jsonDocument.toVariant().toMap();
	for (auto iter = results.begin(); iter != results.end(); ++iter)
	{
		QVariantMap oneScreenData = iter.value().toMap();
		oneScreenParams oneScreen;

		oneScreen.oneScreenLayout.setWidth(oneScreenData.value("oneScreenLayout").toList()[0].toInt());
		oneScreen.oneScreenLayout.setHeight(oneScreenData.value("oneScreenLayout").toList()[1].toInt());
//		QVariantList cameraModelList = oneScreenData.value("Camera_Model").toList();
//		for (int i = 0; i < cameraModelList.size(); i++)
//		{
//			QVariantList oneModelList = cameraModelList[i].toList();
//			oneScreen.Camera_ALLModel.insert(std::pair<QString, int>(oneModelList[0].toString(), oneModelList[1].toInt()));
//		}

		for (auto onedata : oneScreenData.value("oneScreenParams").toList())
		{
			oneCameraParams* oneCameraData = new oneCameraParams;

			oneCameraData->Camera_ID = onedata.toMap().value("Camera_ID").toString();
			oneCameraData->Camera_Analysis_ID = onedata.toMap().value("Camera_Analysis_ID").toInt();
			oneCameraData->Camera_ID_Index = onedata.toMap().value("Camera_ID_Index").toInt();
			oneCameraData->Camera_ID_IndexLength = onedata.toMap().value("Camera_ID_IndexLength").toInt();
			oneCameraData->Camera_Name = onedata.toMap().value("Camera_Name").toString();
			oneCameraData->Camera_AnalysisType = onedata.toMap().value("Camera_AnalysisType").toInt();

			QVariantList modelList = onedata.toMap().value("Camera_Model").toList();
			for (int i = 0; i < modelList.size(); i++)
			{
				QVariantList modelDataList = modelList[i].toList();
				oneCameraData->Camera_Model.insert(std::pair<QString, int>(modelDataList[0].toString(), modelDataList[1].toInt()));
				oneScreen.Camera_ALLModel.insert(std::pair<QString, int>(modelDataList[0].toString(), modelDataList[1].toInt()));
			}

			QVariantList layList = onedata.toMap().value("CCD_Lay").toList();
			for (int i = 0;i < layList.size(); i++)
			{
				oneCameraData->Graphic_Lay.push_back(layList[i].toInt());
			}
			oneCameraData->CCD_ID_Index = onedata.toMap().value("CCD_ID_Index").toInt();
			oneCameraData->CCD_ID_IndexLength = onedata.toMap().value("CCD_ID_IndexLength").toInt();
			oneCameraData->CCD_Count = onedata.toMap().value("CCD_Count").toInt();
			oneCameraData->CCD_NumDiff = onedata.toMap().value("CCD_NumDiff").toInt();

			for (auto oneCCDdata : onedata.toMap().value("CCD_NamesAndSizes").toList())
			{
				oneCCDNameAndSize oneCCDTemp;
				oneCCDTemp.strCCD_ID = oneCCDdata.toMap().value("CCD_ID").toString();
				oneCCDTemp.iCCD_Analysis_ID = oneCCDdata.toMap().value("CCD_Analysis_ID").toInt();
				oneCCDTemp.iCCD_CCD_StartByte = oneCCDdata.toMap().value("CCD_StartByte").toInt();
				oneCCDTemp.strCCD_Name = oneCCDdata.toMap().value("CCD_Name").toString();
				oneCCDTemp.CCD_Size.setWidth(oneCCDdata.toMap().value("CCD_Width").toInt());
				oneCCDTemp.CCD_Size.setHeight(oneCCDdata.toMap().value("CCD_Height").toInt());
				oneCCDTemp.CCD_FZ_Width = oneCCDdata.toMap().value("CCD_FZ_Width").toInt();
				oneCCDTemp.CCD_FZ_Height = oneCCDdata.toMap().value("CCD_FZ_Height").toInt();
				for (auto oneSpectraldata : oneCCDdata.toMap().value("CCD_SpectralList").toList())
				{
					oneCCD_Spectral oneSpectralTemp;
					oneSpectralTemp.strSpectral_ID = oneSpectraldata.toMap().value("Spectral_ID").toString();
					oneSpectralTemp.strSpectral_Name = oneSpectraldata.toMap().value("Spectral_Name").toString();
					oneSpectralTemp.iSpectral_StartBit = oneSpectraldata.toMap().value("Spectral_StartBit").toInt();
					oneSpectralTemp.iSpectral_Length = oneSpectraldata.toMap().value("Spectral_Length").toInt();
					oneSpectralTemp.iSpectral_Width = oneSpectraldata.toMap().value("Spectral_Width").toInt();
					oneCCDTemp.allSpectralParams.insert(std::pair<QString, oneCCD_Spectral>(oneSpectralTemp.strSpectral_ID, oneSpectralTemp));
				}
				//oneCameraData->CCD_NamesAndSizes.push_back(oneCCDTemp);
				//oneCameraData->mapCCD_NamesAndSizes[oneCCDTemp.iCCD_Analysis_ID] = oneCCDTemp;
				oneCameraData->mapCCD_NamesAndSizes.insert(std::make_pair(oneCCDTemp.strCCD_ID, oneCCDTemp));
			}
			oneCameraData->One_Frame_LineCount = onedata.toMap().value("One_Frame_LineCount").toInt();
			oneCameraData->One_Frame_LineLength = onedata.toMap().value("One_Frame_LineLength").toInt();
			oneCameraData->One_Frame_IncludeGraphicLines = onedata.toMap().value("One_Frame_IncludeGraphicLines").toInt();
			oneCameraData->One_Line_FZ_Length = onedata.toMap().value("One_Line_FZ_Length").toInt();
			oneCameraData->One_FZ_Lines = onedata.toMap().value("One_FZ_Lines").toInt();
			oneCameraData->One_Line_GraphicLength = onedata.toMap().value("One_Line_GraphicLength").toInt();
			oneCameraData->One_Pixel_Length = onedata.toMap().value("One_Pixel_Length").toInt();
			oneCameraData->Data_EndianFlag = onedata.toMap().value("Data_EndianFlag").toInt();
			oneCameraData->Data_SkipBits = onedata.toMap().value("Data_SkipBits").toInt();

			oneScreen.vecCameraParams.push_back(oneCameraData);
		}

		mapScreenParams.insert(std::pair<int, oneScreenParams>(iter.key().toInt(), oneScreen));
	}
}

//读取遥控界面配置文件
void ConfigIni::readYaoKongInit()
{
	QString strPathTemp = QApplication::applicationDirPath() + QStringLiteral("/dataFile/yaoKongLay.ini");
	QFile file(strPathTemp);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return;
	}

	QString strOneLine = "";
	QStringList strListTemp;
	OneGroup oneGroupTemp;
	while (!file.atEnd()) //第一行是属性列表
	{
		strOneLine = file.readLine().trimmed();
		if (strOneLine.isEmpty() || strOneLine.contains('*'))
		{
			continue;
		}
		else if (strOneLine.contains(QStringLiteral("WINDOW_LAY")))//窗口布局
		{
			strListTemp = strOneLine.split("=");
			if (strListTemp.size() == 2)
			{
				strListTemp = strListTemp[1].split(",");
				if (strListTemp.size() == 2)
				{
					oneYaoKongLay.layoutYK.setWidth(strListTemp[0].toInt());
					oneYaoKongLay.layoutYK.setHeight(strListTemp[1].toInt());
				}
			}
		}
		else if (strOneLine.contains("WINDOW_CAMERATYPE"))//相机类型对应的指令数值
		{
			strListTemp = strOneLine.split("=");
			strListTemp = strListTemp[1].split(";");
			for (int i = 0; i < strListTemp.size(); i++)
			{
				QStringList strVal = strListTemp[i].split(":");
				oneYaoKongLay.mapComboxParams.insert(std::pair<QString, int>(strVal[0], strVal[1].toInt()));
			}
		}
		else if (strOneLine.contains("WINDOW_PERIODS"))//相机普段对应的指令数值
		{
			strListTemp = strOneLine.split("=");
			strListTemp = strListTemp[1].split(";");
			for (int i = 0; i < strListTemp.size(); i++)
			{
				QStringList strVal = strListTemp[i].split(":");
				oneYaoKongLay.mapComboxParams.insert(std::pair<QString, int>(strVal[0], strVal[1].toInt()));
			}
		}
		else if (strOneLine == QStringLiteral("Param_Start"))//一个group开始
		{
			oneGroupTemp.Param_GroupName = "";
			oneGroupTemp.vecTypeToName.clear();
		}
		else if (strOneLine == QStringLiteral("Param_End"))//一个group结束
		{
			oneYaoKongLay.groups.push_back(oneGroupTemp);
		}
		else
		{
			strListTemp = strOneLine.split("=");
			if (strListTemp.size() != 2)
			{
				continue;
			}
			else if (strListTemp[0].trimmed() == QStringLiteral("Param_GroupName"))
			{
				oneGroupTemp.Param_GroupName = strListTemp[1];
			}
			else
			{
				oneGroupTemp.vecTypeToName.push_back(std::pair<QString, QString>(strListTemp[0].trimmed(), strListTemp[1].trimmed()));
			}
		}
	}
	file.close();             //关闭文件输入流 
}

//读取Json遥控界面配置文件
void ConfigIni::readJsonYaoKongInit()
{
	QString strPathTemp = QApplication::applicationDirPath() + QStringLiteral("/dataFile/yaoKongLay.json");
	QFile file(strPathTemp);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return;
	}
	QString json = file.readAll();
	file.close();

	QJsonParseError error;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toUtf8(), &error);
	if (error.error == QJsonParseError::NoError) {
		if (jsonDocument.isObject()) {
			QVariantMap results = jsonDocument.toVariant().toMap();

			QSize layoutYK;
			layoutYK.setWidth(results.value("WINDOW_LAY").toList()[0].toInt());
			layoutYK.setHeight(results.value("WINDOW_LAY").toList()[1].toInt());
			oneYaoKongLay.layoutYK = layoutYK;

			map<QString, int> mapComboxParams;//combox对应的map
			auto VmapComboxParams = results.value("WINDOW_COMBOXPARAMS").toMap();
			for (auto iter = VmapComboxParams.begin(); iter != VmapComboxParams.end(); ++iter) {
				mapComboxParams[iter.key()] = iter.value().toInt();
			}
			oneYaoKongLay.mapComboxParams = mapComboxParams;

			vector<OneGroup> groups;
			for (auto group : results.value("groups").toList()) {
				OneGroup oneGroup;

				oneGroup.Param_GroupName = group.toMap().value("Param_GroupName").toString();
				vector<std::pair<QString, QString>> vecTypeToName;
				auto VvecTypeToName = group.toMap().value("vecTypeToName").toList();
				for (auto iter : VvecTypeToName) {
					std::pair<QString, QString> pair;
					pair.first = iter.toList()[0].toString();
					pair.second = iter.toList()[1].toString();
					vecTypeToName.push_back(pair);
				}
				oneGroup.vecTypeToName = vecTypeToName;

				groups.push_back(oneGroup);
			}
			oneYaoKongLay.groups = groups;
		}
	}
}

void  ConfigIni::readYaoCeInit()                 //读取遥控界面配置文件
{
	json yaoce_j;
	QString strPathTemp = QApplication::applicationDirPath() + QStringLiteral("/dataFile/yaoce.json");
	ifstream istrm(strPathTemp.toLocal8Bit());
	if (!istrm.is_open()) {
		std::cout << "failed to open " << "yaoce.json" << '\n';
		return;
	}
	else {
		istrm >> yaoce_j;
		istrm.close();
	}

	for (auto group : yaoce_j) {
		YaoCeData yaoCeData;
		yaoCeData.iDataLength = group["group_size"].get<int>();
		vector<YaoCe> temp_v;
		for (auto member : group["group_member"]) {
			YaoCe temp;
			temp.param_Name = QString::fromStdString(member["param_Name"].get<string>().c_str());
			temp.param_Type = member["param_Type"].get<int>();
			temp.param_StartBit = member["param_StartBit"].get<int>();
			temp.param_AnalysisBits = member["param_AnalysisBits"].get<int>();
			temp_v.push_back(temp);
		}
		yaoCeData.vecYaoCeParams = temp_v;
		mapGroupToYaoceData[QString::fromStdString(group["group_name"].get<string>().c_str())] = yaoCeData;
	}
}

ConfigIni* ConfigIni::getInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new ConfigIni;
	}
	return m_instance;
}

QMutex* ConfigIni::getMutex()
{
	return &m_mt;
}

void ConfigIni::delInstance()
{
	if (m_instance != NULL)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

map<int, oneScreenParams>* ConfigIni::getAllWndParams()
{
	return &mapScreenParams;
}

//获取遥测界面布局
YaoKongLay* ConfigIni::getYaoKongWndLay()
{
	return &oneYaoKongLay;
}
map<QString, YaoCeData>* ConfigIni::getYaoceData()                   //获取遥测信息
{
	return &mapGroupToYaoceData;
}

vector<string> ConfigIni::split(string str, string pattern)
{
	string::size_type pos;
	vector<string> result;

	str += pattern;

	int size = str.size();
	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}

	return result;
}

void ConfigIni::WriteLogFile(QString strLog, bool bWrite)
{
	if (bWrite)
	{
		outLog.write(strLog.toLocal8Bit(), strLog.size());
	}
}
