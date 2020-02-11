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

ConfigIni::ConfigIni(QString ykyc_config_path_)
{
	m_instance = NULL;
	QString strPath = QString("d:/log/") + QString::number(QDateTime::currentSecsSinceEpoch()) + QString(".txt");
	outLog.open(strPath.toLocal8Bit(), ios::app);
	if (!outLog.is_open())
	{
		//return;
	}
	ykyc_config_path = ykyc_config_path_;
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
/*
void ConfigIni::readConfigIni()
{
	mapScreenParams.clear();
	QString strPathTemp = QApplication::applicationDirPath() + "/config.ini";
	std::string strPath = strPathTemp.toLocal8Bit();
	ifstream infile;
	infile.open(strPathTemp.toLocal8Bit(), ios::in);   //将文件流对象与文件连接起来 
	if (!infile.is_open())
	{
		return;
	}
	string strOneLine;                   //读取文件行缓存区
	vector<string> vecDataTemp;          //字符串切割缓存区

	int iScreen = 0;                     //屏幕编号
	oneWndData oneDataTemp;              //一个窗口参数
	vector<oneWndData> allWndParams;     //一个屏幕所有窗口参数
	oneScreen oneScreenTemp;             //一个屏幕参数
	
	while (getline(infile, strOneLine)) //第一行是属性列表
	{
		if (strOneLine == "" || strOneLine.at(0) == '*')
		{
			continue;
		}

		if (strOneLine == "WINDOW_PARAM_START")
		{
			oneDataTemp.WINDOW_SubWndNames.clear();
			oneDataTemp.WINDOW_SubWndSizes.clear();
		}
		else if (strOneLine == "WINDOW_PARAM_END")
		{
			allWndParams.push_back(oneDataTemp);
		}
		else if (strOneLine == "SCREEN_END")
		{
			oneScreenTemp.oneScreenParams = allWndParams;
			mapScreenParams.insert(pair<int, oneScreen>(iScreen, oneScreenTemp));
		}

		vecDataTemp = split(strOneLine, "=");
		if (vecDataTemp.size() != 2)
		{
			continue;
		}

		if (vecDataTemp[0] == "SCREEN_NUM")
		{
			iScreen = atoi(vecDataTemp[1].c_str());
			oneScreenTemp.oneScreenParams.clear();
			allWndParams.clear();
		}
		else if (vecDataTemp[0] == "WINDOW_RowAndColumn")//界面分区
		{
			vector<string> vecSize = split(vecDataTemp[1], ",");
			if (vecSize.size() == 2)
			{
				oneScreenTemp.rowAndColumnSize.setWidth(atoi(vecSize[0].c_str()));
				oneScreenTemp.rowAndColumnSize.setHeight(atoi(vecSize[1].c_str()));
			}
		}
		else if (vecDataTemp[0] == "WINDOW_ID")//数据源头       
		{
			oneDataTemp.WINDOW_ID = vecDataTemp[1];
		}
		else if (vecDataTemp[0] == "WINDOW_Name")//子窗口名称        
		{
			oneDataTemp.WINDOW_Name = vecDataTemp[1];
		}
		else if (vecDataTemp[0] == "WINDOW_Size")//子窗口大小
		{
			vector<string> vecSize = split(vecDataTemp[1], ",");
			if (vecSize.size() == 2)
			{
				oneDataTemp.WINDOW_Size.setWidth(atoi(vecSize[0].c_str()));
				oneDataTemp.WINDOW_Size.setHeight(atoi(vecSize[1].c_str()));
			}
		}
		else if (vecDataTemp[0] == "WINDOW_SubWndCount")//子子窗口个数
		{
			oneDataTemp.WINDOW_SubWndCount = atoi(vecDataTemp[1].c_str());
		}
		else if (vecDataTemp[0] == "WINDOW_FZSize")//辅助数据长度
		{
			vector<string> vecSize = split(vecDataTemp[1], ",");
			if (vecSize.size() == 2)
			{
				oneDataTemp.WINDOW_FZSize.setWidth(atoi(vecSize[0].c_str()));
				oneDataTemp.WINDOW_FZSize.setHeight(atoi(vecSize[1].c_str()));
			}
		}
		else if (vecDataTemp[0] == "WINDOW_SubRowAndColumn")//子子窗口分区
		{
			oneDataTemp.WINDOW_SubRowAndColumn = split(vecDataTemp[1], ",");
		}
		else if (vecDataTemp[0] == "WINDOW_SubWndNames")//子子窗口名称
		{
			vector<string> vecName = split(vecDataTemp[1], ";");
			if (vecName.size() == oneDataTemp.WINDOW_SubWndCount)
			{
				oneDataTemp.WINDOW_SubWndNames = vecName;
			}
		}
		else if (vecDataTemp[0] == "WINDOW_SubWndSize")//子子窗口大小
		{
			vector<string> vecSize1 = split(vecDataTemp[1], ";");
			if (vecSize1.size() == oneDataTemp.WINDOW_SubWndCount)
			{
				QSize sizeTemp;
				for (int i = 0; i < vecSize1.size(); i++)
				{
					vector<string> vecSize2 = split(vecSize1[i], ",");
					if (vecSize2.size() == 2)
					{
						oneDataTemp.WINDOW_SubWndSizes.push_back(QSize(atoi(vecSize2[0].c_str()), atoi(vecSize2[1].c_str())));
					}
				}
			}
		}
	}
	infile.close();             //关闭文件输入流 
}
*/

void ConfigIni::readConfigInit()
{
	mapScreenParams.clear();
	//QString strPathTemp = QApplication::applicationDirPath() + "/config.ini";
	QString strPathTemp = QApplication::applicationDirPath() + QStringLiteral("/config.ini");
	QFile file(strPathTemp);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return;
	}


	QString strOneLine;               //读取文件行缓存区
	QStringList vecDataTemp;          //字符串切割缓存区

	int iScreen = 0;                     //屏幕编号
	oneWndData oneDataTemp;              //一个窗口参数
	vector<oneWndData> allWndParams;     //一个屏幕所有窗口参数
	oneScreen oneScreenTemp;             //一个屏幕参数


	while (!file.atEnd()) //第一行是属性列表
	{
		strOneLine = file.readLine().trimmed();
		if (strOneLine.isEmpty() || strOneLine.contains('*'))
		{
			continue;
		}

		if (strOneLine == "WINDOW_PARAM_START")
		{
			oneDataTemp.WINDOW_SubRowAndColumn.clear();
			oneDataTemp.WINDOW_SubWndNames.clear();
			oneDataTemp.WINDOW_SubWndSizes.clear();
		}
		else if (strOneLine == "WINDOW_PARAM_END")
		{
			allWndParams.push_back(oneDataTemp);
		}
		else if (strOneLine == "SCREEN_END")
		{
			oneScreenTemp.oneScreenParams = allWndParams;
			mapScreenParams.insert(pair<int, oneScreen>(iScreen, oneScreenTemp));
		}

		vecDataTemp = strOneLine.split("=");
		if (vecDataTemp.size() != 2)
		{
			continue;
		}

		if (vecDataTemp[0] == "SCREEN_NUM")
		{
			iScreen = vecDataTemp[1].toInt();
			oneScreenTemp.oneScreenParams.clear();
			allWndParams.clear();
		}
		else if (vecDataTemp[0] == "WINDOW_RowAndColumn")//界面分区
		{
			QStringList vecSize = vecDataTemp[1].split(",");
			if (vecSize.size() == 2)
			{
				oneScreenTemp.rowAndColumnSize.setWidth(vecSize[0].toInt());
				oneScreenTemp.rowAndColumnSize.setHeight(vecSize[1].toInt());
			}
		}
		else if (vecDataTemp[0] == "WINDOW_ID")//数据源头       
		{
			oneDataTemp.WINDOW_ID = vecDataTemp[1];
		}
		else if (vecDataTemp[0] == "WINDOW_Name")//子窗口名称        
		{
			oneDataTemp.WINDOW_Name = vecDataTemp[1];
		}
		else if (vecDataTemp[0] == "WINDOW_Size")//子窗口大小
		{
			QStringList vecSize = vecDataTemp[1].split(",");
			if (vecSize.size() == 2)
			{
				oneDataTemp.WINDOW_Size.setWidth(vecSize[0].toInt());
				oneDataTemp.WINDOW_Size.setHeight(vecSize[1].toInt());
			}
		}
		else if (vecDataTemp[0] == "WINDOW_SubWndCount")//子子窗口个数
		{
			oneDataTemp.WINDOW_SubWndCount = vecDataTemp[1].toInt();
		}
		else if (vecDataTemp[0] == "WINDOW_FZSize")//辅助数据长度
		{
			QStringList vecSize = vecDataTemp[1].split(",");
			if (vecSize.size() == 2)
			{
				oneDataTemp.WINDOW_FZSize.setWidth(vecSize[0].toInt());
				oneDataTemp.WINDOW_FZSize.setHeight(vecSize[1].toInt());
			}
		}
		else if (vecDataTemp[0] == "WINDOW_SubRowAndColumn")//子子窗口分区
		{
			QStringList vecCount = vecDataTemp[1].split(",");
			for (int i = 0; i < vecCount.size(); i++)
			{
				oneDataTemp.WINDOW_SubRowAndColumn.push_back(vecCount[i]);
			}
		}
		else if (vecDataTemp[0] == "WINDOW_SubWndNames")//子子窗口名称
		{
			QStringList vecName = vecDataTemp[1].split(";");
			if (vecName.size() == oneDataTemp.WINDOW_SubWndCount)
			{
				for (int i = 0; i < vecName.size(); i++)
				{
					oneDataTemp.WINDOW_SubWndNames.push_back(vecName[i]);
				}
			}
		}
		else if (vecDataTemp[0] == "WINDOW_SubWndSize")//子子窗口大小
		{
			QStringList vecSize1 = vecDataTemp[1].split(";");
			if (vecSize1.size() == oneDataTemp.WINDOW_SubWndCount)
			{
				QSize sizeTemp;
				for (int i = 0; i < vecSize1.size(); i++)
				{
					QStringList vecSize2 = vecSize1[i].split(",");
					if (vecSize2.size() == 2)
					{
						oneDataTemp.WINDOW_SubWndSizes.push_back(QSize(vecSize2[0].toInt(), vecSize2[1].toInt()));
					}
				}
			}
		}
	}
	file.close();             //关闭文件输入流 
}

