#include "SerialPortClass.h"
#include "Common.h"
#include "QsLog.h"

#include <qserialportinfo.h>
#include <qapplication.h>
#include <QMessageBox>

#include <fstream>

#include "json.hpp"
using json = nlohmann::json;

SerialPortClass::SerialPortClass(QString ykyc_config_path)
{
	m_serial = new QSerialPort(this);

	//从串口配置文件中获取配置
	{
		json serial_port_config;
		QString path = QApplication::applicationDirPath() + "/" + ykyc_config_path + QStringLiteral("/serial_port.json");
		std::ifstream istrm;
		istrm.open(path.toStdWString());
		if (!istrm.is_open()) {
			//QMessageBox::critical(this, "ERROR", "failed to open serial_port.json");
			QLOG_ERROR() << path + " " + QStringLiteral("串口配置文件打开失败"); 
			return;
		}
		else {
			istrm >> serial_port_config;
			istrm.close();
		}

		port_name = QString::fromStdString(serial_port_config["port_name"]);
		baudrate = serial_port_config["baudrate"];
	}

	openSerialPort();
	connect(m_serial, SIGNAL(readyRead()), this, SLOT(readData()));

}

SerialPortClass::~SerialPortClass()
{
}

void SerialPortClass::openSerialPort()
{
	//设置默认，用于虚拟串口测试
	//m_serial->setPortName(QStringLiteral("COM3"));
	//自动获取串口
	//for (auto info : QSerialPortInfo::availablePorts()) {
	//	auto temp = info.description();
	//	if (info.description()=="Silicon Labs CP210x USB to UART Bridge") {
	//		m_serial->setPortName(info.portName());
	//	}
	//}
	m_serial->setPortName(port_name);
	m_serial->setBaudRate(baudrate);
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
