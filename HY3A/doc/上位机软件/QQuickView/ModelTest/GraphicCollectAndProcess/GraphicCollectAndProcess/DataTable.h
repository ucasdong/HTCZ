#pragma once

#include <QWidget>
#include "ui_DataTable.h"
#include <QTableWidget>
#include "TableStringView.h"
#include "CodeDataAnalysis.h"

class DataTable : public QWidget
{
	Q_OBJECT

public:
	DataTable(std::map<QString, QSize>* cameraToFzSize);
	~DataTable();

	void initWindows(std::map<QString, QSize>* cameraToFzSize);//��ʼ������
	void updateData(QString strCmeraType, BYTE* dataTemp);     //��������

private:
//	void readFZAnalysisFile();                                 //�������������ļ�
	void updateShowVecData(QString strName, std::vector<std::pair<QString, QString>>& vecData, BYTE* dataTemp); //���½�����е�����

private:
	Ui::DataTable ui;

	int iFzSataShowTime;                                   //���Ƹ���������ʾƵ��
	QPoint m_pressPos;                                     //�����ƶ����λ��
	bool bPress;                                           //�����ƶ�����Ƿ���
	//CodeDataAnalysis codeAnalysis;                         //�����ļ�
	std::map<QString, CodeDataAnalysis*> mapCametraToAnaly;//������Ͷ�Ӧ�Ľ�������
	std::map<QString, TableStringView*> mapCametraToTable; //������Ͷ�Ӧ�ĸ������ݱ��
	std::map<QString, BYTE*> mapCameraToFZdata;            //������Ͷ�Ӧ�ĸ�������
	std::map<QString, QSize>* cameraTypeToFZsize;          //������Ͷ�Ӧ�������ݳ���
	std::map<QString, std::vector<std::pair<QString, QString>>> mapShowData;  //���Ҫ��ʾ������

public slots:
	void on_closeButton_clicked();
	void on_minButton_clicked();
	void on_maxButton_clicked();

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};
