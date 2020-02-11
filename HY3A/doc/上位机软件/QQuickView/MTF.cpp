#include "MTF.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include "QsLog.h"
#include "IBussiness.h"

MTF::MTF(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.groupBox_hide->hide();

	connect(&timer, &QTimer::timeout, this, &MTF::update_MTF);
}

MTF::~MTF()
{
	delete[] data16;
	delete[] m_average_buffer;
}

void MTF::init_MTF(QString DataSourceID_, QString DataSourceType_)
{
	QString title;
	sDataSourceType = DataSourceType_;
	sDataSourceID = DataSourceID_;

	saveParam.strDataID = sDataSourceID;
	saveParam.saveFlag = SAVECURFRAME;

	DataAnalysisModel* m_pDataAnalysisModel = IBussiness::getInstance().get_pDataAnalysisModel();

	if (sDataSourceType == "Camera")
	{
		CameraParams* pData = m_pDataAnalysisModel->GetDataByCameraID(sDataSourceID);
		width = pData->One_Frame_LineLength;
		height = pData->One_Frame_LineCount;
		title.prepend(pData->Camera_Name);
		saveParam.saveType = SAVE_CAMERA_DATA;
	}
	else if (sDataSourceType == "Sensor")
	{
		SensorParams* pCCD = m_pDataAnalysisModel->GetDataBySensor_ID(sDataSourceID);
		width = pCCD->Sensor_Size.width();
		height = pCCD->Sensor_Size.height();
		title.prepend(pCCD->strSensor_Name);
		saveParam.saveType = SAVE_SENSOR_DATA;
	}
	else if (sDataSourceType == "Spectral")
	{
		SpectralParams* pSpectral = m_pDataAnalysisModel->GetDataBySpectralID(sDataSourceID);
		width = pSpectral->iSpectral_Width;
		height = pSpectral->iSpectral_Height;
		title.prepend(pSpectral->strSpectral_Name);
		saveParam.saveType = SAVE_SPECTRAL_DATA;
	}
	this->setWindowTitle(title);

	set_data_type(width, height);

	data16 = new unsigned short[width*height]{ 0 };
	m_average_buffer = new unsigned long[width] {0};
	memset(&m_mtf_value_max, 0, sizeof(MtfDetail) * 100);
	memset(&m_mtf_value_count, 0, sizeof(MtfDetail) * 100);
}

void MTF::update_data_source()
{
	DataAnalysisModel* m_pDataAnalysisModel = IBussiness::getInstance().get_pDataAnalysisModel();
	if (sDataSourceType == "Camera")
	{
		CameraParams* pData = m_pDataAnalysisModel->GetDataByCameraID(sDataSourceID);
		data_source = pData->CurCameraData;
	}
	else if (sDataSourceType == "Sensor")
	{
		SensorParams* pCCD = m_pDataAnalysisModel->GetDataBySensor_ID(sDataSourceID);
		data_source = pCCD->CurOriginalSensorData;
	}
	else if (sDataSourceType == "Spectral")
	{
		SpectralParams* pSpectral = m_pDataAnalysisModel->GetDataBySpectralID(sDataSourceID);
		data_source = pSpectral->CurOriginalSpectralData;
	}
}

void MTF::update_MTF()
{
	memset(&m_mtf_value_max, 0, sizeof(MtfDetail) * 100);
	memset(&m_mtf_value_count, 0, sizeof(MtfDetail) * 100);

	update_data_source();
	trans_data_source_to_data16();

	cal_average();
	cal_range_aver();
	cal_MTF();

	update_table();

	if (b_auto_save) {
		save_max_data();
	}
}


void MTF::set_data_type(int width_, int height_, bool Data_EndianFlag)
{
	width = width_;//ͼ����
	height = height_;//ͼ��߶�
	bit_start = 0;//��ʼ����
	bit_length = 16;//��Ч���س���
	Data_EndianFlag = Data_EndianFlag;//True��ˣ�FalseС�� 
}

void MTF::set_data_type(int width_, int height_, int bit_start_, int bit_length_, bool Data_EndianFlag)
{
	width = width_;//ͼ����
	height = height_;//ͼ��߶�
	bit_start = bit_start_;//��ʼ����
	bit_length = bit_length_;//��Ч���س���
	Data_EndianFlag = Data_EndianFlag;//True��ˣ�FalseС��
}

