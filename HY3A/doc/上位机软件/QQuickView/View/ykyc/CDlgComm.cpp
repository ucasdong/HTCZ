#include "CDlgComm.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QFileDialog>
#include <QMouseEvent>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QRadioButton>
#include <QMessageBox>
#include <QTextCodec>
#include <math.h>
#include <windows.h>
#include <fstream>
#include <QDateTime>
#include <QTime>
#include "QsLog.h"
#include <process.h>
#include <thread>
#include "IBussiness.h"

#include "BroadcastSetWidget.h"

//ת��16�����ַ������ֽ�����
//���ַ�����ת����Ĭ��Ϊ0
void hexstr2byte_array(const char* hexstr, byte* byte_array, int byte_array_length) {
	char hex[2];
	for (int i = 0; i < byte_array_length; i++) {
		hex[0] = *(hexstr + i * 2);
		hex[1] = *(hexstr + i * 2 + 1);
		if (hex[0] == '\0' || hex[1] == '\0') {
			return;
		}
		byte_array[i] = strtol(hex, nullptr, 16);
	}
}


//ת���ֽ����鵽16�����ַ���
std::string byte_array2hexstr(byte* byte_array, int byte_array_length) {
	//char* hexstr = new char[byte_array_length * 2 + 1]{ 0 };
	std::string hexstr(byte_array_length * 2, '0');
	static const char* digits = "0123456789ABCDEF";
	for (int i = 0; i < byte_array_length; i++) {
		hexstr[i * 2] = digits[byte_array[i] / 16];
		hexstr[i * 2 + 1] = digits[byte_array[i] % 16];
	}
	return hexstr;
}

CDlgComm::CDlgComm(QWidget* parent, oneScreen* currentScreen, YaoKongLay* yaoKongLay, map<QString, YaoCeData>* yaoCeData, QString ykyc_config_path)
	: QDialog(parent), m_port(ykyc_config_path)
{
	ui.setupUi(this);

	ui.tableWidget_DianYa->hide();
	ui.tableWidget_lineTest->hide();
	ui.pushButton_folder->hide();
	ui.doubleSpinBox_speed->hide();
	ui.pushButton_upload_show_failed->hide();

	ui.horizontalGroupBox_reset->hide();

	//�㲥��������
	{
		json broadcast_set_config;

		QString path = QApplication::applicationDirPath() + "/" + ykyc_config_path + QStringLiteral("/broadcast_set.json");
		std::ifstream istrm;
		istrm.open(path.toStdWString());
		if (!istrm.is_open()) {
			QMessageBox::critical(this, "ERROR", "failed to open broadcast_set.json");
			return;
		}
		else {
			istrm >> broadcast_set_config;
			istrm.close();
		}

		for (auto one_set : broadcast_set_config) {
			QPushButton* button = new QPushButton(QString::fromStdString(one_set["id"]), this);
			BroadcastSetWidget* dialog = new BroadcastSetWidget(one_set, m_port.m_serial, this);
			connect(button, &QPushButton::clicked, dialog, &BroadcastSetWidget::showNormal);
			ui.horizontalLayout_broadcat_set->addWidget(button);
		}
	}


	//��ʷָ��浽�ļ�
	QDate current_date = QDate::currentDate();
	cmd_log = new QFile(QApplication::applicationDirPath() + "/history_cmd_logs/" + current_date.toString() + ".txt");
	cmd_log->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
	txt_output = new QTextStream(cmd_log);
	QDateTime current_data_time = QDateTime::currentDateTime();
	QString current_data_time_str = current_data_time.toString();
	(*txt_output) << current_data_time_str << "	" << QStringLiteral("��ʼ��¼") << "\n";
	//cmd_log->flush();

	//��ȡ��ע�����ļ�
	broadcast.readBroadcastFile((QApplication::applicationDirPath() + "/" + ykyc_config_path + QStringLiteral("/BroadcastData.ini")).toStdWString());

	//����ykyc�����ļ�
	{
		QString path = QApplication::applicationDirPath() + "/" + ykyc_config_path + QStringLiteral("/ykyc.json");
		std::ifstream istrm;
		istrm.open(path.toStdWString());
		if (!istrm.is_open()) {
			QMessageBox::critical(this, "ERROR", "failed to open json file");
			return;
		}
		else {
			istrm >> ykyc;
			istrm.close();
		}
	}

	if (ykyc["id"] == "HY3_hongwai") {
		//��ʾCOMX����������
		COMX = new SerialPortShowWidget(QApplication::applicationDirPath() + "/" + ykyc_config_path + QStringLiteral("/SerialPortShow.json"));
		ui.verticalLayout_COMX->addWidget(COMX);

		ui.radioButton_yaoce_control_1->hide();
		ui.radioButton_yaoce_control_3->hide();
		ui.radioButton_yaoce_control_5->hide();
		ui.radioButton_yaoce_control_6->hide();
		ui.radioButton_yaoce_control_7->hide();
		ui.radioButton_yaoce_control_8->hide();
		//���ؿɼ���Ƶ�������������л�
		ui.horizontalGroupBox_6->hide();
		//����CAN�л�
		//ui.horizontalGroupBox_4->hide();
	}
	if (ykyc["id"] == "HY3_kejian") {
		ui.radioButton_yaoce_control_2->hide();
		ui.radioButton_yaoce_control_4->hide();
		ui.radioButton_yaoce_control_5->hide();
		ui.radioButton_yaoce_control_6->hide();
		ui.radioButton_yaoce_control_7->hide();
		ui.radioButton_yaoce_control_8->hide();
		//���غ�����Ƶ�������������л�
		ui.horizontalGroupBox_5->hide();
	}

	//setWindowFlags(Qt::FramelessWindowHint);//�ޱ߿� 
	//setMouseTracking(true);
	//QString strPath = QApplication::applicationDirPath() + "\\style\\styles.qss";
	QFile qss(":/QQuickView/View/qss/default.qss");
	qss.open(QFile::ReadOnly);
	setStyleSheet(qss.readAll());
	qss.close();
	//ui.label_table->setStyleSheet(" background-color: rgba(26, 48, 80, 255);");
	//ui.maxButton->hide();

	curScreen = currentScreen;
	curYaoKongLay = yaoKongLay;
	curYaoCeData = yaoCeData;
	byteCmdBuf = new BYTE[ykyc["yao_kong"]["cmd_length"]]; //ң��ָ�����
	memset(byteCmdBuf, 0x00, CmdLength);
	bSend = false;
	iIndex = 0;
	initTableWidget();
	initWindow();

	recvSerialData = false;
	connect(&m_port, SIGNAL(recvSerialData(BYTE*, int)), this, SLOT(slot_recvSerialData(BYTE*, int)));

	//m_timer = new QTimer;
	//connect(m_timer, SIGNAL(timeout()), this, SLOT(slot_sendGetDYCmd()));

	//m_timer->start(500);
// 
// 	QDesktopWidget *pDesk = QApplication::desktop();
// 	this->move((pDesk->width() - this->width()), (pDesk->height() - this->height()));

		//����ѭ������ָ��
	m_timer = new QTimer;
	//connect(m_timer, &QTimer::timeout, this, &CDlgComm::on_pushButton_send_clicked);
	connect(m_timer, &QTimer::timeout, this, &CDlgComm::slot_send_circle);

	//��ʷָ���б�˫���󣬿�����ָ��ִ�п�
	connect(ui.listWidget_CmdList, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* cmd_item) {
		QString cmd = cmd_item->text();
		//cmd = cmd.replace(" ", "");
		cmd = cmd.mid(12, CmdLength * 3);
		this->ui.lineEdit_cmd->setText(cmd);
		trans_cmd(cmd);
	});
	//�ļ�ָ���б�˫���󣬿�����ָ��ִ�п�
	connect(ui.tableWidget_cmd, &QTableWidget::itemDoubleClicked, [this](QTableWidgetItem* cmd_item) {
		this->ui.lineEdit_cmd->setText(cmd_item->text());
		trans_cmd(cmd_item->text());
	});

	//ÿ���༭���µ�ָ��ʱ���Զ���ָ���ַ���ת��Ϊ�ֽ�����
	connect(ui.lineEdit_cmd, &QLineEdit::editingFinished, [this]() {
		trans_cmd(this->ui.lineEdit_cmd->text());
	});

	//����ѭ��������עָ��
	connect(&upload_timer, &QTimer::timeout, this, &CDlgComm::slot_upload_send);

	//connect(&yaoceTimer, &QTimer::timeout, this, &CDlgComm::slot_update_yaoce_table);
	//yaoceTimer.start(1000);
}

void CDlgComm::showCmdInWnd()
{
	QString strShow = "";
	strShow += QTime::currentTime().toString();
	strShow += QStringLiteral(" ���� ");
	for (int i = 0; i < CmdLength; i++)
	{
		strShow += QString("%1").arg(byteCmdBuf[i], 2, 16, QLatin1Char('0')).toUpper();
		strShow += " ";
	}
	QListWidgetItem* item = new QListWidgetItem(strShow);
	//item->setTextAlignment(Qt::AlignCenter);
	//ui.listWidget_CmdList->addItem(item);
	ui.listWidget_CmdList->insertItem(0, item);
	//ui.listWidget_CmdList->update();

	*txt_output << strShow << endl;

	QLOG_INFO() << QStringLiteral("����ָ��") + strShow;
	//cmd_log->flush();
}
void CDlgComm::showCmdInWnd(BYTE* data, int length)
{
	QString strShow = "";
	strShow += QTime::currentTime().toString();
	strShow += QStringLiteral(" ���� ");
	for (int i = 0; i < length; i++)
	{
		strShow += QString("%1").arg(data[i], 2, 16, QLatin1Char('0')).toUpper();
		strShow += " ";
	}
	QListWidgetItem* item = new QListWidgetItem(strShow);
	//item->setTextAlignment(Qt::AlignCenter);
	//ui.listWidget_CmdList->addItem(item);
	ui.listWidget_CmdList->insertItem(0, item);
	//ui.listWidget_CmdList->update();

	*txt_output << strShow << endl;

	QLOG_INFO() << QStringLiteral("����ָ��") + strShow;
	//cmd_log->flush();
}

