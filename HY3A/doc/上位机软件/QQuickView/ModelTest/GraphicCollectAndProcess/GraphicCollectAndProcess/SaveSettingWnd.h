#pragma once

#include <QWidget>
#include "ui_SaveSettingWnd.h"
#include "SaveData.h"
#include "Common.h"
#include "CodeDataAnalysis.h"
#include "ConfigIni.h"
#include <QCheckBox>
#include <QRadioButton>

class SaveSettingWnd : public QWidget
{
	Q_OBJECT

public:
	SaveSettingWnd();
	~SaveSettingWnd();

	void insertSaveData(QString strName, BYTE* saveDataTemp, int iLen); //插入要存储的数据
	//int getSaveFlag();
private:
	Ui::SaveSettingWnd ui;

	CodeDataAnalysis codeAnalysis;           //数据解析

	QPoint m_pressPos;                       //窗口移动鼠标位置
	bool bPress;                             //窗口移动鼠标是否按下
	
    //界面参数-相机选择
	map<QString, QCheckBox*> mapCameraIdToCheckBox;
	map<QString, bool> mapDataSaveFlag;

	//界面参数-谱段选择
	map<QString, QRadioButton*> mapPDIdToRadioBtn;
    map<QString, bool> mapPDFlag;
	int iPDFlag;                             //0:原始数据，1：分普段
	//界面参数-基本设置  
	bool bSaveSetting;                       //存储设置是否成功
	QString strSavePath;                     //所有数据存储路径
	QString strFzPath;                       //辅助数据存放路径
//	int iSaveFlag;

	bool bSaveType;                          //存储类型：长时间存储/约定行数存储
	bool bStartOrStop;                       //开始或者停止存储
	bool bPauseOrContinue;                   //暂停或者继续
	//界面参数-长时间存储设置
	QString strInformation1;                 //自定义说明部分
	double dSaveTime;                        //存储时间
	//界面参数-约定行数存储设置
	int iSaveCount;                          //存储行数
	QString strInformation2;                 //自定义说明部分
	int iFileType;                           //文件格式 1:Raw, 2:Bmp, 3:TiFF
	QString strFileFormat;

	//控制存储结束
	qint64 qintStartTime;                    //开始存储时间
	qint64 iPauseCount;                      //约定行数计数
	qint64 iPauseCountFZ;                    //约定行数计数
	//按普段存储
	QStringList strNameList;
//	BYTE* P_Data;                                //p普段数据
//	BYTE* B1_Data;                               //b1普段数据
//	BYTE* B2_Data;                               //b2普段数据
//	BYTE* B3_Data;                               //b3普段数据
//	BYTE* B4_Data;                               //b4普段数据

	//连续长时间存储使用内存映射文件的存储方式
	std::map<QString, SaveData*> mapTypeToAllData;   //存储所有数据
	std::map<QString, SaveData*> mapTypeToFZData;    //存储辅助数据
	std::map<QString, std::map<QString, SaveData*>> mapTypeToPuDuan;    //存储普段数据
	//约定行数存储，当前采用方法是先把所有数据存到一个文件中，存储结束后对文件处理（需要提前保存文件名）
	QString qintStartPauseTime;                      //开始存储约定行文件时间
	std::map<QString, QString> mapPauseCountFile;    //约定行数文件路径  
	std::map<QString, SaveData*> mapTypeToPauseData; //存储约定行数数据

//	void PausefilesSplit();                          //约定文件拆分
//	QString getFZFileName(BYTE* dataTemp);

public slots:
	void on_closeButton_clicked();
	void on_minButton_clicked();
	void on_maxButton_clicked();
	void on_pushButton_ChoosePath_clicked();           //选择数据存储路径
	//void on_pushButton_ChoosePathFz_clicked();         //选择辅助数据存储路径
	void on_pushButton_sure_clicked();                 //点击保存设置
	void on_pushButton_StartOrStop_clicked();          //点击开始存储
	void on_pushButton_PauseOrContinue_clicked();      //点击暂停存储
	void on_groupBox_LongTime_clicked(bool bCheck);    //点击长时间存储模式
	void on_groupBox_FixedRow_clicked(bool bCheck);    //点击约定行数模式
	void on_PDRadioButton_clicked();
//	void on_pushButton_SaveCCSDS_clicked();

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};
