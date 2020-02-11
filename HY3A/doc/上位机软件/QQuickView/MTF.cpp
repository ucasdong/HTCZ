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
	width = width_;//图像宽度
	height = height_;//图像高度
	bit_start = 0;//起始比特
	bit_length = 16;//有效比特长度
	Data_EndianFlag = Data_EndianFlag;//True大端，False小端 
}

void MTF::set_data_type(int width_, int height_, int bit_start_, int bit_length_, bool Data_EndianFlag)
{
	width = width_;//图像宽度
	height = height_;//图像高度
	bit_start = bit_start_;//起始比特
	bit_length = bit_length_;//有效比特长度
	Data_EndianFlag = Data_EndianFlag;//True大端，False小端
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
		QString str = QStringLiteral("第") + QString::number(i + 1) + QStringLiteral("组");
		headers_horizon.append(str);
	}
	table->setHorizontalHeaderLabels(headers_horizon);

	QStringList headers_vertical;
	headers_vertical.append(QStringLiteral("最大值"));
	headers_vertical.append(QStringLiteral("dnvalue"));
	headers_vertical.append(QStringLiteral("位置"));
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
//计算矩形区域内每一列的均值
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
//计算矩形区域内的均值
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
		QMessageBox::critical(this, "ERROR", QStringLiteral("区域设置有误,请重新设置！"));
		QLOG_ERROR() << QStringLiteral("MTF 区域设置有误,请重新设置！");
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
	//按输入的计算组数计算N次
	for (int i = 0; i < m_mtf_array; i++)
	{
		////找到第一个不小于目标门限的像元均值（如若像元均值全部小于目标门限则不予计算），保存该像元下标值StartIndex
		//value2 = m_average_buffer[start];
		//while (value2 < m_target_th)
		//{
		//	start++;
		//	value2 = m_average_buffer[start];
		//	if (start >= m_end_col)
		//		return;
		//}
		////以StartIndex为起始点、StartIndex加上输入的靶标间距值为结束点（若结束点大于图像宽度则不予计算），在此范围内找到最后一个大于目标门限的像元均值
		//if (start + m_mtf_to_mtf > m_end_col) {
		//	return;
		//}
		//for (int j = start; j < start + m_mtf_to_mtf; j++)
		//{
		//	value2 = m_average_buffer[j];
		//	//其下标值记为EndIndex
		//	if (value2 > m_target_th)
		//		end = j;
		//	//若查找过程中EndIndex与StartIndex的差值大于靶标线对* 计算倍频，则不予计算
		//	if ((j - start) > m_mtf_count * m_mtf_sample)
		//		break;
		//}
		//遍历StartIndex---EndIndex范围内的像元均值并计算MTF值，计算流程如下：
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
			//记录当前计算组数中最大的MTF值
			if (m_mtf_value_max[i].mtfvalue < value.mtfvalue)
			{
				memcpy(&m_mtf_value_max[i], &value, sizeof(MtfDetail));
				m_mtf_value_max[i].update = true;
			}
			//遍历过程中如果取到的像元下标 - StartIndex < 每组靶标限对 则
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
	//更新表
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
	//传函最大值mtf_max，最大值所在的组数mtf_array_max，传函最大时对应的dn值和位置
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
	//采图信息
	QString save_info =
		//QStringLiteral("谱段数") + QString::number(42)
		this->windowTitle()
		+ QStringLiteral("-最大传函值") + QString::number(mtf_max)
		+ QStringLiteral("-最大DN值") + QString::number(dn1_max)
		+ QStringLiteral("-最小DN值") + QString::number(dn2_max)
		+ QStringLiteral("-出现的组数") + QString::number(m_mtf_array)
		//+ QStringLiteral("-焦面位置") + m_focal_plane
		+ QStringLiteral("-倍频数") + QString::number(m_mtf_sample)
		//+ QStringLiteral("-视场位置") + QString::number(42)
		;

	static QString old_file_path = "";
	//最大值出现时采集该时刻图像，覆盖存储
	if (mtf_max_updated)
	{
		ui.lineEdit_save_info->setText(save_info);

		//删除旧文件
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
	QString m_folder = QFileDialog::getExistingDirectory(this, QStringLiteral("打开存储文件夹"),
		ui.lineEdit_folder->text(),
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);
	ui.lineEdit_folder->setText(m_folder);
}

void MTF::on_pushButton_startCal_clicked()
{
	//一秒计算5次
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
	//更新表
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
			QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("未选择存储位置"), QMessageBox::Ok);
			return;
		}
		QDir dir(ui.lineEdit_folder->text());
		if (!dir.exists()) {
			dir.mkdir(ui.lineEdit_folder->text());
		}
		b_auto_save = true;
		ui.pushButton_save->setText(QStringLiteral("停止存图"));
		ui.lineEdit_folder->setReadOnly(true);
	}
	else {
		b_auto_save = false;
		ui.pushButton_save->setText(QStringLiteral("开始存图"));
		ui.lineEdit_folder->setReadOnly(false);
	}
}