void CDlgComm::updateDianYaTableData()
{
	if (!recvSerialData)
	{
		return;
	}
	if (!bSend)
	{
		ui.tableWidget_DianYa->item(0, 1)->setText(QString::number(0.00));
		ui.tableWidget_DianYa->item(1, 1)->setText(QString::number(0.00));
		ui.tableWidget_DianYa->item(2, 1)->setText(QString::number(0.00));
		ui.tableWidget_DianYa->item(3, 1)->setText(QString::number(0.00));
		ui.tableWidget_DianYa->item(4, 1)->setText(QString::number(0.00));
		ui.tableWidget_DianYa->item(5, 1)->setText(QString::number(0.00));
	}
	else
	{
		map<QString, QString>::iterator it = mapSendDianYaCmd.find(QStringLiteral("���ݼӵ�1"));
		if (it != mapSendDianYaCmd.end())
		{
			ui.tableWidget_DianYa->item(0, 1)->setText(QString::number(4.992));
		}
		else
		{
			ui.tableWidget_DianYa->item(0, 1)->setText(QString::number(0.924));
		}

		it = mapSendDianYaCmd.find(QStringLiteral("���ݼӵ�2"));
		if (it != mapSendDianYaCmd.end())
		{
			ui.tableWidget_DianYa->item(1, 1)->setText(QString::number(4.992));
			if (iIndex % 6 == 5)
			{
				ui.tableWidget_DianYa->item(2, 1)->setText(QString::number(4.998));
			}
			else if (iIndex % 6 == 0)
			{
				ui.tableWidget_DianYa->item(2, 1)->setText(QString::number(4.992));
			}
		}
		else
		{
			ui.tableWidget_DianYa->item(1, 1)->setText(QString::number(0.924));
			ui.tableWidget_DianYa->item(2, 1)->setText(QString::number(0.924));
		}

		it = mapSendDianYaCmd.find(QStringLiteral("���ݼӵ�1"));
		if (it != mapSendDianYaCmd.end())
		{
			ui.tableWidget_DianYa->item(3, 1)->setText(QString::number(4.992));
		}
		else
		{
			ui.tableWidget_DianYa->item(3, 1)->setText(QString::number(0.924));
		}

		it = mapSendDianYaCmd.find(QStringLiteral("���ݼӵ�2"));
		if (it != mapSendDianYaCmd.end())
		{
			ui.tableWidget_DianYa->item(4, 1)->setText(QString::number(4.992));
			ui.tableWidget_DianYa->item(5, 1)->setText(QString::number(4.998));
		}
		else
		{
			ui.tableWidget_DianYa->item(4, 1)->setText(QString::number(0.924));
			ui.tableWidget_DianYa->item(5, 1)->setText(QString::number(0.924));
		}
	}
}

void CDlgComm::updateYaoceTableData()
{

	map<QString, YaoCeData>::iterator itYaoceParam = curYaoCeData->find(strCurShowType);
	if (itYaoceParam == curYaoCeData->end())
	{
		return;
	}
	map<QString, BYTE*>::iterator itData = mapTypeToData.find(strCurShowType);
	if (itData == mapTypeToData.end())
	{
		return;
	}
	int iIndexRow = 0;
	for (int i = 0; i < ui.tableWidget_Telemetry->rowCount(); i++)
	{
		QString strType = ui.tableWidget_Telemetry->item(i, 1)->text();
		if (strType.contains(strCurShowType))
		{
			iIndexRow = i;
			break;
		}
	}

	for (vector<YaoCe>::iterator itParam = itYaoceParam->second.vecYaoCeParams.begin(); itParam != itYaoceParam->second.vecYaoCeParams.end(); itParam++)
	{
		BYTE* dataStart = itData->second + itParam->param_StartBit;
		QString strShow = "";
		for (int i = 0; i < itParam->param_AnalysisBits; i++)
		{
			strShow += QString("%1").arg(*(dataStart + i), 2, 16, QLatin1Char('0'));
		}
		ui.tableWidget_Telemetry->item(iIndexRow, 2)->setText(strShow);
		iIndexRow++;
	}
}

void CDlgComm::updateYaoceTableData_2()
{

	map<QString, YaoCeData>::iterator itYaoceParam = curYaoCeData->find(strCurShowType);
	if (itYaoceParam == curYaoCeData->end())
	{
		return;
	}
	map<QString, BYTE*>::iterator itData = mapTypeToData.find(strCurShowType);
	if (itData == mapTypeToData.end())
	{
		return;
	}
	int iIndexRow = 0;
	for (int i = 0; i < ui.tableWidget_Telemetry_2->rowCount(); i++)
	{
		QString strType = ui.tableWidget_Telemetry_2->item(i, 1)->text();
		if (strType.contains(strCurShowType))
		{
			iIndexRow = i;
			break;
		}
	}

	for (vector<YaoCe>::iterator itParam = itYaoceParam->second.vecYaoCeParams.begin(); itParam != itYaoceParam->second.vecYaoCeParams.end(); itParam++)
	{
		BYTE* dataStart = itData->second + itParam->param_StartBit;
		QString strShow = "";
		for (int i = 0; i < itParam->param_AnalysisBits; i++)
		{
			strShow += QString("%1").arg(*(dataStart + i), 2, 16, QLatin1Char('0'));
		}
		ui.tableWidget_Telemetry_2->item(iIndexRow, 2)->setText(strShow);
		iIndexRow++;
	}
}


void CDlgComm::changeYaoceTable(QString strType)
{
	for (int i = 0; i < ui.tableWidget_Telemetry->rowCount(); i++)
	{
		QTableWidgetItem* item = ui.tableWidget_Telemetry->item(i, 1);
		if (item->text().contains(strType))
		{
			ui.tableWidget_Telemetry->setRowHidden(i, false);
		}
		else
		{
			ui.tableWidget_Telemetry->setRowHidden(i, true);
		}
	}
	for (map<QString, YaoCeData>::iterator itYaoce = curYaoCeData->begin(); itYaoce != curYaoCeData->end(); itYaoce++)
	{
		if (itYaoce->first.contains(strType))
		{
			strCurShowType = itYaoce->first;
		}
	}
}

CDlgComm::~CDlgComm()
{
	stop_flag = true;

	if (cmd_log != nullptr&&cmd_log->isOpen()) {
		cmd_log->close();
	}
}

void CDlgComm::initListWidget()
{

}

void CDlgComm::initTableWidget()
{
	QTableWidget* table = ui.tableWidget_cmd;

	//���
	if (ykyc["cmd_name_custom"] != NULL && ykyc["cmd_name_custom"] == true) {
		table->setColumnCount(3);
		QStringList list_horheader;
		list_horheader.append(QStringLiteral("����"));
		list_horheader.append(QStringLiteral("ָ��"));
		list_horheader.append(QStringLiteral("�Զ����ͼ����"));
		table->setHorizontalHeaderLabels(list_horheader);
	}
	else {
		table->setColumnCount(2);
		QStringList list_horheader;
		list_horheader.append(QStringLiteral("����"));
		list_horheader.append(QStringLiteral("ָ��"));
		table->setHorizontalHeaderLabels(list_horheader);

		ui.pushButton_cmd_name_custom->hide();
		ui.radioButton_cmd_name_custom->hide();
	}

	//���û����עң��
	if (ykyc["shang_zhu_yao_ce"]["hide"] == true) {
		ui.horizontalLayout->removeItem(ui.verticalLayout_8);
	}

	table->setEditTriggers(QAbstractItemView::NoEditTriggers);//���ò��ɱ༭
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//����Ӧ�п�
	table->verticalHeader()->setVisible(false); //���ô�ֱͷ���ɼ�

	//�м��
	QStringList strLineTestList;
	strLineTestList << QStringLiteral("�����Ƿ��д�") << QStringLiteral("���ݴ����к�") << QStringLiteral("�����Ƿ��д�") << QStringLiteral("���ݴ����к�");
	for (size_t i = 0; i < strLineTestList.size(); i++)
	{
		int iCount = ui.tableWidget_lineTest->rowCount();
		ui.tableWidget_lineTest->insertRow(iCount);
		QTableWidgetItem* item1 = new QTableWidgetItem(strLineTestList[i]);
		QTableWidgetItem* item2 = new QTableWidgetItem(QString("00"));
		item1->setTextAlignment(Qt::AlignCenter);
		item2->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget_lineTest->setItem(iCount, 0, item1);
		ui.tableWidget_lineTest->setItem(iCount, 1, item2);
	}

	//��ѹң��
	strRowList << QStringLiteral("����+6VDֱ��ң��") << QStringLiteral("����+6VAֱ��ң��") << QStringLiteral("����-6VAֱ��ң��")
		<< QStringLiteral("����+6VDֱ��ң��") << QStringLiteral("����+6VAֱ��ң��") << QStringLiteral("����-6VAֱ��ң��");
	for (int i = 0; i < strRowList.size(); i++)
	{
		int iCount = ui.tableWidget_DianYa->rowCount();
		ui.tableWidget_DianYa->insertRow(iCount);
		QTableWidgetItem* item1 = new QTableWidgetItem(strRowList[i]);
		QTableWidgetItem* item2 = new QTableWidgetItem(QString(""));
		item1->setTextAlignment(Qt::AlignCenter);
		item2->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget_DianYa->setItem(iCount, 0, item1);
		ui.tableWidget_DianYa->setItem(iCount, 1, item2);
	}

	//ң��
	ui.tableWidget_Telemetry->setColumnCount(3); //��������
	ui.tableWidget_Telemetry_2->setColumnCount(3); //��������
	//table_widget->horizontalHeader()->setDefaultSectionSize(150);
	//table_widget->horizontalHeader()->setClickable(false); //���ñ�ͷ���ɵ����Ĭ�ϵ�����������
	//���ñ�ͷ����
	QStringList header;
	header << QStringLiteral("ң������") << QStringLiteral("��������") << QStringLiteral("��ֵ");
	ui.tableWidget_Telemetry->setHorizontalHeaderLabels(header);
	ui.tableWidget_Telemetry_2->setHorizontalHeaderLabels(header);

	ui.tableWidget_Telemetry->hideColumn(1);//���صڶ���
	ui.tableWidget_Telemetry_2->hideColumn(1);//���صڶ���


	//���ñ�ͷ����Ӵ�
	QFont font = ui.tableWidget_Telemetry->horizontalHeader()->font();
	font.setBold(true);
	ui.tableWidget_Telemetry->horizontalHeader()->setFont(font);

	ui.tableWidget_Telemetry->horizontalHeader()->setStretchLastSection(true); //���ó�������
	//ui.tableWidget_Telemetry->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//����Ӧ�п�
	//ui.tableWidget_Telemetry->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);//����Ӧ�и�
	//table_widget->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	//table_widget->verticalHeader()->setDefaultSectionSize(20); //�����и�
	ui.tableWidget_Telemetry->setFrameShape(QFrame::NoFrame); //�����ޱ߿�
	//table_widget->setShowGrid(false); //���ò���ʾ������
	ui.tableWidget_Telemetry->verticalHeader()->setVisible(false); //���ô�ֱͷ���ɼ�
	ui.tableWidget_Telemetry->setSelectionMode(QAbstractItemView::ExtendedSelection);  //�ɶ�ѡ��Ctrl��Shift��  Ctrl+A���ܹ���
	ui.tableWidget_Telemetry->setSelectionBehavior(QAbstractItemView::SelectRows);  //����ѡ����Ϊʱÿ��ѡ��һ��
	//ui.tableWidget_Telemetry->setEditTriggers(QAbstractItemView::NoEditTriggers); //���ò��ɱ༭

		//���ñ�ͷ����Ӵ�
	//QFont font = ui.tableWidget_Telemetry_2->horizontalHeader()->font();
	font.setBold(true);
	ui.tableWidget_Telemetry_2->horizontalHeader()->setFont(font);

	ui.tableWidget_Telemetry_2->horizontalHeader()->setStretchLastSection(true); //���ó�������
	//ui.tableWidget_Telemetry_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//����Ӧ�п�
	//ui.tableWidget_Telemetry_2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);//����Ӧ�и�
	//table_widget->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	//table_widget->verticalHeader()->setDefaultSectionSize(20); //�����и�
	ui.tableWidget_Telemetry_2->setFrameShape(QFrame::NoFrame); //�����ޱ߿�
	//table_widget->setShowGrid(false); //���ò���ʾ������
	ui.tableWidget_Telemetry_2->verticalHeader()->setVisible(false); //���ô�ֱͷ���ɼ�
	ui.tableWidget_Telemetry_2->setSelectionMode(QAbstractItemView::ExtendedSelection);  //�ɶ�ѡ��Ctrl��Shift��  Ctrl+A���ܹ���
	ui.tableWidget_Telemetry_2->setSelectionBehavior(QAbstractItemView::SelectRows);  //����ѡ����Ϊʱÿ��ѡ��һ��
	ui.tableWidget_Telemetry_2->setEditTriggers(QAbstractItemView::NoEditTriggers); //���ò��ɱ༭

	//ui.tableWidget_Telemetry->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	//ui.tableWidget_Telemetry_2->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

	ui.tableWidget_Telemetry->setColumnWidth(0, 300);
	ui.tableWidget_Telemetry_2->setColumnWidth(0, 300);

	for (map<QString, YaoCeData>::iterator itYaoce = curYaoCeData->begin(); itYaoce != curYaoCeData->end(); itYaoce++)
	{
		BYTE* dataTemp = new BYTE[itYaoce->second.iDataLength];
		memset(dataTemp, 0x00, itYaoce->second.iDataLength);
		mapTypeToData.insert(pair<QString, BYTE*>(itYaoce->first, dataTemp));

		for (int i = 0; i < itYaoce->second.vecYaoCeParams.size(); i++)
		{
			int row_count;
			QTableWidgetItem *item1 = new QTableWidgetItem(itYaoce->second.vecYaoCeParams[i].param_Name);
			QTableWidgetItem *item2 = new QTableWidgetItem(itYaoce->first);
			QTableWidgetItem *item3 = new QTableWidgetItem("00");

			if (itYaoce->first == QStringLiteral("�ڲ���������ң��"))
			{
				row_count = ui.tableWidget_Telemetry->rowCount(); //��ȡ������
				ui.tableWidget_Telemetry->insertRow(row_count); //��������

				ui.tableWidget_Telemetry->setItem(row_count, 0, item1);
				ui.tableWidget_Telemetry->setItem(row_count, 1, item2);
				ui.tableWidget_Telemetry->setItem(row_count, 2, item3);

			}
			else if (itYaoce->first == QStringLiteral("��ע״̬ң��"))
			{
				row_count = ui.tableWidget_Telemetry_2->rowCount(); //��ȡ������
				ui.tableWidget_Telemetry_2->insertRow(row_count); //��������

				ui.tableWidget_Telemetry_2->setItem(row_count, 0, item1);
				ui.tableWidget_Telemetry_2->setItem(row_count, 1, item2);
				ui.tableWidget_Telemetry_2->setItem(row_count, 2, item3);
			}


			//item1->setTextAlignment(Qt::AlignCenter);
			//item2->setTextAlignment(Qt::AlignCenter);
			//item3->setTextAlignment(Qt::AlignCenter);

			//if (itYaoce->first != QStringLiteral("�ڲ���������ң��"))
			//{
			//	if (itYaoce->first != QStringLiteral("��ע״̬ң��"))
			//	{
			//		ui.tableWidget_Telemetry->setRowHidden(row_count, true);
			//	}
			//}
			//else
			//{
			//	strCurShowType = itYaoce->first;
			//}
		}
	}
}