//读取Json配置文件
void ConfigIni::readJsonConfigInit()
{
	mapScreenParams.clear();
	allWndParams.clear();
	//QString strPathTemp = QApplication::applicationDirPath() + QStringLiteral("/ykyc_config/config.json");
	QString strPathTemp = QApplication::applicationDirPath() +"/"+ ykyc_config_path + QStringLiteral("/config.json");
	QFile file(strPathTemp);
	if (!file.open(QFile::ReadOnly)) {
		qDebug() << "No File";
		return;
	}
	QString json = file.readAll();
	file.close();

	QJsonParseError error;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toUtf8(), &error);
	if (error.error == QJsonParseError::NoError) {
		if (jsonDocument.isObject()) {
			QVariantMap results = jsonDocument.toVariant().toMap();
			for (auto iter = results.begin(); iter != results.end(); ++iter) {
				//qDebug() << iter.key().toInt();

				oneScreen oneScreen;


				QSize rowAndColumnSize;
				rowAndColumnSize.setWidth(iter.value().toMap().value("rowAndColumnSize").toList()[0].toInt());
				rowAndColumnSize.setHeight(iter.value().toMap().value("rowAndColumnSize").toList()[1].toInt());
				oneScreen.rowAndColumnSize = rowAndColumnSize;


				vector<oneWndData> oneScreenParams;
				for (auto onedata : iter.value().toMap().value("oneScreenParams").toList()) {
					oneWndData oneWndData;

					oneWndData.WINDOW_Name = onedata.toMap().value("WINDOW_Name").toString();
					oneWndData.WINDOW_ID = onedata.toMap().value("WINDOW_ID").toString();
					QSize WINDOW_Size;
					WINDOW_Size.setWidth(onedata.toMap().value("WINDOW_Size").toList()[0].toInt());
					WINDOW_Size.setHeight(onedata.toMap().value("WINDOW_Size").toList()[1].toInt());
					oneWndData.WINDOW_Size = WINDOW_Size;
					QSize WINDOW_FZSize;
					WINDOW_FZSize.setWidth(onedata.toMap().value("WINDOW_FZSize").toList()[0].toInt());
					WINDOW_FZSize.setHeight(onedata.toMap().value("WINDOW_FZSize").toList()[1].toInt());
					oneWndData.WINDOW_FZSize = WINDOW_FZSize;
					oneWndData.WINDOW_SubWndCount = onedata.toMap().value("WINDOW_SubWndCount").toInt();
					vector<QString> WINDOW_SubRowAndColumn;
					for (auto n : onedata.toMap().value("WINDOW_SubRowAndColumn").toList()) {
						WINDOW_SubRowAndColumn.push_back(n.toString());
					}
					oneWndData.WINDOW_SubRowAndColumn = WINDOW_SubRowAndColumn;
					vector<QString> WINDOW_SubWndNames;
					for (auto n : onedata.toMap().value("WINDOW_SubWndNames").toList()) {
						WINDOW_SubWndNames.push_back(n.toString());
					}
					oneWndData.WINDOW_SubWndNames = WINDOW_SubWndNames;
					vector<QSize> WINDOW_SubWndSizes;
					for (auto qz : onedata.toMap().value("WINDOW_SubWndSizes").toList()) {
						QSize WINDOW_Size;
						WINDOW_Size.setWidth(qz.toList()[0].toInt());
						WINDOW_Size.setHeight(qz.toList()[1].toInt());
						WINDOW_SubWndSizes.push_back(WINDOW_Size);
					}
					oneWndData.WINDOW_SubWndSizes = WINDOW_SubWndSizes;

					oneScreenParams.push_back(oneWndData);
				}

				oneScreen.oneScreenParams = oneScreenParams;

				mapScreenParams[iter.key().toInt()] = oneScreen;

			}
		}
	}
}

void ConfigIni::readYaoCeInit()
{
	json yaoce_j;
	QString strPathTemp = QApplication::applicationDirPath() + "/" + ykyc_config_path + QStringLiteral("/yaoce.json");
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
	QString strPathTemp = QApplication::applicationDirPath() + "/" + ykyc_config_path + QStringLiteral("/yaoKongLay.json");
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

ConfigIni* ConfigIni::getInstance(QString ykyc_config_path_)
{
	if (m_instance == NULL)
	{
		m_instance = new ConfigIni(ykyc_config_path_);
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

map<int, oneScreen>* ConfigIni::getAllWndParams()
{
	return &mapScreenParams;
}

//获取遥测界面布局
YaoKongLay* ConfigIni::getYaoKongWndLay()
{
	return &oneYaoKongLay;
}

std::map<QString, YaoCeData>* ConfigIni::getYaoceData()
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
