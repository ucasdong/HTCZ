#pragma once

#include <QObject>
#include <QSerialPort>
#include "Common.h"

class SerialPortClass : public QObject
{
	Q_OBJECT

public:
	SerialPortClass();
	~SerialPortClass();

	void openSerialPort();
	void writeData(char* data);
	QSerialPort *m_serial = nullptr;

signals:
	void recvSerialData(BYTE*, int);

public slots:
	void readData();
};