void CDlgComm::initWindow()
{
	//��ʾ�������
	QHBoxLayout* hBoxInWidgetCam = new QHBoxLayout(ui.widget_CameraType);

	for (vector<oneWndData>::iterator itCamera = curScreen->oneScreenParams.begin(); itCamera != curScreen->oneScreenParams.end(); itCamera++)//һ����Ļ�ڵĴ�������
	{
		QRadioButton* radio = new QRadioButton(itCamera->WINDOW_Name);
		//radio->setStyleSheet("color:rgb(255,255,255)");
		connect(radio, SIGNAL(clicked()), this, SLOT(on_cameraRadio_clicked()));
		hBoxInWidgetCam->addWidget(radio);
		if (itCamera == curScreen->oneScreenParams.begin())
		{
			radio->setChecked(true);
			radio->clicked(true);
		}
	}
	ui.widget_CameraType->setLayout(hBoxInWidgetCam);
	ui.widget_CameraType->hide();

	//��ʾָ��ģ�鲿��
	QSize laySize = curYaoKongLay->layoutYK;
	ui.widget->setContentsMargins(0, 0, 0, 0);
	QVBoxLayout* vBoxInWidget = new QVBoxLayout(ui.widget);
	vBoxInWidget->setContentsMargins(0, 0, 0, 0);
	vector<QHBoxLayout*> vecHbox;
	for (int i = 0; i < laySize.height(); i++)
	{
		QHBoxLayout* hBox = new QHBoxLayout;
		hBox->setContentsMargins(0, 0, 0, 0);
		vecHbox.push_back(hBox);
	}
	int iNum = 0;
	for (vector<OneGroup>::iterator itGroup = curYaoKongLay->groups.begin(); itGroup != curYaoKongLay->groups.end(); itGroup++)
	{
		vector<QObject*> vecTemp;//��ŵ�ǰָ����������ÿؼ�

		QGroupBox* groupTemp = new QGroupBox;
		groupTemp->setContentsMargins(0, 0, 0, 0);
		QStringList strNameType = itGroup->Param_GroupName.split(":");
		groupTemp->setTitle(strNameType[0]);//��ǰָ������
		QVBoxLayout* vBoxInGroup = new QVBoxLayout(groupTemp);
		vBoxInGroup->setContentsMargins(20, 20, 20, 20);
		vBoxInGroup->setSpacing(8);

		//Comboxƽ��
		if (strNameType[1] == "52") {
			QGridLayout* flat_layout = new QGridLayout;
			int row_count = 0;
			int col_counter = 0;

			for (vector<std::pair<QString, QString>>::iterator itSub = itGroup->vecTypeToName.begin(); itSub != itGroup->vecTypeToName.end(); itSub++)
			{
				if (itSub->first == QStringLiteral("Param_LineEdit"))
				{
					QStringList strName_Bits = itSub->second.split("_");
					QLabel* label = new QLabel(strName_Bits[0]);
					flat_layout->addWidget(label, 0, col_counter);

					for (int i = 1; i < row_count; i++) {
						QLineEdit* lineEdit = new QLineEdit;
						QStringList strStartValid = strName_Bits[1].split(",");
						User *pUser = new User();
						pUser->iStartBit = strStartValid[0].toInt();    //��ָ���е���ʼλ
						pUser->iValidBits = strStartValid[1].toInt();   //��ָ���е���Чλ
						lineEdit->setUserData(Qt::UserRole, pUser);

						flat_layout->addWidget(lineEdit, i, col_counter);
					}

					//vecTemp.push_back(lineEdit);
					col_counter++;
				}
				else if (itSub->first == QStringLiteral("Param_Combox"))
				{
					//���ͷ
					QStringList strListTemp = itSub->second.split(":");
					QLabel* label = new QLabel(strListTemp[0]);
					flat_layout->addWidget(label, 0, col_counter);
					//�õ�����
					QStringList strName_Bits = strListTemp[1].split("_");
					strListTemp = strName_Bits[0].split(",");
					row_count = strListTemp.count() + 1;
					//
					for (int i = 1; i < row_count; i++) {
						QLabel* label = new QLabel(strListTemp[i - 1]);

						QStringList strStartValid = strName_Bits[1].split(",");
						User *pUser = new User();
						pUser->iStartBit = strStartValid[0].toInt();    //��ָ���е���ʼλ
						pUser->iValidBits = strStartValid[1].toInt();   //��ָ���е���Чλ
						label->setUserData(Qt::UserRole, pUser);

						flat_layout->addWidget(label, i, col_counter);
					}

					//vecTemp.push_back(combox);
					col_counter++;
				}
				else if (itSub->first == QStringLiteral("Param_Label"))
				{
					//���ͷ
					QStringList strListTemp = itSub->second.split(":");
					QLabel* label = new QLabel(strListTemp[0]);
					flat_layout->addWidget(label, 0, col_counter);

					for (int i = 1; i < row_count; i++) {
						QLabel* labelName = new QLabel(strListTemp[1]);
						flat_layout->addWidget(labelName, i, col_counter);
					}
					col_counter++;
				}
				else if (itSub->first == QStringLiteral("Param_SendButton"))
				{
					QStringList strListTemp = itSub->second.split(":");//���ͣ�ID

					for (int i = 1; i < row_count; i++) {
						QPushButton* btn = new QPushButton(strListTemp[0]);
						connect(btn, SIGNAL(clicked()), this, SLOT(on_allSendBtn_clicked()));
						User *pUser = new User();
						pUser->nID = strListTemp[1].toInt(nullptr, 16);    //w1
						pUser->iFlag = strNameType[1].toInt();  //B0-B3����
						pUser->strName = strNameType[0] + qobject_cast<QLabel*>(flat_layout->itemAtPosition(i, 0)->widget())->text();        //��ǰָ�����ƣ���������ָ���mapGroupToSubs�ҵ������Ŀؼ�
						btn->setUserData(Qt::UserRole, pUser);

						flat_layout->addWidget(btn, i, col_counter);
					}

				}
			}
			for (int i = 1; i < row_count; i++) {
				vector<QObject*> vecTemp;//��ŵ�ǰָ����������ÿؼ�
				for (int j = 0; j < col_counter; j++) {
					vecTemp.push_back(flat_layout->itemAtPosition(i, j)->widget());
				}
				mapGroupToSubs.insert(std::pair<QString, vector<QObject*>>(strNameType[0] + qobject_cast<QLabel*>(flat_layout->itemAtPosition(i, 0)->widget())->text(), vecTemp));

			}


			////��񲼾֣���ͷ
			//int col_count = 0;
			//for (vector<std::pair<QString, QString>>::iterator itSub = itGroup->vecTypeToName.begin(); itSub != itGroup->vecTypeToName.end(); itSub++) {
			//	if (itSub->first == QStringLiteral("Param_SendButton")) {
			//		continue;
			//	}
			//	flat_layout->addWidget(new QLabel(itSub->second.split(":")[0]),0, col_count);
			//	col_count++;
			//}

			////����ֻ����Combox�ڵ�һ�е����
			//auto itSub = itGroup->vecTypeToName.begin();
			//if (itSub->first == QStringLiteral("Param_Combox")) {

			//	QStringList strName_Bits = itSub->second.split(":")[1].split("_");
			//	QStringList strListTemp = strName_Bits[0].split(",");
			//}

			vBoxInGroup->addLayout(flat_layout);
		}
		//������ʾ
		else {
			for (vector<std::pair<QString, QString>>::iterator itSub = itGroup->vecTypeToName.begin(); itSub != itGroup->vecTypeToName.end(); itSub++)
			{
				QHBoxLayout* hBoxSub = new QHBoxLayout;
				if (itSub->first == QStringLiteral("Param_LineEdit"))
				{
					QStringList strName_Bits = itSub->second.split("_");
					QLabel* label = new QLabel(strName_Bits[0]);
					QLineEdit* lineEdit = new QLineEdit;

					QStringList strStartValid = strName_Bits[1].split(",");
					User *pUser = new User();
					pUser->iStartBit = strStartValid[0].toInt();    //��ָ���е���ʼλ
					pUser->iValidBits = strStartValid[1].toInt();   //��ָ���е���Чλ
					if (strStartValid.length() >= 3) {
						pUser->base = strStartValid[2].toInt();
						if (pUser->base == 10) {
							lineEdit->setText("12500");
						}
					}
					else {
						pUser->base = 16;
					}
					lineEdit->setUserData(Qt::UserRole, pUser);

					hBoxSub->addWidget(label);
					hBoxSub->addWidget(lineEdit);
					vecTemp.push_back(lineEdit);
				}
				else if (itSub->first == QStringLiteral("Param_Combox"))
				{
					QStringList strListTemp = itSub->second.split(":");
					QLabel* label = new QLabel(strListTemp[0]);

					QStringList strName_Bits = strListTemp[1].split("_");

					strListTemp = strName_Bits[0].split(",");
					QComboBox* combox = new QComboBox;
					combox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
					combox->addItems(strListTemp);

					QStringList strStartValid = strName_Bits[1].split(",");
					User *pUser = new User();
					pUser->iStartBit = strStartValid[0].toInt();    //��ָ���е���ʼλ
					pUser->iValidBits = strStartValid[1].toInt();   //��ָ���е���Чλ
					combox->setUserData(Qt::UserRole, pUser);

					hBoxSub->addWidget(label);
					hBoxSub->addWidget(combox);
					vecTemp.push_back(combox);
				}
				else if (itSub->first == QStringLiteral("Param_Label"))
				{
					QLabel* labelName = new QLabel(itSub->second);
					vecTemp.push_back(labelName);
					hBoxSub->addWidget(labelName);
				}
				else if (itSub->first == QStringLiteral("Param_SendButton"))
				{
					hBoxSub->addStretch();
					QStringList strListTemp = itSub->second.split(":");//���ͣ�ID
					QPushButton* btn = new QPushButton(strListTemp[0]);
					connect(btn, SIGNAL(clicked()), this, SLOT(on_allSendBtn_clicked()));
					User *pUser = new User();
					pUser->nID = strListTemp[1].toInt(nullptr, 16);    //w1
					pUser->iFlag = strNameType[1].toInt();  //B0-B3����
					pUser->strName = strNameType[0];        //��ǰָ�����ƣ���������ָ���mapGroupToSubs�ҵ������Ŀؼ�
					btn->setUserData(Qt::UserRole, pUser);
					//btn->setFixedSize(70, 20);
					hBoxSub->addWidget(btn);
				}

				vBoxInGroup->addLayout(hBoxSub);
			}
			groupTemp->setLayout(vBoxInGroup);
			mapGroupToSubs.insert(std::pair<QString, vector<QObject*>>(strNameType[0], vecTemp));
		}


		int index = iNum / laySize.width();
		vecHbox[index]->addWidget(groupTemp);
		iNum++;
	}
	for (int i = 0; i < vecHbox.size(); i++)
	{
		vBoxInWidget->addLayout(vecHbox[i]);
	}
	vBoxInWidget->addStretch();
	ui.widget->setLayout(vBoxInWidget);

}

