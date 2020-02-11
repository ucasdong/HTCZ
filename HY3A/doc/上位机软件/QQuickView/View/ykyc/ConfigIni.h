#pragma once

#include <QObject>
#include <QMutex>
#include <QSize>
#include <vector>
#include <map>
#include <iostream>
#include <QFile>
#include <QTextStream>

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

/*
struct oneWndData
{
	string WINDOW_Name;                    //子窗口名称
	string WINDOW_ID;                      //相机ID
	QSize WINDOW_Size;                     //子窗口大小
	QSize WINDOW_FZSize;                   //辅助数据长度
	int WINDOW_SubWndCount;                //子子窗口个数
	vector<string> WINDOW_SubRowAndColumn; //子子窗口分区
	vector<string> WINDOW_SubWndNames;     //子子窗口名称
	vector<QSize> WINDOW_SubWndSizes;      //子子窗口大小
};
*/
struct oneWndData
{
	QString WINDOW_Name;                    //子窗口名称
	QString WINDOW_ID;                      //相机ID
	QSize WINDOW_Size;                      //子窗口大小
	QSize WINDOW_FZSize;                    //辅助数据长度
	int WINDOW_SubWndCount;                 //子子窗口个数
	vector<QString> WINDOW_SubRowAndColumn; //子子窗口分区
	vector<QString> WINDOW_SubWndNames;     //子子窗口名称
	vector<QSize> WINDOW_SubWndSizes;       //子子窗口大小
};

struct oneScreen
{
	QSize rowAndColumnSize;            //屏幕布局
	vector<oneWndData> oneScreenParams;//一个屏幕内的窗口配置
};

class ConfigIni : public QObject
{
	Q_OBJECT

public:
	~ConfigIni();
	static ConfigIni* getInstance(QString ykyc_config_path_);
	static QMutex* getMutex();
	static void delInstance();
	map<int, oneScreen>* getAllWndParams();                    //获取主界面布局
	YaoKongLay* getYaoKongWndLay();                            //获取遥控界面布局
	map<QString, YaoCeData>* getYaoceData();               //获取遥测信息

	static vector<string> split(string str, string pattern);   //字符串切割

	static void WriteLogFile(QString strLog, bool bWrite);     //写日志
private:
	ConfigIni(QString ykyc_config_path_);
	QString ykyc_config_path;

	//主窗口控件布局
	static ConfigIni* m_instance;
	static QMutex m_mt;
	vector<oneWndData> allWndParams;
	map<int, oneScreen> mapScreenParams;  //屏幕对应的窗口配置
	//void readConfigIni();               //读取配置文件
	void readConfigInit();                //读取配置文件
	void readJsonConfigInit();            //读取Json配置文件
	//遥测遥控控件布局
	YaoKongLay oneYaoKongLay;
	map<QString, YaoCeData> mapGroupToYaoceData;
	void readYaoCeInit();                 //读取遥控界面配置文件
	void readYaoKongInit();               //读取遥控界面配置文件
	void readJsonYaoKongInit();           //读取Json遥控界面配置文件
};
