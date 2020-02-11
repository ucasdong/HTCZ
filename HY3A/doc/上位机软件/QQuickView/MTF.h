#pragma once

#include <QDialog>
#include "ui_MTF.h"

#include <QTimer>

#include "CommonHeader.h"

struct MtfDetail {
	double mtfvalue;
	int    count;
	int    dnvalue;
	int    dnvalue1;
	int    position;
	void* next;
	void* prev;
	void* tail;
	bool   update;
};

class MTF : public QDialog
{
	Q_OBJECT

public:
	MTF(QWidget *parent = Q_NULLPTR);
	~MTF();

	void init_MTF(QString DataSourceID_, QString DataSourceType_);
	QString sDataSourceType;
	QString sDataSourceID;
	void update_data_source();

	void update_MTF();

private:
	Ui::MTF ui;

	BYTE* data_source;//源数据

	int width;//图像宽度
	int height;//图像高度
	int bit_start;//起始比特
	int bit_length;//有效比特长度
	bool Data_EndianFlag;//True大端，False小端
	void set_data_type(int width_, int height_, bool Data_EndianFlag = true);
	void set_data_type(int width_, int height_, int bit_start_, int bit_length_, bool Data_EndianFlag = true);

	unsigned short* data16;//16位图像数据
	//BYTE* data8;//8位图像数据
	void trans_data_source_to_data16();

	QTimer timer;//定时刷新Timer


private:
	int start_row;//计算起始行
	//int step_row;//行计算间隔
	int	m_mtf_seek_pos;//阈值偏移位置
	int bottom_noise;//底噪

	int					m_start_col;		//起始列
	int					m_end_col;			//终止列
	int					m_mtf_array;		//计算组数
	int					m_cal_row;			//计算行数
	int					m_mtf_count;		//每组靶标线对
	int					m_mtf_to_mtf;		//靶标间距
	int					m_target_th;		//目标门限
	int					m_mtf_sample;		//计算倍频
	double				m_mtf_th;			//传函门限

	unsigned long*		m_average_buffer;//平均值
	unsigned long long	m_range_aver_buffer;
	MtfDetail			m_mtf_value_max[100];
	MtfDetail			m_mtf_value_count[100];

	double mtf_max;
	//最大值变化，存储数据
	bool mtf_max_updated = false;
	int dn1_max;
	int dn2_max;
	int array_max;
	int position_max;

private:
	void init_MTF_table();

	void cal_average();
	void cal_range_aver();
	void cal_MTF();
	void update_table();

	SaveParams saveParam;
	bool b_auto_save = false;//是否开启自动存图
	void save_max_data();



public slots:
	void on_pushButton_save_clicked();
	void on_pushButton_folder_clicked();

	void on_pushButton_startCal_clicked();
	void on_pushButton_stopCal_clicked();
	void on_pushButton_Zero_clicked();
};
