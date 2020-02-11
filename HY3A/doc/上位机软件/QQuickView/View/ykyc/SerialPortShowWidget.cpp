#include "SerialPortShowWidget.h"

#include <QApplication>
#include <QMessageBox>

#include <fstream>

#include "QsLog.h"
#include "json.hpp"
using json = nlohmann::json;

//转换字节数组到16进制字符串（加空格）
//std::string byte_array2hexstr_blank(byte* byte_array, int byte_array_length) {
//	//char* hexstr = new char[byte_array_length * 2 + 1]{ 0 };
//	std::string hexstr(byte_array_length * 3 - 1, ' ');
//	static const char* digits = "0123456789ABCDEF";
//	for (int i = 0; i < byte_array_length; i++) {
//		hexstr[i * 3] = digits[byte_array[i] / 16];
//		hexstr[i * 3 + 1] = digits[byte_array[i] % 16];
//	}
//	return hexstr;
//}

SerialPortShowWidget::SerialPortShowWidget(QString config_path, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	json config;
	{
		//配置文件中获取配置，然后打开串口	
		QString path = config_path;
		std::ifstream istrm;
		istrm.open(path.toStdWString());
		if (!istrm.is_open()) {
			QMessageBox::critical(this, "ERROR", path + " " + QStringLiteral("串口配置文件打开失败"));
			QLOG_ERROR() << path + " " + QStringLiteral("串口配置文件打开失败");
			return;
		}
		else {
			istrm >> config;
			istrm.close();
		}
	}
	ui.label_name->setText(QString::fromStdString(config["id"]));
	col_count = config["list_table"]["col_count"];
	row_count = config["list_table"]["row_count"];
	QString port_name = QString::fromStdString(config["serial_port"]["port_name"]);
	int baudrate = config["serial_port"]["baudrate"];
	open_serial_port(port_name, baudrate);

	//默认显示数据
	stop_flag = false;

	//ModelView
	ui.listView_recv->setModel(&list_model);

	//LOG文件
	QDate current_date = QDate::currentDate();
	recv_log = new QFile(QApplication::applicationDirPath() + "/COMX_logs/" + current_date.toString() + ".txt");
	recv_log->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
	txt_output = new QTextStream(recv_log);
	QDateTime current_data_time = QDateTime::currentDateTime();
	QString current_data_time_str = current_data_time.toString();
	(*txt_output) << current_data_time_str << "	" << QStringLiteral("开始记录") << "\n";

	//
	connect(&serial_port, &QSerialPort::readyRead, this, &SerialPortShowWidget::slot_read_data);
	connect(&timer, &QTimer::timeout, this, &SerialPortShowWidget::slot_refresh_list_table);
	//
	connect(ui.pushButton_recv_start, &QPushButton::clicked, this, &SerialPortShowWidget::start_recv);
	connect(ui.pushButton_recv_stop, &QPushButton::clicked, this, &SerialPortShowWidget::stop_recv);
	connect(ui.pushButton_clear, &QPushButton::clicked, this, &SerialPortShowWidget::clear);

	timer.start(100);
}

SerialPortShowWidget::~SerialPortShowWidget()
{
	if (recv_log != nullptr&&recv_log->isOpen()) {
		recv_log->close();
	}
}

void SerialPortShowWidget::open_serial_port(QString port_name, int baudrate)
{
	serial_port.setPortName(port_name);
	serial_port.setBaudRate(baudrate);
	serial_port.setDataBits(QSerialPort::Data8);
	serial_port.setParity(QSerialPort::NoParity);
	serial_port.setStopBits(QSerialPort::OneStop);
	serial_port.setFlowControl(QSerialPort::NoFlowControl);
	if (!serial_port.open(QIODevice::ReadWrite))
	{
		QMessageBox::critical(this, "ERROR", QStringLiteral("串口打开失败"));
		QLOG_ERROR() << QStringLiteral("串口打开失败");
	}
}

void SerialPortShowWidget::start_recv()
{
	stop_flag = false;
}

void SerialPortShowWidget::stop_recv()
{
	stop_flag = true;
	serial_port.clear();
	recv_data.clear();
}

void SerialPortShowWidget::clear()
{
	list.clear();
	list_model.setStringList(list);
}

void SerialPortShowWidget::insert_data(QByteArray data)
{
	QString strShow;
	strShow += QTime::currentTime().toString();
	//QString length_str;
	//length_str += QString::number(length / 100);
	//length_str += QString::number(length % 100 / 10);
	//length_str += QString::number(length % 10);
	strShow += QStringLiteral(" 接收") + QString::number(col_count) + QStringLiteral("字节 ");
	//strShow += QString::fromStdString(byte_array2hexstr_blank(recv_data, length));
	strShow += data.toHex(' ').toUpper();

	*txt_output << strShow << endl;

	list.insert(0, strShow);
	//只显示row_count行
	if (list.size() > row_count) {
		list.pop_back();
	}

}
void SerialPortShowWidget::slot_refresh_list_table()
{
	list_model.setStringList(list);
}

void SerialPortShowWidget::slot_read_data()
{
	if (stop_flag) {
		//clear the internal class buffers and the UART (driver) buffers
		//terminate pending read or write operations
		serial_port.clear();
		recv_data.clear();
		return;
	}
	//int read_length = serial_port.read((char*)recv_data, MAX_LENGTH);
	recv_data += serial_port.readAll();

	int start=recv_data.indexOf(0x31);
	recv_data.remove(0, start);

	//
	while (recv_data.length() > col_count) {
		insert_data(recv_data.mid(0, col_count));
		recv_data.remove(0, col_count);
	}
}