void CDlgComm::on_closeButton_clicked()
{
	this->close();
}

void CDlgComm::on_minButton_clicked()
{
	this->showMinimized();
}

//���еķ��Ͱ�ť�ӿ�
void CDlgComm::on_allSendBtn_clicked()
{
	memset(byteCmdBuf, 0x00, CmdLength);

	QPushButton* btn = qobject_cast<QPushButton*>(sender());
	User* pUser = (User*)(btn->userData(Qt::UserRole));
	//ң��ָ��
	if (pUser->iFlag == 0)
	{

		//byteCmdBuf[0] = 0x51;
		//byteCmdBuf[1] = 0x08;
		//byteCmdBuf[2] = 0x20;
		//byteCmdBuf[3] = 0x11;

		std::string header = ykyc["yao_kong"]["cmd_header"];
		hexstr2byte_array(header.c_str(), byteCmdBuf, header.length() / 2);
		QLabel* label;
		//D5-D9
		unsigned int resultCmd = 0;
		map<QString, vector<QObject*>>::iterator itFind = mapGroupToSubs.find(pUser->strName);//�ҵ���ǰָ���Ӧ�����ÿؼ�
		if (itFind != mapGroupToSubs.end())
		{
			for (vector<QObject*>::iterator itSub = itFind->second.begin(); itSub != itFind->second.end(); itSub++)
			{
				QLineEdit* lineEdit = qobject_cast<QLineEdit*>(*itSub);
				QComboBox* comBox = qobject_cast<QComboBox*>(*itSub);
				label = qobject_cast<QLabel*>(*itSub);
				if (lineEdit)
				{
					User* pUserLine = (User*)(lineEdit->userData(Qt::UserRole));
					int iData = lineEdit->text().toInt(NULL, 16);
					iData = iData % ((int)pow(2, pUserLine->iValidBits));//��Ч����
					iData = iData << pUserLine->iStartBit;//ƫ��
					resultCmd += iData;
				}
				else if (comBox)
				{
					map<QString, int>::iterator itPeriod = curYaoKongLay->mapComboxParams.find(comBox->currentText());
					if (itPeriod != curYaoKongLay->mapComboxParams.end())
					{
						User* pUserCombox = (User*)(comBox->userData(Qt::UserRole));
						int iData = itPeriod->second;
						iData = iData % ((int)pow(2, pUserCombox->iValidBits));//��Ч����
						iData = iData << pUserCombox->iStartBit;//ƫ��
						resultCmd += iData;
					}
				}
			}
		}
		//D4
		byteCmdBuf[4] = pUser->nID;
		byteCmdBuf[5] = (resultCmd / 256 / 256 / 256) % 256;
		byteCmdBuf[6] = (resultCmd / 256 / 256) % 256;
		byteCmdBuf[7] = (resultCmd / 256) % 256;
		byteCmdBuf[8] = resultCmd % 256;
		if (label) {
			QString label_str = label->text();
			int start = label_str.mid(1, 1).toInt();
			int end = label_str.mid(4, 1).toInt();
			int i = 0;
			for (int index = start; index <= end; index++) {
				byteCmdBuf[index + 3] = label_str.mid(6 + i, 2).toInt(nullptr, 16);
				i++;
			}
		}

		DataBlockBroadcast broadcast;
		for (int i = 2; i < 9; i++)
		{
			broadcast.Sum(byteCmdBuf[i]);
		}
		byteCmdBuf[9] = broadcast.SUM;//��У��
	}
	//ң��ָ��
	if (pUser->iFlag == 10)
	{
		std::string header = ykyc["yao_kong"]["cmd_header_2"];
		hexstr2byte_array(header.c_str(), byteCmdBuf, header.length() / 2);
		QLabel* label;
		//D5-D9
		unsigned int resultCmd = 0;
		map<QString, vector<QObject*>>::iterator itFind = mapGroupToSubs.find(pUser->strName);//�ҵ���ǰָ���Ӧ�����ÿؼ�
		if (itFind != mapGroupToSubs.end())
		{
			for (vector<QObject*>::iterator itSub = itFind->second.begin(); itSub != itFind->second.end(); itSub++)
			{
				QLineEdit* lineEdit = qobject_cast<QLineEdit*>(*itSub);
				QComboBox* comBox = qobject_cast<QComboBox*>(*itSub);
				label = qobject_cast<QLabel*>(*itSub);
				if (lineEdit)
				{
					User* pUserLine = (User*)(lineEdit->userData(Qt::UserRole));
					int iData = lineEdit->text().toInt(NULL, 16);
					iData = iData % ((int)pow(2, pUserLine->iValidBits));//��Ч����
					iData = iData << pUserLine->iStartBit;//ƫ��
					resultCmd += iData;
				}
				else if (comBox)
				{
					map<QString, int>::iterator itPeriod = curYaoKongLay->mapComboxParams.find(comBox->currentText());
					if (itPeriod != curYaoKongLay->mapComboxParams.end())
					{
						User* pUserCombox = (User*)(comBox->userData(Qt::UserRole));
						int iData = itPeriod->second;
						iData = iData % ((int)pow(2, pUserCombox->iValidBits));//��Ч����
						iData = iData << pUserCombox->iStartBit;//ƫ��
						resultCmd += iData;
					}
				}
			}
		}
		//D4
		byteCmdBuf[4] = pUser->nID;
		byteCmdBuf[5] = (resultCmd / 256 / 256 / 256) % 256;
		byteCmdBuf[6] = (resultCmd / 256 / 256) % 256;
		byteCmdBuf[7] = (resultCmd / 256) % 256;
		byteCmdBuf[8] = resultCmd % 256;
		if (label) {
			QString label_str = label->text();
			int start = label_str.mid(1, 1).toInt();
			int end = label_str.mid(4, 1).toInt();
			int i = 0;
			for (int index = start; index <= end; index++) {
				byteCmdBuf[index + 3] = label_str.mid(6 + i, 2).toInt(nullptr, 16);
				i++;
			}
		}

		DataBlockBroadcast broadcast;
		for (int i = 2; i < 9; i++)
		{
			broadcast.Sum(byteCmdBuf[i]);
		}
		byteCmdBuf[9] = broadcast.SUM;//��У��
	}
	else if (pUser->iFlag == 1)
	{
		byteCmdBuf[0] = 0x3f;
		byteCmdBuf[1] = 0x00;
		byteCmdBuf[2] = 0x00;
		byteCmdBuf[3] = 0x00;
		byteCmdBuf[5] = 0x00;
		byteCmdBuf[6] = 0x00;
		byteCmdBuf[7] = 0x00;
		unsigned int resultCmd = 0;
		int iFlag = 0;//0:���ݣ�1:����

		map<QString, vector<QObject*>>::iterator itFind = mapGroupToSubs.find(pUser->strName);//�ҵ���ǰָ���Ӧ�����ÿؼ�
		if (itFind != mapGroupToSubs.end())
		{
			for (vector<QObject*>::iterator itSub = itFind->second.begin(); itSub != itFind->second.end(); itSub++)
			{
				QLineEdit* lineEdit = qobject_cast<QLineEdit*>(*itSub);
				QComboBox* comBox = qobject_cast<QComboBox*>(*itSub);
				if (lineEdit)
				{
					User* pUserLine = (User*)(lineEdit->userData(Qt::UserRole));
					int iData = lineEdit->text().toInt(NULL, 16);
					iData = iData % ((int)pow(2, pUserLine->iValidBits));//��Ч����
					iData = iData << pUserLine->iStartBit;//ƫ��
					resultCmd += iData;
				}
				else if (comBox)
				{
					if (comBox)
					{
						/*
						QString strName = comBox->currentText();
						if (strName.contains(QStringLiteral("���ݶϵ�")))
						{
							for (map<QString,QString>::iterator it = mapSendDianYaCmd.begin();it != mapSendDianYaCmd.end();)
							{
								if (it->first.contains(QStringLiteral("����")))
								{
									it = mapSendDianYaCmd.erase(it);
								}
								else
								{
									it++;
								}
							}
							recvSerialData = false;
						}
						else if (strName.contains(QStringLiteral("���ݶϵ�")))
						{
							for (map<QString, QString>::iterator it = mapSendDianYaCmd.begin(); it != mapSendDianYaCmd.end();)
							{
								if (it->first.contains(QStringLiteral("����")))
								{
									it = mapSendDianYaCmd.erase(it);
								}
								else
								{
									it++;
								}
							}
							recvSerialData = false;
						}
						else
						{
							mapSendDianYaCmd.insert(std::pair<QString, QString>(strName, strName));
						}

						if (strName.contains(QStringLiteral("��")))
						{
							iFlag = 1;
						}
						*/
						map<QString, int>::iterator itPeriod = curYaoKongLay->mapComboxParams.find(comBox->currentText());
						if (itPeriod != curYaoKongLay->mapComboxParams.end())
						{
							User* pUserCombox = (User*)(comBox->userData(Qt::UserRole));
							int iData = itPeriod->second;
							iData = iData % ((int)pow(2, pUserCombox->iValidBits));//��Ч����
							iData = iData << pUserCombox->iStartBit;//ƫ��
							resultCmd += iData;
						}
					}
				}
			}
		}
		byteCmdBuf[5] = 0x00;
		if (iFlag == 1)
		{
			byteCmdBuf[6] = 0x01;
			byteCmdBuf[7] = resultCmd;
		}
		else
		{
			byteCmdBuf[7] = resultCmd;
		}
	}
	else if (pUser->iFlag == 2)
	{
		if (btn->text().contains(QStringLiteral("����")))
		{
			btn->setText(QStringLiteral("ֹͣ"));
			bSend = true;
			iMulSendFlag = 0;
			m_timer->start(2000);
		}
		else
		{
			btn->setText(QStringLiteral("����"));
			bSend = false;
			m_timer->stop();
		}
		return;
	}
	else if (pUser->iFlag == 4)
	{
		unsigned int resultCmd = 0;
		map<QString, vector<QObject*>>::iterator itFind = mapGroupToSubs.find(pUser->strName);//�ҵ���ǰָ���Ӧ�����ÿؼ�
		if (itFind != mapGroupToSubs.end())
		{
			for (vector<QObject*>::iterator itSub = itFind->second.begin(); itSub != itFind->second.end(); itSub++)
			{
				QLineEdit* lineEdit = qobject_cast<QLineEdit*>(*itSub);
				QComboBox* comBox = qobject_cast<QComboBox*>(*itSub);
				if (lineEdit)
				{
					User* pUserLine = (User*)(lineEdit->userData(Qt::UserRole));
					int iData = lineEdit->text().toInt(NULL, 16);
					iData = iData % ((int)pow(2, pUserLine->iValidBits));//��Ч����
					iData = iData << pUserLine->iStartBit;//ƫ��
					resultCmd += iData;
				}
				else if (comBox)
				{
					if (comBox)
					{
						map<QString, int>::iterator itPeriod = curYaoKongLay->mapComboxParams.find(comBox->currentText());
						if (itPeriod != curYaoKongLay->mapComboxParams.end())
						{
							User* pUserCombox = (User*)(comBox->userData(Qt::UserRole));
							int iData = itPeriod->second;
							iData = iData % ((int)pow(2, pUserCombox->iValidBits));//��Ч����
							iData = iData << pUserCombox->iStartBit;//ƫ��
							resultCmd += iData;
						}
					}
				}
			}
		}
		byteCmdBuf[0] = 0X35;
		byteCmdBuf[4] = resultCmd;
	}
	else if (pUser->iFlag == 5)
	{
		//D0-D3
		byteCmdBuf[0] = 0x30;
		byteCmdBuf[1] = 00;
		byteCmdBuf[2] = pUser->nID;
		//D5-D9
		unsigned int resultCmd = 0;
		map<QString, vector<QObject*>>::iterator itFind = mapGroupToSubs.find(pUser->strName);//�ҵ���ǰָ���Ӧ�����ÿؼ�
		if (itFind != mapGroupToSubs.end())
		{
			for (vector<QObject*>::iterator itSub = itFind->second.begin(); itSub != itFind->second.end(); itSub++)
			{
				QLineEdit* lineEdit = qobject_cast<QLineEdit*>(*itSub);
				QComboBox* comBox = qobject_cast<QComboBox*>(*itSub);
				if (lineEdit)
				{
					User* pUserLine = (User*)(lineEdit->userData(Qt::UserRole));
					int iData = lineEdit->text().toInt(NULL, 16);
					iData = iData % ((int)pow(2, pUserLine->iValidBits));//��Ч����
					iData = iData << pUserLine->iStartBit;//ƫ��
					resultCmd += iData;
				}
				else if (comBox)
				{
					if (comBox)
					{
						map<QString, int>::iterator itPeriod = curYaoKongLay->mapComboxParams.find(comBox->currentText());
						if (itPeriod != curYaoKongLay->mapComboxParams.end())
						{
							User* pUserCombox = (User*)(comBox->userData(Qt::UserRole));
							int iData = itPeriod->second;
							iData = iData % ((int)pow(2, pUserCombox->iValidBits));//��Ч����
							iData = iData << pUserCombox->iStartBit;//ƫ��
							resultCmd += iData;
						}
						changeYaoceTable(comBox->currentText().left(2));
						updateYaoceTableData();
					}
				}
			}
		}
		if (resultCmd == 1)
		{
			byteCmdBuf[3] = 1;
		}
		else if (resultCmd == 2)
		{
			byteCmdBuf[4] = 1;
		}
		else if (resultCmd == 3)
		{
			byteCmdBuf[5] = 1;
		}
		else if (resultCmd == 4)
		{
			byteCmdBuf[3] = 0;
		}
		else if (resultCmd == 5)
		{
			byteCmdBuf[4] = 0;
		}
		else if (resultCmd == 6)
		{
			byteCmdBuf[5] = 0;
		}
	}
	else if (pUser->iFlag == 6)
	{
		byteCmdBuf[0] = 0x20;

		if (btn->text().contains(QStringLiteral("����")))
		{
			btn->setText(QStringLiteral("ֹͣ"));
			//bSend = true;
			iMulSendFlag = 1;
			m_timer->start(2000);
		}
		else
		{
			btn->setText(QStringLiteral("����"));
			//bSend = false;
			m_timer->stop();
		}
		return;
	}
	//����ָ��
	else if (pUser->iFlag == 50)
	{
		unsigned int resultCmd = 0;
		map<QString, vector<QObject*>>::iterator itFind = mapGroupToSubs.find(pUser->strName);//�ҵ���ǰָ���Ӧ�����ÿؼ�
		if (itFind != mapGroupToSubs.end())
		{
			for (vector<QObject*>::iterator itSub = itFind->second.begin(); itSub != itFind->second.end(); itSub++)
			{
				QLineEdit* lineEdit = qobject_cast<QLineEdit*>(*itSub);
				QComboBox* comBox = qobject_cast<QComboBox*>(*itSub);
				if (lineEdit)
				{
					User* pUserLine = (User*)(lineEdit->userData(Qt::UserRole));
					QString cmd_str = lineEdit->text();
					cmd_str = cmd_str.replace(" ", "");
					hexstr2byte_array(cmd_str.toStdString().c_str(), byteCmdBuf, 9);
				}
			}
		}

		DataBlockBroadcast broadcast;
		for (int i = 0; i < 9; i++)
		{
			broadcast.Sum(byteCmdBuf[i]);
		}
		byteCmdBuf[9] = broadcast.SUM;//��У��
	}
	//��עָ��
	else if (pUser->iFlag == 51)
	{
		std::string header = ykyc["shang_zhu"]["cmd_header"];
		hexstr2byte_array(header.c_str(), byteCmdBuf, header.length() / 2);

		//D5-D9
		unsigned int resultCmd = 0;
		map<QString, vector<QObject*>>::iterator itFind = mapGroupToSubs.find(pUser->strName);//�ҵ���ǰָ���Ӧ�����ÿؼ�
		if (itFind != mapGroupToSubs.end())
		{
			for (vector<QObject*>::iterator itSub = itFind->second.begin(); itSub != itFind->second.end(); itSub++)
			{
				QLineEdit* lineEdit = qobject_cast<QLineEdit*>(*itSub);
				QComboBox* comBox = qobject_cast<QComboBox*>(*itSub);
				if (lineEdit)
				{
					User* pUserLine = (User*)(lineEdit->userData(Qt::UserRole));
					int iData = lineEdit->text().toInt(NULL, 16);
					iData = iData % ((int)pow(2, pUserLine->iValidBits));//��Ч����
					iData = iData << pUserLine->iStartBit;//ƫ��
					resultCmd += iData;
				}
				else if (comBox)
				{
					if (comBox)
					{
						map<QString, int>::iterator itPeriod = curYaoKongLay->mapComboxParams.find(comBox->currentText());
						if (itPeriod != curYaoKongLay->mapComboxParams.end())
						{
							User* pUserCombox = (User*)(comBox->userData(Qt::UserRole));
							int iData = itPeriod->second;
							iData = iData % ((int)pow(2, pUserCombox->iValidBits));//��Ч����
							iData = iData << pUserCombox->iStartBit;//ƫ��
							resultCmd += iData;
						}
					}
				}
			}
		}
		//D4
		byteCmdBuf[4] = pUser->nID;
		byteCmdBuf[5] = (resultCmd / 256 / 256 / 256) % 256;
		byteCmdBuf[6] = (resultCmd / 256 / 256) % 256;
		byteCmdBuf[7] = (resultCmd / 256) % 256;
		byteCmdBuf[8] = resultCmd % 256;

		DataBlockBroadcast broadcast;
		for (int i = 2; i < 9; i++)
		{
			broadcast.Sum(byteCmdBuf[i]);
		}
		byteCmdBuf[9] = broadcast.SUM;//��У��
	}
	//comboxƽ��
	else if (pUser->iFlag == 52)
	{

		std::string header = ykyc["yao_kong"]["cmd_header"];
		hexstr2byte_array(header.c_str(), byteCmdBuf, header.length() / 2);

		//D5-D9
		unsigned int resultCmd = 0;
		map<QString, vector<QObject*>>::iterator itFind = mapGroupToSubs.find(pUser->strName);//�ҵ���ǰָ���Ӧ�����ÿؼ�
		if (itFind != mapGroupToSubs.end())
		{
			for (vector<QObject*>::iterator itSub = itFind->second.begin(); itSub != itFind->second.end(); itSub++)
			{
				QLineEdit* lineEdit = qobject_cast<QLineEdit*>(*itSub);
				QComboBox* comBox = qobject_cast<QComboBox*>(*itSub);
				QLabel* label = qobject_cast<QLabel*>(*itSub);
				if (lineEdit)
				{
					User* pUserLine = (User*)(lineEdit->userData(Qt::UserRole));
					int iData = lineEdit->text().toInt(NULL, 16);
					iData = iData % ((int)pow(2, pUserLine->iValidBits));//��Ч����
					iData = iData << pUserLine->iStartBit;//ƫ��
					resultCmd += iData;
				}
				else if (comBox)
				{
					map<QString, int>::iterator itPeriod = curYaoKongLay->mapComboxParams.find(comBox->currentText());
					if (itPeriod != curYaoKongLay->mapComboxParams.end())
					{
						User* pUserCombox = (User*)(comBox->userData(Qt::UserRole));
						int iData = itPeriod->second;
						iData = iData % ((int)pow(2, pUserCombox->iValidBits));//��Ч����
						iData = iData << pUserCombox->iStartBit;//ƫ��
						resultCmd += iData;
					}
				}
				else if (label)
				{
					map<QString, int>::iterator itPeriod = curYaoKongLay->mapComboxParams.find(label->text());
					if (itPeriod != curYaoKongLay->mapComboxParams.end())
					{
						User* pUserCombox = (User*)(label->userData(Qt::UserRole));
						int iData = itPeriod->second;
						iData = iData % ((int)pow(2, pUserCombox->iValidBits));//��Ч����
						iData = iData << pUserCombox->iStartBit;//ƫ��
						resultCmd += iData;
					}
				}
			}
		}
		//D4
		byteCmdBuf[4] = pUser->nID;
		byteCmdBuf[5] = (resultCmd / 256 / 256 / 256) % 256;
		byteCmdBuf[6] = (resultCmd / 256 / 256) % 256;
		byteCmdBuf[7] = (resultCmd / 256) % 256;
		byteCmdBuf[8] = resultCmd % 256;

		DataBlockBroadcast broadcast;
		for (int i = 2; i < 9; i++)
		{
			broadcast.Sum(byteCmdBuf[i]);
		}
		byteCmdBuf[9] = broadcast.SUM;//��У��
	}
	//�Ƕ�����
	else if (pUser->iFlag == 53)
	{
		BYTE data[16] = { 0 };
		data[1] = ui.comboBox_CAN->currentIndex();
		data[0] = pUser->nID / 256;
		data[2] = pUser->nID % 256;


		map<QString, vector<QObject*>>::iterator itFind = mapGroupToSubs.find(pUser->strName);//�ҵ���ǰָ���Ӧ�����ÿؼ�
		if (itFind != mapGroupToSubs.end())
		{
			for (vector<QObject*>::iterator itSub = itFind->second.begin(); itSub != itFind->second.end(); itSub++)
			{
				QLineEdit* lineEdit = qobject_cast<QLineEdit*>(*itSub);
				QComboBox* comBox = qobject_cast<QComboBox*>(*itSub);
				QLabel* label = qobject_cast<QLabel*>(*itSub);
				if (lineEdit)
				{
					User* pUserLine = (User*)(lineEdit->userData(Qt::UserRole));
					//string hexstr = lineEdit->text().toStdString();
					QString str = lineEdit->text();

					int start = pUserLine->iStartBit;
					int length = pUserLine->iValidBits;

					if (str.length() > 16) {
						str = str.mid(0, 16);
					}
					unsigned long long num = str.toULongLong(nullptr, pUserLine->base);

					for (int i = 0; i < length; i++) {
						data[start + length - 1 - i] = num >> i * 8;
					}

					//string hexstr = QString::number(num, 16).toStdString();

					//if (hexstr.length() % 2) {
					//	hexstr = hexstr + "0";
					//}

					//hexstr2byte_array(hexstr.c_str(), data + start, length);
				}
				else if (comBox)
				{
					map<QString, int>::iterator itPeriod = curYaoKongLay->mapComboxParams.find(comBox->currentText());
					if (itPeriod != curYaoKongLay->mapComboxParams.end())
					{
						User* pUserCombox = (User*)(comBox->userData(Qt::UserRole));
						int iData = itPeriod->second;
						int start = pUserCombox->iStartBit;
						int length = pUserCombox->iValidBits;
						for (int i = 0; i < length; i++) {
							data[start + i] = iData >> (length - i - 1) * 8 << 6;
						}
					}
				}
			}
		}

		m_port.m_serial->write((char*)data, 16);
		showCmdInWnd(data, 16);
		return;

	}
	//�ڲ�CAN���߸�λ�㲥
	else if (pUser->iFlag == 54)
	{

		//D5-D9
		unsigned int resultCmd = 0;
		map<QString, vector<QObject*>>::iterator itFind = mapGroupToSubs.find(pUser->strName);//�ҵ���ǰָ���Ӧ�����ÿؼ�
		if (itFind != mapGroupToSubs.end())
		{
			for (vector<QObject*>::iterator itSub = itFind->second.begin(); itSub != itFind->second.end(); itSub++)
			{
				QLineEdit* lineEdit = qobject_cast<QLineEdit*>(*itSub);
				QComboBox* comBox = qobject_cast<QComboBox*>(*itSub);
				if (lineEdit)
				{
					User* pUserLine = (User*)(lineEdit->userData(Qt::UserRole));
					int iData = lineEdit->text().toInt(NULL, 16);
					iData = iData % ((int)pow(2, pUserLine->iValidBits));//��Ч����
					iData = iData << pUserLine->iStartBit;//ƫ��
					resultCmd += iData;
				}
				else if (comBox)
				{
					map<QString, int>::iterator itPeriod = curYaoKongLay->mapComboxParams.find(comBox->currentText());
					if (itPeriod != curYaoKongLay->mapComboxParams.end())
					{
						User* pUserCombox = (User*)(comBox->userData(Qt::UserRole));
						int iData = itPeriod->second;
						int start = pUserCombox->iStartBit;
						int length = pUserCombox->iValidBits;
						for (int i = 0; i < length; i++) {
							byteCmdBuf[start + i] = iData >> (length - i - 1) * 8;
						}
					}
				}
			}
		}
		//D4
		byteCmdBuf[0] = 0x9F;
		byteCmdBuf[1] = 0xC8;
		byteCmdBuf[2] = 0xE0;
		byteCmdBuf[3] = pUser->nID;

		DataBlockBroadcast broadcast;
		for (int i = 2; i < 9; i++)
		{
			broadcast.Sum(byteCmdBuf[i]);
		}
		byteCmdBuf[9] = broadcast.SUM;//��У��
	}

	//m_port.writeData((char*)byteCmdBuf);

	static std::string head = ykyc["serial_port_to_can"]["yao_kong"]["add_head"];
	static std::string tail = ykyc["serial_port_to_can"]["yao_kong"]["add_tail"];
	static char* data = new char[CmdLength + head.length() / 2 + tail.length() / 2];

	hexstr2byte_array(head.c_str(), (byte*)data, head.length() / 2);
	memcpy(data + +head.length() / 2, byteCmdBuf, CmdLength);
	hexstr2byte_array(tail.c_str(), (byte*)data + head.length() / 2 + CmdLength, tail.length() / 2);
	//AB����
	data[1] = ui.comboBox_CAN->currentIndex();

	m_port.m_serial->write(data, CmdLength + head.length() / 2 + tail.length() / 2);

	showCmdInWnd();
}

