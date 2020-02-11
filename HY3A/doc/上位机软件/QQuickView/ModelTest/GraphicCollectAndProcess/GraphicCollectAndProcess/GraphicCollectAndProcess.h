#pragma once

#include <QtWidgets/QWidget>
#include "ui_GraphicCollectAndProcess.h"
#include "DataTable.h"
#include "ShowSetting.h"
#include "MyGroupBox.h"
#include "ConfigIni.h"
#include "HistoryReplay.h"
#include <QTimer>
#include <QMenu>
//#include "../../Model/DataAnalysisModel.h"

class DataAnalysisModel;
class GraphicCollectAndProcess : public QWidget
{
	Q_OBJECT

public:
	GraphicCollectAndProcess(oneScreenParams* oneScreenTemp);
	~GraphicCollectAndProcess();

	void initWindows();                                     //窗口初始化
	void setDataAnalysisModel(DataAnalysisModel* dataModelTemp);

private:
	Ui::GraphicCollectAndProcessClass ui;
	DataAnalysisModel* dataModel;

	bool bStartOrStopGrab;          //开始或停止采集
	QPoint m_pressPos;              //鼠标按下位置
	bool bPress;                    //鼠标是否按下
	int iKeyFlag;                   //esc按键标志：1，正常状态；2，隐藏工具栏；3，隐藏标题栏
	QTimer* timer;                   
	QMenu* pop_menu;                //右键菜单
	int iFlagDataType;              //列表十进制或者十六进制显示(0:十进制，1十六进制)
	oneScreenParams* currentScreen;  //当前屏幕配置参数
	int iStartBit;                  //数据解析起始位

	ShowSetting* showSettingWnd;    //显示设置界面
	//SaveSettingWnd* saveWnd;        //存储设置
	DataTable* dataTableFZ;         //辅助数据列表界面
	HistoryReplay* rePlayWnd;       //历史回放界面

	map<QString, QSize> cameraTypeToFZsize;           //相机类型对应辅助数据长度
	map<QString, MyGroupBox*> mapTypeToGraphicGroup;      //类型-图像窗口映射
	map<QString, bool> mapTypeToShowState;                //类型-显示或者隐藏                          

	void initConfigFile();          //配置初始化
	void createMenu();              //创建菜单
	void setDataShowType(int iFlag);//设置显示类型

public slots:
	void on_closeButton_clicked();//关闭
	void on_minButton_clicked();  //最小化
	void on_maxButton_clicked();  //最大化
	void on_pushButton_hideOrshow_clicked();//隐藏显示右侧辅助数据列表

	void on_toolButton_YaoceYaokong_clicked();//点击遥测遥控
	void on_toolButton_TableDisplay_clicked();//辅助数据显示窗口
	void on_toolButton_DataReplay_clicked();  //点击数据回放
	void on_toolButton_ShowSetting_clicked(); //显示设置窗口
	void on_toolButton_SaveSetting_clicked(); //存储设置
	void on_toolButton_StartColl_clicked();   //开始停止采集
	void slot_HistoryReplay(QString strFilePath, bool bStartOrStop); //数据回放

	void slot_ActionGraphic();                //图像显示
	void slot_ActionTable();                  //表格显示
	void slot_ActionLine();                   //曲线显示
	void slot_DataShowType();                 //表格数据类型(十进制或者十六进制)

	void slot_StartBitChanged(int iStartBitTemp);                  //数据解析偏移
	void slot_ModelChanged(QString model);                         //数据模式切换
	void slot_MaxWnd(QPushButton* btn);                            //窗口最大化
	void slot_TimerOut();

 signals:
 	void signal_StartOrStopGrab(bool);                             //开始或者停止采集 
	void signal_SaveData();
	void signal_ReplayData(QString, bool);
	void signal_ChangeModel(int iGraphicWidth);

protected://界面鼠标拖动
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	void contextMenuEvent(QContextMenuEvent *event);
	void keyPressEvent(QKeyEvent *event);

	//void resizeEvent(QResizeEvent *event);
};
