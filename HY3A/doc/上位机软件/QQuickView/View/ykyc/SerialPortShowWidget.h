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

	//�Ƿ�ֹͣ��������
	bool stop_flag;
	
	//���б��в�����յ�������
	void insert_data(QByteArray data);

	//��ʷָ��
	QFile* recv_log;
	QTextStream* txt_output;

	//��ʱ����ÿ��ˢ��10��
	QTimer timer;

	//ÿ����ʾ���ֽ���
	int col_count;
	//��ʾ����
	int row_count;


public slots:
	void slot_read_data();
	void slot_refresh_list_table();
};