void MTF::trans_data_source_to_data16()
{
	for (int index = 0; index < height*width; index++) {
		if (Data_EndianFlag) {
			data16[index] = (data_source[index * 2] << 8) + data_source[index * 2 + 1];
		}
		else {
			data16[index] = (data_source[index * 2 + 1] << 8) + data_source[index * 2];
		}
		data16[index] <<= bit_start;
		data16[index] >>= 16 - bit_length;
	}
}
void MTF::init_MTF_table()
{
	QTableWidget* table = ui.tableWidget_mtf;
	table->clear();
	table->setColumnCount(m_mtf_array);
	table->setRowCount(3);

	QStringList headers_horizon;
	for (int i = 0; i < m_mtf_array; i++) {
		QString str = QStringLiteral("��") + QString::number(i + 1) + QStringLiteral("��");
		headers_horizon.append(str);
	}
	table->setHorizontalHeaderLabels(headers_horizon);

	QStringList headers_vertical;
	headers_vertical.append(QStringLiteral("���ֵ"));
	headers_vertical.append(QStringLiteral("dnvalue"));
	headers_vertical.append(QStringLiteral("λ��"));
	table->setVerticalHeaderLabels(headers_vertical);

	for (int i = 0; i < m_mtf_array; i++) {
		table->setItem(0, i, new QTableWidgetItem(QString::number(m_mtf_value_max[i].mtfvalue)));
		QString dnvalue1_2 = QString::number(m_mtf_value_max[i].dnvalue) + "," + QString::number(m_mtf_value_max[i].dnvalue1);
		table->setItem(1, i, new QTableWidgetItem(dnvalue1_2));
		table->setItem(2, i, new QTableWidgetItem(QString::number(1 + m_mtf_value_max[i].position)
			+ "," +
			QString::number(1 + m_mtf_value_max[i].position)
		));
	}
	//table->show();

}
//�������������ÿһ�еľ�ֵ
void MTF::cal_average()
{
	memset(m_average_buffer, 0, sizeof(unsigned long) * width);

	for (int pixIndex = m_start_col; pixIndex < m_end_col; pixIndex++) {
		//for (int i = start_row; i < start_row + m_cal_row * step_row; i += step_row) {
			//unsigned short temp = data16[i * width + pixIndex];
		for (int i = start_row; i < start_row + m_cal_row; i++) {
			m_average_buffer[pixIndex] += data16[i * width + pixIndex];
		}
	}
	for (int pixIndex = m_start_col; pixIndex < m_end_col; pixIndex++) {
		unsigned long temp = m_average_buffer[pixIndex];
		m_average_buffer[pixIndex] /= m_cal_row;
	}
}
//������������ڵľ�ֵ
void MTF::cal_range_aver()
{
	for (int pixIndex = m_start_col; pixIndex < m_end_col; pixIndex++) {
		for (int i = start_row; i < start_row + m_cal_row; i++) {
			///////////////////////////		   
			WORD wValue = data16[i *width + pixIndex];
			//WORD wHigh = HIBYTE(wValue);
			//WORD wLow = LOBYTE(wValue);
			//WORD wTemp = wLow << 8;
			//wValue = wTemp + wHigh;
			///////////////////////////	
			m_range_aver_buffer += wValue;
		}
	}
	m_range_aver_buffer = m_range_aver_buffer / ((m_end_col - m_start_col)*m_cal_row);
}