void CDlgComm::on_cameraRadio_clicked()
{
	QRadioButton* radio = qobject_cast<QRadioButton*>(sender());
	QString strName = radio->text().trimmed();
	map<QString, int>::iterator itType = curYaoKongLay->mapComboxParams.find(strName);
	if (itType != curYaoKongLay->mapComboxParams.end())
	{
		byteCamera = itType->second;
	}
}

//#pragma optimize("",off)
void CDlgComm::slot_recvSerialData(BYTE* recvData_, int iLen)
{
	recvSerialData = true;

	BYTE* recvData = recvData_ + 4;

	//recvData[6] += recvData[4];
	//recvData += 5;
	//QLOG_INFO() << QString::fromStdString(byte_array2hexstr(recvData, iLen));

	std::string recv_data_header = byte_array2hexstr(recvData, 2);
	std::string recv_data_header_ = byte_array2hexstr(recvData_, 2);
	//ң��Ӧ��
	if (recv_data_header == ykyc["yao_kong_response"]["response_header"] ||
		recv_data_header == ykyc["yao_kong_response"]["response_header_2"]) {
		QString strShow;
		strShow += QTime::currentTime().toString();
		strShow += QStringLiteral(" ���� ");
		for (int i = 0; i < CmdLength; i++)
		{
			strShow += QString("%1").arg(recvData[i], 2, 16, QLatin1Char('0')).toUpper();
			strShow += " ";
		}
		*txt_output << strShow << endl;
		QListWidgetItem* item = new QListWidgetItem(strShow);
		//item->setTextAlignment(Qt::AlignCenter);
		QColor green;
		green.setGreen(120);
		item->setTextColor(green);
		//ui.listWidget_CmdList->addItem(item);
		ui.listWidget_CmdList->insertItem(0, item);

		//��������Ӧ
		get_response = true;
	}
	//��עӦ��
	else if (recv_data_header == ykyc["shang_zhu_response"]["response_header"]) {
		QString strShow;
		strShow += QTime::currentTime().toString();
		strShow += QStringLiteral(" ���� ");
		for (int i = 0; i < CmdLength; i++)
		{
			strShow += QString("%1").arg(recvData[i], 2, 16, QLatin1Char('0')).toUpper();
			strShow += " ";
		}
		*txt_output << strShow << endl;
		QListWidgetItem* item = new QListWidgetItem(strShow);
		//item->setTextAlignment(Qt::AlignCenter);
		QColor green;
		green.setGreen(255);
		item->setTextColor(green);
		//ui.listWidget_CmdList->addItem(item);
		ui.listWidget_CmdList->insertItem(0, item);

		//��������Ӧ
		get_response = true;
	}
	//ң��
	else if (recv_data_header_ == ykyc["yao_ce"]["data_header"]) {
		strCurShowType = QStringLiteral("�ڲ���������ң��");

		//yaoceByteArray.append((char*)recvData_, iLen);
		//BYTE* yaoce_dataB = (BYTE*)yaoceByteArray.data();

		int recv_count = iLen / 16;
		for (int i = 0; i < recv_count; i++) {
			memcpy(yaoce_data + 12 * recvData_[i * 16 + 3], recvData_ + i * 16 + 4, 12);
			mapTypeToData[QStringLiteral("�ڲ���������ң��")] = yaoce_data + 2;
			updateYaoceTableData();
			//update();
			QCoreApplication::processEvents();
		}

		//memcpy(yaoce_data + 12 * recvData_[3], recvData_ + 4,12 );
		//if (recvData_[3] == 3) {
		//	mapTypeToData[QStringLiteral("�ڲ���������ң��")] = yaoce_data;
		//	updateYaoceTableData();
		//	memset(yaoce_data, 0x00, 48);
		//}



		//BYTE* yaoce_data = new BYTE[48]{ 0 };

		//yaoce.append((char*)recvData_, iLen);

		//BYTE* yaoce_dataB = (BYTE*)yaoce.data();

		//if (yaoce.size() >= 64) {
		//	//int recv_count = yaoce.size() / 16;
		//	int recv_count = 4;
		//	//if (recv_count > 4) {
		//	//	recv_count = 4;
		//	//}
		//	for (int i = 0; i < recv_count; i++) {
		//		memcpy(yaoce_data + 12 * yaoce.data()[i * 16 + 3], yaoce.data() + i * 16 + 4, 12);

		//		mapTypeToData[QStringLiteral("�ڲ���������ң��")] = yaoce_data + 2;
		//		updateYaoceTableData();
		//		QCoreApplication::processEvents();
		//	}
		//	yaoce.remove(0, recv_count * 16);
		//}
		//delete[] yaoce_data;

		//static QByteArray yaoce;
		////if (recvData_[3] == 0) {
		////	yaoce.clear();
		////}
		//yaoce.append((char*)recvData_, iLen);

		//if (yaoce.size() >= 64) {
		//	if (recvData_[3] != 0) {
		//		yaoce.remove(0, (4 - recvData_[3]) * 16);
		//	}
		//	else {
		//		for (int i = 0; i < 4; i++) {
		//			memcpy(yaoce_data + 12 * i, yaoce.data() + 16 * i + 4, 12);
		//		}
		//		mapTypeToData[QStringLiteral("�ڲ���������ң��")] = yaoce_data + 2;
		//		updateYaoceTableData();
		//		yaoce.remove(0, 64);
		//	}

		//}
		//memset(yaoce_data, 0x00, 48);
	}
	//��עң��
	else if (recv_data_header_ == ykyc["shang_zhu_yao_ce"]["data_header"]) {

		int recv_count = iLen / 16;
		for (int i = 0; i < recv_count; i++) {
			memcpy(yaoce_data_2 + 12 * recvData_[i * 16 + 3], recvData_ + i * 16 + 4, 12);
		}
		mapTypeToData[QStringLiteral("��ע״̬ң��")] = yaoce_data_2 + 2;
		updateYaoceTableData_2();
	}
	else {

	}


	delete[] recvData_;

}
//#pragma optimize("",on)
void CDlgComm::slot_sendGetDYCmd()
{
	if (iMulSendFlag == 0)
	{
		memset(byteCmdBuf, 0x00, CmdLength);
		byteCmdBuf[0] = 0x3e;

		byteCmdBuf[2] = iIndex;
		m_port.writeData((char*)byteCmdBuf);
		m_port.writeData((char*)byteCmdBuf);
		m_port.writeData((char*)byteCmdBuf);
		showCmdInWnd();
		//Sleep(2000);
		iIndex++;
		if (iIndex > 5)
		{
			iIndex = 0;
		}

		//updateDianYaTableData();
	}
	else if (iMulSendFlag == 1)
	{
		memset(byteCmdBuf, 0x00, CmdLength);
		byteCmdBuf[0] = 0x20;

		//byteCmdBuf[1] = iIndex;
		m_port.writeData((char*)byteCmdBuf);
		showCmdInWnd();
		//iIndex++;
		//if (iIndex > 3)
		//{
		//	iIndex = 0;
		//}
	}
}

