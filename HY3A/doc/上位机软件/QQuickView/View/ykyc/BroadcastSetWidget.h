#pragma once

#include <QWidget>
#include <QDialog>
#include "ui_BroadcastSetWidget.h"

#include <QSerialPort>

#include "CommonHeader.h"

#include "json.hpp"
using json = nlohmann::json;

class BroadcastSetWidget : public QDialog
{
	Q_OBJECT

public:
	BroadcastSetWidget(json broadcast_set_config_, QSerialPort* serial_port_,QWidget *parent = Q_NULLPTR);
	~BroadcastSetWidget();

private:
	Ui::BroadcastSetWidget ui;

	json broadcast_set_config;
	QSerialPort* serial_port;
	//单条指令
	BYTE* cmd;

	void init_table();

	//列填充的开始和结束
	int fill_col_begin;
	int fill_col_length;

public slots:
	void on_pushButton_fixed_clicked();
	void on_pushButton_gradual_clicked();
	void on_pushButton_send_clicked();

};
