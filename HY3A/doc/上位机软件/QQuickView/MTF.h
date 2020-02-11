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

	BYTE* data_source;//Դ����

	int width;//ͼ����
	int height;//ͼ��߶�
	int bit_start;//��ʼ����
	int bit_length;//��Ч���س���
	bool Data_EndianFlag;//True��ˣ�FalseС��
	void set_data_type(int width_, int height_, bool Data_EndianFlag = true);
	void set_data_type(int width_, int height_, int bit_start_, int bit_length_, bool Data_EndianFlag = true);

	unsigned short* data16;//16λͼ������
	//BYTE* data8;//8λͼ������
	void trans_data_source_to_data16();

	QTimer timer;//��ʱˢ��Timer


private:
	int start_row;//������ʼ��
	//int step_row;//�м�����
	int	m_mtf_seek_pos;//��ֵƫ��λ��
	int bottom_noise;//����

	int					m_start_col;		//��ʼ��
	int					m_end_col;			//��ֹ��
	int					m_mtf_array;		//��������
	int					m_cal_row;			//��������
	int					m_mtf_count;		//ÿ��б��߶�
	int					m_mtf_to_mtf;		//�б���
	int					m_target_th;		//Ŀ������
	int					m_mtf_sample;		//���㱶Ƶ
	double				m_mtf_th;			//��������

	unsigned long*		m_average_buffer;//ƽ��ֵ
	unsigned long long	m_range_aver_buffer;
	MtfDetail			m_mtf_value_max[100];
	MtfDetail			m_mtf_value_count[100];

	double mtf_max;
	//���ֵ�仯���洢����
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
	bool b_auto_save = false;//�Ƿ����Զ���ͼ
	void save_max_data();



public slots:
	void on_pushButton_save_clicked();
	void on_pushButton_folder_clicked();

	void on_pushButton_startCal_clicked();
	void on_pushButton_stopCal_clicked();
	void on_pushButton_Zero_clicked();
};