void CDlgComm::mousePressEvent(QMouseEvent *event)
{
	m_pressPos = event->pos();
	bPress = false;
	//if ((event->button() == Qt::LeftButton) && m_pressPos.x() < (this->width() - 80) && m_pressPos.y() < (ui.topWidget->height()))
	{
		//�������ڴ����λ�ã�����ʹ��event->globalPos() - this->pos()��
		bPress = true;
	}
}

void CDlgComm::mouseMoveEvent(QMouseEvent *event)
{
	if (bPress)
	{
		//����������Ļ��λ��
		QPoint move_pos = event->globalPos();
		//this->move(move_pos - m_pressPos);
	}
}

void CDlgComm::mouseReleaseEvent(QMouseEvent *event)
{
	bPress = false;
}

void CDlgComm::on_pushButton_folder_clicked() {
	QString folder_dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
		"./",
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);

	if (folder_dir.isEmpty()) {
		return;
	}

	auto table = ui.tableWidget_cmd;

	//���ò��ɱ༭
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);

	table->setColumnCount(2);
	QStringList list_horheader;
	list_horheader.append(QStringLiteral("�ļ�"));
	list_horheader.append(QStringLiteral("ָ��"));
	table->setHorizontalHeaderLabels(list_horheader);

	QDir dir(folder_dir);
	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	dir.setSorting(QDir::Size | QDir::Reversed);
	QFileInfoList list = dir.entryInfoList();

	table->setRowCount(list.size());

	for (int i = 0; i < list.size(); ++i) {
		QFileInfo fileInfo = list.at(i);
		//�����ļ���
		table->setItem(i, 0, new QTableWidgetItem(fileInfo.fileName()));

		QString filepath;
		filepath.append(fileInfo.path());
		filepath += "/" + fileInfo.fileName();
		QFile file(filepath);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QString cmd = file.readLine();
			//����ָ������
			table->setItem(i, 1, new QTableWidgetItem(cmd));
		}
	}
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//����Ӧ�п�
	table->verticalHeader()->setVisible(false); //���ô�ֱͷ���ɼ�
}
void CDlgComm::on_pushButton_cmd_name_clicked()
{
	QTableWidget* table = ui.tableWidget_cmd;

	QString file_path = QFileDialog::getOpenFileName(this, QStringLiteral("��txt�ļ�"), "./", "(*.txt)");
	if (file_path.isEmpty()) {
		return;
	}
	auto tmp = file_path;
	QFile file(file_path);
	int row_counter = 0;
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {

		while (true) {
			QString cmd = file.readLine();
			if (cmd.isEmpty()) {
				break;
			}
			row_counter++;
		}
		file.close();
	}
	if (row_counter > cmd_row_count) {
		cmd_row_count = row_counter;
		table->setRowCount(cmd_row_count);
	}
	QTextCodec* codec = QTextCodec::codecForName("GBK");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		int i = 0;
		while (true) {
			QString cmd = codec->toUnicode(file.readLine());
			if (cmd.isEmpty()) {
				break;
			}
			//����ָ������
			table->setItem(i, 0, new QTableWidgetItem(cmd));
			i++;
		}
		file.close();
	}

}
void CDlgComm::on_pushButton_cmd_name_custom_clicked()
{
	QTableWidget* table = ui.tableWidget_cmd;

	QString file_path = QFileDialog::getOpenFileName(this, QStringLiteral("��txt�ļ�"), "./", "(*.txt)");
	if (file_path.isEmpty()) {
		return;
	}
	auto tmp = file_path;
	QFile file(file_path);
	int row_counter = 0;
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {

		while (true) {
			QString cmd = file.readLine();
			if (cmd.isEmpty()) {
				break;
			}
			row_counter++;
		}
		file.close();
	}
	if (row_counter > cmd_row_count) {
		cmd_row_count = row_counter;
		table->setRowCount(cmd_row_count);
	}
	QTextCodec* codec = QTextCodec::codecForName("GBK");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		int i = 0;
		while (true) {
			QString cmd = codec->toUnicode(file.readLine());
			if (cmd.isEmpty()) {
				break;
			}
			//����ָ������
			table->setItem(i, 2, new QTableWidgetItem(cmd));
			i++;
		}
		file.close();
	}

}
void CDlgComm::on_pushButton_cmd_value_clicked()
{
	QTableWidget* table = ui.tableWidget_cmd;

	QString file_path = QFileDialog::getOpenFileName(this, QStringLiteral("��txt�ļ�"), "./", "(*.txt)");
	if (file_path.isEmpty()) {
		return;
	}
	QFile file(file_path);
	int row_counter = 0;
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		while (true) {
			QString cmd = file.readLine();
			if (cmd.isEmpty()) {
				break;
			}
			row_counter++;
		}
		file.close();
	}
	if (row_counter > cmd_row_count) {
		cmd_row_count = row_counter;
		table->setRowCount(cmd_row_count);
	}
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		int i = 0;
		while (true) {
			QString cmd = file.readLine();
			if (cmd.isEmpty()) {
				break;
			}
			//����ָ������
			table->setItem(i, 1, new QTableWidgetItem(cmd));
			i++;
		}
		file.close();
	}
}
void CDlgComm::on_pushButton_send_clicked()
{
	QString cmd;
	cmd = ui.lineEdit_cmd->text();
	cmd.replace(" ", "");

	//������Ҫ��ӵ�ͷ��β
	static std::string head = ykyc["serial_port_to_can"]["yao_kong"]["add_head"];
	static std::string tail = ykyc["serial_port_to_can"]["yao_kong"]["add_tail"];
	static char* data = new char[CmdLength + head.length() / 2 + tail.length() / 2];
	hexstr2byte_array(head.c_str(), (byte*)data, head.length() / 2);
	hexstr2byte_array(tail.c_str(), (byte*)data + head.length() / 2 + CmdLength, tail.length() / 2);
	//AB����
	data[1] = ui.comboBox_CAN->currentIndex();

	int sleep_count = 0;

	//ѭ������һ�е�ָ��
	int cmd_count = cmd.length() / 2 / CmdLength;
	for (int i = 0; i < cmd_count; i++) {
		trans_cmd(cmd.mid(CmdLength*i * 2, CmdLength * 2));
		memcpy(data + head.length() / 2, byteCmdBuf, CmdLength);
		if (get_response) {
			m_port.m_serial->write(data, CmdLength + head.length() / 2 + tail.length() / 2);
			showCmdInWnd();
			get_response = false;
		}
		else {
			Sleep(16);
			i--;

			sleep_count++;
			if (sleep_count >= 60) {
				QMessageBox::critical(this, "ERROR", QStringLiteral("Ӧ��ʱ�䳬��һ��"));
				m_timer->stop();
				send_counter = 0;
				send_times_counter = 0;
				get_response = true;
				return;
			}
			//�ֶ��¼�ѭ������ֹ����
			QCoreApplication::processEvents();
		}
	}
	//���η���
	get_response = true;
}
void CDlgComm::on_pushButton_send_circle_clicked()
{
	//double speed = ui.doubleSpinBox_speed->value();
	//if (speed <= 0) {
	//	return;
	//}
	double send_delay = ui.doubleSpinBox_send_delay->value();

	//send_circle_flag = true;
	send_times = ui.spinBox_times->value();
	send_number = 0;
	send_counter = 0;
	send_times_counter = 0;

	//m_timer->start(1000 / speed);
	m_timer->start(send_delay * 1000);

	QListWidgetItem* item = new QListWidgetItem(QStringLiteral("��ʼ���� . . ."));
	ui.listWidget_CmdList->insertItem(0, item);

}

