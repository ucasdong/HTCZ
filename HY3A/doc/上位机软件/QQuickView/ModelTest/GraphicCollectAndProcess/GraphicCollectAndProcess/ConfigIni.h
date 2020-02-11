#pragma once

#include <QObject>
#include <QMutex>
#include <QSize>
#include <vector>
#include <map>
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QPixmap>
#include "Common.h"
#include "../../Include/CommonHeader.h"
using namespace std;


struct OneGroup 
{
	QString Param_GroupName;
	vector<std::pair<QString, QString>> vecTypeToName;
};

struct YaoKongLay
{
	QSize layoutYK;
	map<QString, int> mapComboxParams;   //combox对应的map
	vector<OneGroup> groups;
};

struct YaoCe {
	QString	param_Name;
	int 	param_Type;
	int		param_StartBit;
	int		param_AnalysisBits;
};

struct YaoCeData
{
	int iDataLength;
	vector<YaoCe> vecYaoCeParams;
};


class ConfigIni : public QObject
{
	Q_OBJECT

public:
	~ConfigIni();
	static ConfigIni* getInstance();
	static void delInstance();

	static QMutex* getMutex();
	map<int, oneScreenParams>* getAllWndParams();              //获取主界面布局
	YaoKongLay* getYaoKongWndLay();                            //获取遥测界面布局
	map<QString, YaoCeData>* getYaoceData();                   //获取遥测信息
	static vector<string> split(string str, string pattern);   //字符串切割
	static void WriteLogFile(QString strLog, bool bWrite);     //写日志

private:
	ConfigIni();

	//主窗口控件布局
	static ConfigIni* m_instance;
	static QMutex m_mt;
	map<int, oneScreenParams> mapScreenParams;  //屏幕对应的窗口配置
	void readJsonConfigInit();                  //读取Json配置文件

	//遥测遥控控件布局
	YaoKongLay oneYaoKongLay;
	void readYaoKongInit();               //读取遥控界面配置文件
	void readJsonYaoKongInit();           //读取Json遥控界面配置文件
	map<QString, YaoCeData> mapGroupToYaoceData;
	void readYaoCeInit();                 //读取遥控界面配置文件
};
