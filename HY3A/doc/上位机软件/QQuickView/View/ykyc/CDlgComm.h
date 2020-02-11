#pragma once

#include <QtWidgets/QWidget>
#include "ui_CDlgComm.h"
#include "ConfigIni.h"
#include "Common.h"
#include "SerialPortClass.h"
#include <QTimer>
#include <QPushButton>
#include <QDialog>
#include "broadcast/DataBlockBroadcast.h"
#include "SerialPortShowWidget.h"
#include "json.hpp"
using json = nlohmann::json;

class CDlgComm : public QDialog
{
	Q_OBJECT

public:
	CDlgComm(QWidget* parent, oneScreen* currentScreen, YaoKongLay* yaoKongLay, map<QString, YaoCeData>* yaoCeData, QString ykyc_config_path);
	~CDlgComm();

private:
	Ui::CDlgCommClass ui;

	json ykyc;

	QPoint m_pressPos;
	bool bPress;

	oneScreen* curScreen;    //主要用来获取当前屏幕上的相机型号
	YaoKongLay* curYaoKongLay; //当前遥控布局
	map<QString, YaoCeData>* curYaoCeData;
	map<QString, QString> mapSendDianYaCmd;

	BYTE* byteCmdBuf;       //存放指令缓冲区
	int byteCamera;         //选择相机类型
	SerialPortClass m_port;
	QTimer* m_timer;

	int iMulSendFlag; //0:AN遥测，1：行检测遥测

	map<QString, vector<QObject*>> mapGroupToSubs;//一条指令对应的所有设置界面
	map<QString, BYTE*> mapTypeToData;
	QString strCurShowType;

	void initListWidget();  //初始化遥控指令列表
	void initTableWidget(); //初始化要测试显示表格
	void initWindow();      //初始化窗口
	void showCmdInWnd();    //界面中显示发送的指令
	void showCmdInWnd(BYTE* data, int length);
	void updateDianYaTableData();
	void updateYaoceTableData();
	void updateYaoceTableData_2();
	void changeYaoceTable(QString strType);
public:
	bool bSend;
	bool recvSerialData;
	int iIndex = 0;
	QStringList strRowList;

public slots:
	void on_closeButton_clicked();
	void on_minButton_clicked();
	void on_allSendBtn_clicked();  //所有的发送按钮接口
	void on_cameraRadio_clicked(); //相机类型选择
	void slot_sendGetDYCmd();
	void slot_recvSerialData(BYTE* recvData_, int iLen);

	void on_pushButton_folder_clicked();
	void on_pushButton_cmd_name_clicked();
	void on_pushButton_cmd_name_custom_clicked();
	void on_pushButton_cmd_value_clicked();
	void on_pushButton_send_clicked();
	void on_pushButton_send_circle_clicked();
	void on_pushButton_send_circle_stop_clicked();
	void on_pushButton_broadcast_switch_send_clicked();
	void on_pushButton_yaoce_control_send_clicked();
	void on_pushButton_other_1_clicked();
	void on_pushButton_other_2_clicked();
	void on_pushButton_other_3_clicked();
	void on_pushButton_other_4_clicked();
	void on_pushButton_other_5_clicked();

	void on_pushButton_save_path_clicked();
	//图采复位
	void on_pushButton_reset_clicked();

	//程序上注
	void on_pushButton_upload_file_open_clicked();
	void on_pushButton_upload_send_begin_clicked();
	void on_pushButton_upload_send_pause_clicked();
	void on_pushButton_upload_send_stop_clicked();
	//void on_pushButton_upload_show_failed_clicked();
	void slot_upload_send();

	//循环发送
	void slot_send_circle();

	void slot_update_yaoce_table();

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

private:
	void trans_cmd(QString cmd_str);
	//循环发送flag
	bool send_circle_flag;
	//已发送的指令数量
	int send_number;
	//循环次数
	int send_times;
	//循环次数计数
	int send_times_counter;
	//单次循环发送计数
	int send_counter;
	//循环指令表格行数
	int cmd_row_count = 0;

	QFile* cmd_log;
	QTextStream* txt_output;

	//程序上注
	int m_package_count;
	int iEndPagNum;             //最后一包序号
	void sendDataNew();
	DataBlockBroadcast broadcast;
	vector<BYTE*> vecData;       //上注块广播数据
	BYTE version;                //版本号
	int m_time;
	BYTE m_program_edition;
	bool bShangZhu;      //是否在上注
	std::map<int, string> failedData;   //发送失败的包和原因

	bool m_loadstart;
	void SendFpgaPackage(int iPag);
	bool bOnePakFinish;  //是否在上注包

	QTimer upload_timer;
	bool pause_flag;
	bool stop_flag;

	//串口应答标志
	bool get_response = true;
	//返回遥测值
	BYTE yaoce_data[48] = { 0 };
	QByteArray yaoceByteArray;
	//QTimer yaoceTimer;
	BYTE yaoce_data_2[36] = { 0 };

	//串口显示
	SerialPortShowWidget* COMX;
};