void MTF::cal_MTF()
{
	int nStartPos = m_start_col;
	for (int nCol = nStartPos; nCol != width - 1; ++nCol)
	{
		if (m_average_buffer[nCol] > m_range_aver_buffer)
		{
			nStartPos = nCol;
			nStartPos += (m_mtf_seek_pos*m_mtf_sample);
			break;
		}
	}
	if (nStartPos >= width)
	{
		QMessageBox::critical(this, "ERROR", QStringLiteral("������������,���������ã�"));
		QLOG_ERROR() << QStringLiteral("MTF ������������,���������ã�");
		on_pushButton_stopCal_clicked();
		return;
	}

	int start = nStartPos;
	int end = 0;
	double tmp, tmp1;
	unsigned long long value2, value1;
	double mtf_tmp = 0;
	int cnt = 0;
	MtfDetail value;
	int step;
	//int endmtf_to_mtf = start + m_mtf_to_mtf;
	bool ctf = ui.radioButton_ctf->isChecked();
	//������ļ�����������N��
	for (int i = 0; i < m_mtf_array; i++)
	{
		////�ҵ���һ����С��Ŀ�����޵���Ԫ��ֵ��������Ԫ��ֵȫ��С��Ŀ������������㣩���������Ԫ�±�ֵStartIndex
		//value2 = m_average_buffer[start];
		//while (value2 < m_target_th)
		//{
		//	start++;
		//	value2 = m_average_buffer[start];
		//	if (start >= m_end_col)
		//		return;
		//}
		////��StartIndexΪ��ʼ�㡢StartIndex��������İб���ֵΪ�����㣨�����������ͼ����������㣩���ڴ˷�Χ���ҵ����һ������Ŀ�����޵���Ԫ��ֵ
		//if (start + m_mtf_to_mtf > m_end_col) {
		//	return;
		//}
		//for (int j = start; j < start + m_mtf_to_mtf; j++)
		//{
		//	value2 = m_average_buffer[j];
		//	//���±�ֵ��ΪEndIndex
		//	if (value2 > m_target_th)
		//		end = j;
		//	//�����ҹ�����EndIndex��StartIndex�Ĳ�ֵ���ڰб��߶�* ���㱶Ƶ���������
		//	if ((j - start) > m_mtf_count * m_mtf_sample)
		//		break;
		//}
		//����StartIndex---EndIndex��Χ�ڵ���Ԫ��ֵ������MTFֵ�������������£�
		//for (int k = start; k < end; k++)
		end = nStartPos + m_mtf_count * m_mtf_sample * 2;
		for (int k = start; k < end; k++)
		{
			value2 = m_average_buffer[k];
			value1 = m_average_buffer[k + m_mtf_sample];
			tmp = value2;
			tmp1 = value1;
			mtf_tmp = abs(tmp - tmp1) / ((tmp1 + tmp) - 2 * bottom_noise);

			if (ctf)
				mtf_tmp = mtf_tmp * 3.1415926 / 4;

			value.mtfvalue = mtf_tmp;
			value.dnvalue = 0;
			value.dnvalue1 = 0;
			value.dnvalue = value2;
			value.dnvalue1 = value1;
			value.position = k;
			value.count = 0;
			value.next = NULL;
			value.prev = NULL;
			value.tail = NULL;
			//��¼��ǰ��������������MTFֵ
			if (m_mtf_value_max[i].mtfvalue < value.mtfvalue)
			{
				memcpy(&m_mtf_value_max[i], &value, sizeof(MtfDetail));
				m_mtf_value_max[i].update = true;
			}
			//�������������ȡ������Ԫ�±� - StartIndex < ÿ��б��޶� ��
			if ((k - start) < m_mtf_count)
			{
				memcpy(&m_mtf_value_count[k - start], &value, sizeof(MtfDetail));
			}
		}
		start += m_mtf_to_mtf;
		if (start >= m_end_col)
			return;
	}
}

void MTF::update_table()
{
	QTableWidget* table = ui.tableWidget_mtf;
	//���±�
	for (int i = 0; i < m_mtf_array; i++) {
		table->item(0, i)->setText(QString::number(m_mtf_value_max[i].mtfvalue));

		QString dnvalue1_2 = QString::number(m_mtf_value_max[i].dnvalue)
			+ "," + QString::number(m_mtf_value_max[i].dnvalue1);
		table->item(1, i)->setText(dnvalue1_2);

		QString position1_2 = QString::number(1 + m_mtf_value_max[i].position)
			+ "," + QString::number(1 + m_mtf_value_max[i].position);
		table->item(2, i)->setText(position1_2);
	}
	table->viewport()->update();
	//�������ֵmtf_max�����ֵ���ڵ�����mtf_array_max���������ʱ��Ӧ��dnֵ��λ��
	for (int i = 0; i < m_mtf_array; i++) {
		if (mtf_max < m_mtf_value_max[i].mtfvalue) {
			mtf_max = m_mtf_value_max[i].mtfvalue;
			mtf_max_updated = true;
			array_max = i + 1;
			position_max = m_mtf_value_max[i].position;
			dn1_max = m_mtf_value_max[i].dnvalue;
			dn2_max = m_mtf_value_max[i].dnvalue1;

		}
	}
	ui.lineEdit_mtf_max->setText(QString::number(mtf_max));
	ui.lineEdit_mtf_array_max->setText(QString::number(array_max));
	ui.lineEdit_mtf_position_max->setText(QString::number(1 + position_max)
		+ "," + QString::number(1 + position_max + m_mtf_sample));
	ui.lineEdit_mtf_dn_max->setText(QString::number(dn1_max) + "," + QString::number(dn2_max));
}

