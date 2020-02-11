#pragma once

#include <QObject>
#include <QSerialPort>
#include "Common.h"

class SerialPortClass : public QObject
{
	Q_OBJECT

public:
	SerialPortClass(QString ykyc_config_path);
	~SerialPortClass();

	void openSerialPort();
	void writeData(char* data);
	QSerialPort *m_serial = nullptr;
	QString port_name;
	int baudrate;

signals:
	void recvSerialData(BYTE*, int);

public slots:
	void readData();
};