void CDlgComm::on_pushButton_send_circle_stop_clicked()
{
	m_timer->stop();

	QListWidgetItem* item = new QListWidgetItem(QStringLiteral("�������� . . ."));
	ui.listWidget_CmdList->insertItem(0, item);
}

void CDlgComm::on_pushButton_broadcast_switch_send_clicked()
{
	BYTE broadcat_switch[16] = { 0 };
	broadcat_switch[0] = 0x31;
	broadcat_switch[1] = 0x00;
	broadcat_switch[2] = 0x07;
	broadcat_switch[3] = ui.radioButton_broadcast_3->isChecked();
	broadcat_switch[4] = ui.comboBox_broadcast_4->currentIndex();
	broadcat_switch[5] = ui.comboBox_broadcast_5->currentIndex();
	broadcat_switch[6] = ui.radioButton_broadcast_6->isChecked();
	broadcat_switch[7] = ui.radioButton_broadcast_7->isChecked();
	broadcat_switch[8] = ui.radioButton_broadcast_8->isChecked();
	broadcat_switch[9] = ui.radioButton_broadcast_9->isChecked();

	if (broadcat_switch[5] >= 0x02) {
		broadcat_switch[4] = broadcat_switch[5];
		m_port.m_serial->write((char*)broadcat_switch, 16);
		showCmdInWnd(broadcat_switch, 16);

		broadcat_switch[4] = 0x00;
		m_port.m_serial->write((char*)broadcat_switch, 16);
		showCmdInWnd(broadcat_switch, 16);
	}

	broadcat_switch[4] = ui.comboBox_broadcast_4->currentIndex();
	m_port.m_serial->write((char*)broadcat_switch, 16);
	showCmdInWnd(broadcat_switch, 16);
}

void CDlgComm::on_pushButton_yaoce_control_send_clicked()
{
	BYTE yaoce_switch[16] = { 0 };
	yaoce_switch[0] = 0x30;
	yaoce_switch[1] = 0x00;
	yaoce_switch[2] = 0x08;
	yaoce_switch[3] = ui.radioButton_yaoce_control_1->isChecked();
	yaoce_switch[4] = ui.radioButton_yaoce_control_2->isChecked();
	yaoce_switch[5] = ui.radioButton_yaoce_control_3->isChecked();
	yaoce_switch[6] = ui.radioButton_yaoce_control_4->isChecked();
	yaoce_switch[7] = ui.radioButton_yaoce_control_5->isChecked();
	yaoce_switch[8] = ui.radioButton_yaoce_control_6->isChecked();
	yaoce_switch[9] = ui.radioButton_yaoce_control_7->isChecked();
	yaoce_switch[10] = ui.radioButton_yaoce_control_8->isChecked();

	m_port.m_serial->write((char*)yaoce_switch, 16);
	showCmdInWnd(yaoce_switch, 16);
}

void CDlgComm::on_pushButton_other_1_clicked()
{
	BYTE data[16] = { 0 };
	data[0] = 0x32;
	data[1] = 0x00;
	data[2] = 0x01;
	data[3] = ui.comboBox_other_1->currentIndex();

	m_port.m_serial->write((char*)data, 16);
	showCmdInWnd(data, 16);
}