void MTF::save_max_data()
{
	//��ͼ��Ϣ
	QString save_info =
		//QStringLiteral("�׶���") + QString::number(42)
		this->windowTitle()
		+ QStringLiteral("-��󴫺�ֵ") + QString::number(mtf_max)
		+ QStringLiteral("-���DNֵ") + QString::number(dn1_max)
		+ QStringLiteral("-��СDNֵ") + QString::number(dn2_max)
		+ QStringLiteral("-���ֵ�����") + QString::number(m_mtf_array)
		//+ QStringLiteral("-����λ��") + m_focal_plane
		+ QStringLiteral("-��Ƶ��") + QString::number(m_mtf_sample)
		//+ QStringLiteral("-�ӳ�λ��") + QString::number(42)
		;

	static QString old_file_path = "";
	//���ֵ����ʱ�ɼ���ʱ��ͼ�񣬸��Ǵ洢
	if (mtf_max_updated)
	{
		ui.lineEdit_save_info->setText(save_info);

		//ɾ�����ļ�
		if (old_file_path != "") {
			QFile old_file(old_file_path);
			old_file.remove();
		}

		QString path = ui.lineEdit_folder->text() + "/" + save_info + ".raw";

		saveParam.strSavePath = path;
		DataAnalysisModel* m_pDataAnalysisModel = IBussiness::getInstance().get_pDataAnalysisModel();
		m_pDataAnalysisModel->SaveModelData(saveParam);

		old_file_path = path;
		mtf_max_updated = false;
	}
}

void MTF::on_pushButton_folder_clicked()
{
	QString m_folder = QFileDialog::getExistingDirectory(this, QStringLiteral("�򿪴洢�ļ���"),
		ui.lineEdit_folder->text(),
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);
	ui.lineEdit_folder->setText(m_folder);
}

void MTF::on_pushButton_startCal_clicked()
{
	//һ�����5��
	timer.start(200);

	m_start_col = ui.lineEdit_start_col->text().toInt();
	m_end_col = ui.lineEdit_end_col->text().toInt();
	start_row = ui.lineEdit_start_row->text().toInt();
	m_cal_row = ui.lineEdit_end_row->text().toInt() - start_row;

	m_mtf_array = ui.spinBox_mtf_array->value();
	m_mtf_to_mtf = ui.lineEdit_mtf_to_mtf->text().toInt();
	m_target_th = ui.lineEdit_target_th->text().toInt();
	m_mtf_count = ui.lineEdit_mtf_count->text().toInt();
	m_mtf_sample = ui.lineEdit_mtf_sample->text().toInt();
	//m_focal_plane = ui.lineEdit_focal_plane->text();
	bottom_noise = ui.lineEdit_bottom_noise->text().toInt();

	mtf_max = 0;
	array_max = 0;
	position_max = 0;

	init_MTF_table();
}

void MTF::on_pushButton_stopCal_clicked()
{
	timer.stop();
}

void MTF::on_pushButton_Zero_clicked()
{
	//ui.tableWidget_mtf->clearContents();
	QTableWidget* table = ui.tableWidget_mtf;
	//���±�
	for (int i = 0; i < m_mtf_array; i++) {
		table->item(0, i)->setText("0");
		table->item(1, i)->setText("0,0");
		table->item(2, i)->setText("0,0");
	}
	table->viewport()->update();
}


void MTF::on_pushButton_save_clicked() {
	if (!b_auto_save) {
		if (ui.lineEdit_folder->text().isEmpty())
		{
			QMessageBox::warning(NULL, QStringLiteral("��ʾ"), QStringLiteral("δѡ��洢λ��"), QMessageBox::Ok);
			return;
		}
		QDir dir(ui.lineEdit_folder->text());
		if (!dir.exists()) {
			dir.mkdir(ui.lineEdit_folder->text());
		}
		b_auto_save = true;
		ui.pushButton_save->setText(QStringLiteral("ֹͣ��ͼ"));
		ui.lineEdit_folder->setReadOnly(true);
	}
	else {
		b_auto_save = false;
		ui.pushButton_save->setText(QStringLiteral("��ʼ��ͼ"));
		ui.lineEdit_folder->setReadOnly(false);
	}
}

