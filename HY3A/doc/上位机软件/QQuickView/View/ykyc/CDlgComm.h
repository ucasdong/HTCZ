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

	oneScreen* curScreen;    //��Ҫ������ȡ��ǰ��Ļ�ϵ�����ͺ�
	YaoKongLay* curYaoKongLay; //��ǰң�ز���
	map<QString, YaoCeData>* curYaoCeData;
	map<QString, QString> mapSendDianYaCmd;

	BYTE* byteCmdBuf;       //���ָ�����
	int byteCamera;         //ѡ���������
	SerialPortClass m_port;
	QTimer* m_timer;

	int iMulSendFlag; //0:ANң�⣬1���м��ң��

	map<QString, vector<QObject*>> mapGroupToSubs;//һ��ָ���Ӧ���������ý���
	map<QString, BYTE*> mapTypeToData;
	QString strCurShowType;

	void initListWidget();  //��ʼ��ң��ָ���б�
	void initTableWidget(); //��ʼ��Ҫ������ʾ���
	void initWindow();      //��ʼ������
	void showCmdInWnd();    //��������ʾ���͵�ָ��
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
	void on_allSendBtn_clicked();  //���еķ��Ͱ�ť�ӿ�
	void on_cameraRadio_clicked(); //�������ѡ��
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
	//ͼ�ɸ�λ
	void on_pushButton_reset_clicked();

	//������ע
	void on_pushButton_upload_file_open_clicked();
	void on_pushButton_upload_send_begin_clicked();
	void on_pushButton_upload_send_pause_clicked();
	void on_pushButton_upload_send_stop_clicked();
	//void on_pushButton_upload_show_failed_clicked();
	void slot_upload_send();

	//ѭ������
	void slot_send_circle();

	void slot_update_yaoce_table();

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

private:
	void trans_cmd(QString cmd_str);
	//ѭ������flag
	bool send_circle_flag;
	//�ѷ��͵�ָ������
	int send_number;
	//ѭ������
	int send_times;
	//ѭ����������
	int send_times_counter;
	//����ѭ�����ͼ���
	int send_counter;
	//ѭ��ָ��������
	int cmd_row_count = 0;

	QFile* cmd_log;
	QTextStream* txt_output;

	//������ע
	int m_package_count;
	int iEndPagNum;             //���һ�����
	void sendDataNew();
	DataBlockBroadcast broadcast;
	vector<BYTE*> vecData;       //��ע��㲥����
	BYTE version;                //�汾��
	int m_time;
	BYTE m_program_edition;
	bool bShangZhu;      //�Ƿ�����ע
	std::map<int, string> failedData;   //����ʧ�ܵİ���ԭ��

	bool m_loadstart;
	void SendFpgaPackage(int iPag);
	bool bOnePakFinish;  //�Ƿ�����ע��

	QTimer upload_timer;
	bool pause_flag;
	bool stop_flag;

	//����Ӧ���־
	bool get_response = true;
	//����ң��ֵ
	BYTE yaoce_data[48] = { 0 };
	QByteArray yaoceByteArray;
	//QTimer yaoceTimer;
	BYTE yaoce_data_2[36] = { 0 };

	//������ʾ
	SerialPortShowWidget* COMX;
};