void CDlgComm::on_pushButton_other_2_clicked()
{
	BYTE data[16] = { 0 };
	data[0] = 0x33;
	data[1] = 0x00;
	data[2] = 0x01;
	data[3] = ui.comboBox_other_2->currentIndex();

	m_port.m_serial->write((char*)data, 16);
	showCmdInWnd(data, 16);
}

void CDlgComm::on_pushButton_other_3_clicked()
{
	BYTE data[16] = { 0 };
	data[0] = 0x34;
	data[1] = 0x00;
	data[2] = 0x01;
	data[3] = ui.comboBox_other_3->currentIndex();

	m_port.m_serial->write((char*)data, 16);
	showCmdInWnd(data, 16);
}

void CDlgComm::on_pushButton_other_4_clicked()
{
	BYTE data[16] = { 0 };
	data[0] = 0x35;
	data[1] = 0x00;
	data[2] = 0x01;
	data[3] = ui.comboBox_other_4->currentIndex();

	m_port.m_serial->write((char*)data, 16);
	showCmdInWnd(data, 16);
}

void CDlgComm::on_pushButton_other_5_clicked()
{
	BYTE data[16] = { 0 };
	data[0] = 0x39;
	data[1] = 0x00;
	data[2] = 0x01;
	data[3] = ui.comboBox_other_5->currentIndex();

	m_port.m_serial->write((char*)data, 16);
	showCmdInWnd(data, 16);
}


void CDlgComm::on_pushButton_save_path_clicked()
{
	QString save_path = QFileDialog::getExistingDirectory(this, QStringLiteral("ѡ��洢λ��"));
	if (save_path.isEmpty()) {
		return;
	}
	ui.lineEdit_save_path->setText(save_path);
}

void CDlgComm::on_pushButton_reset_clicked()
{
	BYTE data[16] = { 0 };
	data[0] = 0x38;
	data[1] = 0x00;
	data[2] = 0x01;
	data[3] = 0x01;

	m_port.m_serial->write((char*)data, 16);
	showCmdInWnd(data, 16);

	//QLOG_INFO() << QStringLiteral("ͼ�ɸ�λָ��: ")<<QByteArray::fromRawData((char*)data,16).toHex(' ') <<QStringLiteral("�ѷ���");
}

void CDlgComm::on_pushButton_upload_file_open_clicked()
{
	QString file_path = QFileDialog::getOpenFileName(this, tr("Open File"));
	if (file_path.isEmpty()) {
		return;
	}
	ui.lineEdit_upload_file_path->setText(file_path);
}

void CDlgComm::on_pushButton_upload_send_begin_clicked()
{
	int upload_start_number = ui.spinBox_upload_start_number->value();
	int upload_end_number = ui.spinBox_upload_start_number->value();

	m_package_count = upload_start_number;
	iEndPagNum = upload_end_number;

	sendDataNew();
}
void CDlgComm::on_pushButton_upload_send_pause_clicked()
{
	QString str;
	str = ui.pushButton_upload_send_pause->text();
	if (str == QStringLiteral("��ͣ")) {
		pause_flag = true;
		upload_timer.stop();
		ui.pushButton_upload_send_pause->setText(QStringLiteral("����"));
	}
	else {
		int time = ui.lineEdit_upload_send_delay->text().toInt();
		pause_flag = false;
		upload_timer.start(time);
		ui.pushButton_upload_send_pause->setText(QStringLiteral("��ͣ"));
	}
}
void CDlgComm::on_pushButton_upload_send_stop_clicked()
{
	stop_flag = true;
}
void CDlgComm::sendDataNew()
{
	std::string file_path = ui.lineEdit_upload_file_path->text().toStdString();
	std::string strErr = "";
	bool bHaveReadSendFile = broadcast.ReadSendDataFile(file_path, strErr);//��ȡ�ļ�
	if (!bHaveReadSendFile)
	{
		QMessageBox::critical(this, "ERROR", QString::fromStdString(strErr));
		return;
	}
	m_time = ui.lineEdit_upload_send_delay->text().toInt();     //���ͼ��ʱ��

	m_program_edition = ui.lineEdit_upload_program_version->text().toInt();     //�汾��
	version = m_program_edition;

	bShangZhu = true;//��ע��ʼ
//	SetTimer(120,m_time,NULL);
	//UINT32 threadId;
	m_loadstart = true;
	pause_flag = false;
	stop_flag = false;
	//HANDLE  m_hProThread = (HANDLE)_beginthreadex(NULL, 0, CDlgComm::ProThread_time, this, CREATE_SUSPENDED, &threadId);
	//ResumeThread(m_hProThread);
	//std::thread* m_hProThread = new std::thread([this]() {
	//	this->ProThread_time(this);
	//});
	upload_timer.start(m_time);

	failedData.clear();//ÿ�����·��ͽ�ʧ�ܰ����
}

void CDlgComm::slot_upload_send() {
	//CDlgComm *p = (CDlgComm*)params;
	QString str;
	//while (1) {
		//if (stop_flag) {
		//	break;
		//}
		//if (pause_flag) {
		//	continue;
		//}
	m_loadstart = true;
	SendFpgaPackage(m_package_count);
	//str = QStringLiteral("�ѷ��ͳ�������%1��").arg(p->m_package_count);
	if (((m_package_count % 100) == 0) && (m_package_count != 0))
		//p->m_sendlist.ResetContent();
	//p->m_sendlist.InsertString(0, str);
		if (m_package_count > iEndPagNum)
			upload_timer.stop();

	//Sleep(m_time);
	//while (m_loadstart);
	m_package_count++;
	//}
}
void CDlgComm::SendFpgaPackage(int iPag)
{
	//BYTE type = m_processselect.GetCurSel() + 0xe;

	BYTE type = 0x0E;

	vector<BYTE*> vecData = broadcast.GetOnePagData("��ע���ݿ�㲥", type, version, iPag - 1);//��ȡ��������
	//CString str;
	BYTE   CMD[18];
	//CGrabImage*  grabimage = (CGrabImage*)theapp_mainparams.m_window_manager.grabdlg;
	bOnePakFinish = true;
	for (int i = 0; i < vecData.size(); i++)
	{

		memset(CMD, 0, 18);
		CMD[0] = 0x10;
		CMD[1] = 0x00;
		CMD[2] = 11;
		CMD[3] = 8;
		memcpy(&CMD[4], vecData[i], 10);
		//grabimage->Insert_CommCmd(CMD, true);
		m_port.m_serial->write((char*)CMD, 18);

	}
	bOnePakFinish = false;
	broadcast.DestorySendData();
}


void CDlgComm::slot_send_circle()
{
	//m_timer->stop();
	QTableWidget* table = ui.tableWidget_cmd;
	if (send_counter >= table->rowCount()) {
		return;
	}
	QString cmd = table->item(send_counter, 1)->text();
	cmd.replace(" ", "");

	//������Ҫ��ӵ�ͷ��β
	static std::string head = ykyc["serial_port_to_can"]["yao_kong"]["add_head"];
	static std::string tail = ykyc["serial_port_to_can"]["yao_kong"]["add_tail"];
	static char* data = new char[CmdLength + head.length() / 2 + tail.length() / 2];
	hexstr2byte_array(head.c_str(), (byte*)data, head.length() / 2);
	hexstr2byte_array(tail.c_str(), (byte*)data + head.length() / 2 + CmdLength, tail.length() / 2);
	//AB����
	data[1] = ui.comboBox_CAN->currentIndex();


	int sleep_count = 0;

	//ѭ������һ�е�ָ��
	int cmd_count = cmd.length() / 2 / CmdLength;
	for (int i = 0; i < cmd_count; i++) {
		trans_cmd(cmd.mid(CmdLength*i * 2, CmdLength * 2));
		memcpy(data + head.length() / 2, byteCmdBuf, CmdLength);
		if (get_response) {
			m_port.m_serial->write(data, CmdLength + head.length() / 2 + tail.length() / 2);
			showCmdInWnd();
			get_response = false;
			sleep_count = 0;
		}
		else {
			Sleep(16);
			i--;

			sleep_count++;
			if (sleep_count >= 60) {
				QMessageBox::critical(this, "ERROR", QStringLiteral("Ӧ��ʱ�䳬��һ��"));
				m_timer->stop();
				send_counter = 0;
				send_times_counter = 0;
				get_response = true;
				return;
			}
			//�ֶ��¼�ѭ������ֹ����
			QCoreApplication::processEvents();
		}
	}
	//һ�е����һ��ָ�����ɺ󣬼���Ƿ���Ӧ��
	sleep_count = 0;
	while (true) {
		if (get_response) {
			break;
		}
		else {
			Sleep(16);

			sleep_count++;
			if (sleep_count >= 60) {
				QMessageBox::critical(this, "ERROR", QStringLiteral("Ӧ��ʱ�䳬��һ��"));
				m_timer->stop();
				send_counter = 0;
				send_times_counter = 0;
				get_response = true;
				return;
			}
			//�ֶ��¼�ѭ������ֹ����
			QCoreApplication::processEvents();
		}
	}

	//������ɺ���ʱ����
	QTimer::singleShot(ui.doubleSpinBox_save_delay->value() * 1000, [=]() {
		QString cmd_save_name = " ";
		if (ui.radioButton_cmd_name_custom->isChecked() && ui.tableWidget_cmd->item(send_counter, 2)) {
			cmd_save_name = ui.tableWidget_cmd->item(send_counter, 2)->text();
		}
		else if (ui.tableWidget_cmd->item(send_counter, 0)) {
			cmd_save_name = ui.tableWidget_cmd->item(send_counter, 0)->text();
		}
		IBussiness::getInstance().saveDataAfterCmd(ui.lineEdit_save_path->text(), cmd_save_name, send_times_counter);
	});

	send_counter++;
	if (send_counter >= table->rowCount()) {
		send_counter = 0;
		send_times_counter++;
	}
	if (send_times_counter >= send_times) {
		m_timer->stop();
	}
}

void CDlgComm::trans_cmd(QString cmd_str)
{
	//this->ui.lineEdit_cmd->setText(cmd_str.mid(0, CmdLength * 2));

	cmd_str = cmd_str.replace(" ", "");
	memset(byteCmdBuf, 0x00, CmdLength);
	if (cmd_str.length() < CmdLength * 2) {
		hexstr2byte_array(cmd_str.toStdString().c_str(), this->byteCmdBuf, cmd_str.length() / 2);
	}
	else {
		hexstr2byte_array(cmd_str.toStdString().c_str(), this->byteCmdBuf, CmdLength);
	}

	if (ui.checkBox_auto_checksum->isChecked()) {
		DataBlockBroadcast broadcast;
		for (int i = 2; i < CmdLength - 1; i++)
		{
			broadcast.Sum(byteCmdBuf[i]);
		}
		byteCmdBuf[9] = broadcast.SUM;//��У��
	}
}
void CDlgComm::slot_update_yaoce_table() {
	BYTE* t = (BYTE*)yaoceByteArray.data();

	int index = yaoceByteArray.data()[3];
	if (yaoceByteArray.size() >= 64 && index != 0x00) {
		yaoceByteArray.remove(0, (4 - index) * 16);
	}
	if (yaoceByteArray.size() >= 64 && index == 0x00) {
		for (int i = 0; i < 4; i++) {
			memcpy(yaoce_data + 12 * i, yaoceByteArray.data() + 16 * i + 4, 12 * sizeof(BYTE));
		}
		mapTypeToData[QStringLiteral("�ڲ���������ң��")] = yaoce_data + 2;
		updateYaoceTableData();
		yaoceByteArray.remove(0, 64);
	}
}