#pragma once

#include <QWidget>
#include "ui_SerialPortShowWidget.h"

#include <QSerialPort>
#include <QFile>
#include <QStringList>
#include <QStringListModel>
#include <QTimer>
#include <qbytearray.h>

#include <CommonHeader.h>

class SerialPortShowWidget : public QWidget
{
	Q_OBJECT

public:
	SerialPortShowWidget(QString config_path, QWidget *parent = Q_NULLPTR);
	~SerialPortShowWidget();

	void open_serial_port(QString port_name, int baudrate);
	void start_recv();
	void stop_recv();
	void clear();

private:
	Ui::SerialPortShowWidget ui;

	QSerialPort serial_port;
	QByteArray recv_data;

	//
	QStringList list;
	QStringListModel list_model;

	//是否停止插入数据
	bool stop_flag;
	
	//在列表中插入接收到的数据
	void insert_data(QByteArray data);

	//历史指令
	QFile* recv_log;
	QTextStream* txt_output;

	//定时器，每秒刷新10次
	QTimer timer;

	//每行显示的字节数
	int col_count;
	//显示行数
	int row_count;


public slots:
	void slot_read_data();
	void slot_refresh_list_table();
};
