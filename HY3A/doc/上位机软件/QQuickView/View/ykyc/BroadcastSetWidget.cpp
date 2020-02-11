#include "BroadcastSetWidget.h"

#include <qmessagebox.h>

#include <QsLog.h>

BroadcastSetWidget::BroadcastSetWidget(json broadcast_set_config_, QSerialPort* serial_port_, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	broadcast_set_config = broadcast_set_config_;
	serial_port = serial_port_;

	fill_col_begin = -1;
	fill_col_length = 0;

	cmd = new BYTE[broadcast_set_config["cmd_length"]];

	this->setWindowTitle(QString::fromStdString(broadcast_set_config["id"]));
	init_table();

	//填充默认值
	on_pushButton_fixed_clicked();
}

BroadcastSetWidget::~BroadcastSetWidget()
{
}

void BroadcastSetWidget::init_table()
{
	QTableWidget* table = ui.tableWidget;
	json& config = broadcast_set_config;

	table->setColumnCount(config["cmd_length"]);
	//第一行用来显示表头
	table->setRowCount(config["cmd_count"] + 1);

	table->horizontalHeader()->hide();
	table->verticalHeader()->hide();
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//自适应列宽

	//填充表格内的固定值
	int col_counter = 0;
	for (auto col_header : config["table"]) {

		if (col_header["type"] == "fixed" || col_header["type"] == "fixed_hide") {

			table->setSpan(0, col_counter, 1, col_header["length"]);

			QString fixed_value = QString::fromStdString(col_header["value"]);
			for (int i = 1; i < table->rowCount(); i++) {
				for (int j = 0; j < col_header["length"]; j++) {
					table->setItem(i, col_counter + j, new QTableWidgetItem(fixed_value.mid(j * 2, 2)));
				}
			}

			//有名字的固定列
			if (col_header["type"] == "fixed") {
				table->setItem(0, col_counter, new QTableWidgetItem(QString::fromStdString(col_header["id"])));
			}
			//需要隐藏的固定列
			if (col_header["type"] == "fixed_hide") {
				for (int col = col_counter; col < col_counter + col_header["length"]; col++) {
					table->hideColumn(col);
				}
			}

			col_counter += col_header["length"];
		}
		else if (col_header["type"] == "index" || col_header["type"] == "index_hide") {
			table->setItem(0, col_counter, new QTableWidgetItem(QStringLiteral("index")));
			table->setSpan(0, col_counter, 1, col_header["length"]);

			unsigned long long index = 0;
			for (int i = 1; i < table->rowCount(); i++) {
				for (int j = 0; j < col_header["length"]; j++) {
					BYTE cur_index = index >> ((col_header["length"] - j - 1) * 2);
					table->setItem(i, col_counter + j, new QTableWidgetItem(QString::number(cur_index, 16).toUpper()));
					index++;
				}
			}

			//需要隐藏的固定index
			if (col_header["type"] == "index_hide") {
				for (int col = col_counter; col < col_counter + col_header["length"]; col++) {
					table->hideColumn(col);
				}
			}

			col_counter += col_header["length"];
		}
		else if (col_header["type"] == "data") {
			table->setItem(0, col_counter, new QTableWidgetItem(QString::fromStdString(col_header["id"])));
			table->setSpan(0, col_counter, 1, col_header["length"]);

			for (int i = 1; i < table->rowCount(); i++) {
				for (int j = 0; j < col_header["length"]; j++) {
					table->setItem(i, col_counter + j, new QTableWidgetItem("0"));
				}
			}

			//找到需要填充数据的位置
			if (fill_col_begin == -1) {
				fill_col_begin = col_counter;
			}
			fill_col_length += col_header["length"];

			col_counter += col_header["length"];
		}
	}

	//未配置的预留填充0
	for (int row = 1; row < table->rowCount(); row++) {
		for (int col = col_counter; col < table->columnCount(); col++) {
			table->setItem(row, col, new QTableWidgetItem("0"));
		}
	}

}

void BroadcastSetWidget::on_pushButton_fixed_clicked()
{
	QTableWidget* table = ui.tableWidget;
	QString value = QString::number(ui.spinBox_value->value(), 16).toUpper();

	//填充指定值
	for (int row = 1; row < table->rowCount(); row++) {
		for (int col = fill_col_begin; col < fill_col_begin + fill_col_length; col++) {
			table->setItem(row, col, new QTableWidgetItem(value));
		}
	}
	//填充数据中的LT
	QString LT = QString::fromStdString(broadcast_set_config["LT_header"]);
	table->setItem(1, fill_col_begin, new QTableWidgetItem(LT.mid(0, 2)));
	table->setItem(1, fill_col_begin + 1, new QTableWidgetItem(LT.mid(2, 4)));
	//填充数据校验和
	int SUM = (fill_col_length * (table->rowCount() - 1) - 3)*ui.spinBox_value->value();
	//校验和从T开始算
	SUM += LT.mid(2, 4).toInt(nullptr, 16);
	table->setItem(table->rowCount() - 1, fill_col_begin + fill_col_length - 1, new QTableWidgetItem(QString::number(SUM % 256, 16).toUpper()));
}

void BroadcastSetWidget::on_pushButton_gradual_clicked()
{
	QTableWidget* table = ui.tableWidget;
	int value = ui.spinBox_value->value();

	//LT先填充 -2 -1
	value = value - 2;
	for (int row = 1; row < table->rowCount(); row++) {
		for (int col = fill_col_begin; col < fill_col_begin + fill_col_length; col++) {
			table->setItem(row, col, new QTableWidgetItem(QString::number(value % 256, 16).toUpper()));
			value++;
		}
	}
	//填充数据中的LT
	QString LT = QString::fromStdString(broadcast_set_config["LT_header"]);
	table->setItem(1, fill_col_begin, new QTableWidgetItem(LT.mid(0, 2)));
	table->setItem(1, fill_col_begin + 1, new QTableWidgetItem(LT.mid(2, 4)));
	//填充数据校验和
	int first_value = table->item(1, fill_col_begin + 2)->text().toInt(nullptr, 16);
	int value_count = fill_col_length * (table->rowCount() - 1) - 3;
	int SUM = value_count * (first_value + first_value + value_count - 1) / 2;
	//校验和从T开始算
	SUM += LT.mid(2, 4).toInt(nullptr,16);
	table->setItem(table->rowCount() - 1, fill_col_begin + fill_col_length - 1, new QTableWidgetItem(QString::number(SUM % 256, 16).toUpper()));
}

void BroadcastSetWidget::on_pushButton_send_clicked()
{
	QTableWidget* table = ui.tableWidget;

	for (int row = 1; row < table->rowCount(); row++) {
		for (int col = 0; col < table->columnCount(); col++) {
			cmd[col] = table->item(row, col)->text().toInt(nullptr, 16);
		}
		serial_port->write((char*)cmd, broadcast_set_config["cmd_length"]);
	}
	QLOG_INFO() << this->windowTitle() + QStringLiteral("发送成功");
	//QMessageBox::information(this, "INFO", QStringLiteral("发送成功"));
	ui.label_status->setText(QStringLiteral("发送成功"));
}