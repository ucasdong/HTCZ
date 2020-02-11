#include "SerialPortClass.h"
#include "Common.h"
SerialPortClass::SerialPortClass()
{
	m_serial = new QSerialPort(this);
	openSerialPort();
	connect(m_serial, SIGNAL(readyRead()), this, SLOT(readData()));
	
}

SerialPortClass::~SerialPortClass()
{
}

void SerialPortClass::openSerialPort()
{
	m_serial->setPortName(QStringLiteral("COM10"));
	m_serial->setBaudRate(BAUDRATE);
	m_serial->setDataBits(QSerialPort::Data8);
	m_serial->setParity(QSerialPort::NoParity);
	m_serial->setStopBits(QSerialPort::OneStop);
	m_serial->setFlowControl(QSerialPort::NoFlowControl);
	if (m_serial->open(QIODevice::ReadWrite))
	{
		int a = 0;
	}
	else
	{
		int a = 0;
	}
}

void SerialPortClass::writeData(char* data)
{
	m_serial->write(data, CmdLength);
}

void SerialPortClass::readData()
{
	const QByteArray data = m_serial->readAll();
	BYTE* dataRecv = new BYTE[data.length()];
	memcpy(dataRecv, data, data.length());
	emit recvSerialData(dataRecv, data.length());
}
