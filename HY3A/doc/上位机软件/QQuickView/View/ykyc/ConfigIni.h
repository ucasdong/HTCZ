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
	map<QString, int> mapComboxParams;   //combox��Ӧ��map
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
	string WINDOW_Name;                    //�Ӵ�������
	string WINDOW_ID;                      //���ID
	QSize WINDOW_Size;                     //�Ӵ��ڴ�С
	QSize WINDOW_FZSize;                   //�������ݳ���
	int WINDOW_SubWndCount;                //���Ӵ��ڸ���
	vector<string> WINDOW_SubRowAndColumn; //���Ӵ��ڷ���
	vector<string> WINDOW_SubWndNames;     //���Ӵ�������
	vector<QSize> WINDOW_SubWndSizes;      //���Ӵ��ڴ�С
};
*/
struct oneWndData
{
	QString WINDOW_Name;                    //�Ӵ�������
	QString WINDOW_ID;                      //���ID
	QSize WINDOW_Size;                      //�Ӵ��ڴ�С
	QSize WINDOW_FZSize;                    //�������ݳ���
	int WINDOW_SubWndCount;                 //���Ӵ��ڸ���
	vector<QString> WINDOW_SubRowAndColumn; //���Ӵ��ڷ���
	vector<QString> WINDOW_SubWndNames;     //���Ӵ�������
	vector<QSize> WINDOW_SubWndSizes;       //���Ӵ��ڴ�С
};

struct oneScreen
{
	QSize rowAndColumnSize;            //��Ļ����
	vector<oneWndData> oneScreenParams;//һ����Ļ�ڵĴ�������
};

class ConfigIni : public QObject
{
	Q_OBJECT

public:
	~ConfigIni();
	static ConfigIni* getInstance(QString ykyc_config_path_);
	static QMutex* getMutex();
	static void delInstance();
	map<int, oneScreen>* getAllWndParams();                    //��ȡ�����沼��
	YaoKongLay* getYaoKongWndLay();                            //��ȡң�ؽ��沼��
	map<QString, YaoCeData>* getYaoceData();               //��ȡң����Ϣ

	static vector<string> split(string str, string pattern);   //�ַ����и�

	static void WriteLogFile(QString strLog, bool bWrite);     //д��־
private:
	ConfigIni(QString ykyc_config_path_);
	QString ykyc_config_path;

	//�����ڿؼ�����
	static ConfigIni* m_instance;
	static QMutex m_mt;
	vector<oneWndData> allWndParams;
	map<int, oneScreen> mapScreenParams;  //��Ļ��Ӧ�Ĵ�������
	//void readConfigIni();               //��ȡ�����ļ�
	void readConfigInit();                //��ȡ�����ļ�
	void readJsonConfigInit();            //��ȡJson�����ļ�
	//ң��ң�ؿؼ�����
	YaoKongLay oneYaoKongLay;
	map<QString, YaoCeData> mapGroupToYaoceData;
	void readYaoCeInit();                 //��ȡң�ؽ��������ļ�
	void readYaoKongInit();               //��ȡң�ؽ��������ļ�
	void readJsonYaoKongInit();           //��ȡJsonң�ؽ��������ļ�
};
