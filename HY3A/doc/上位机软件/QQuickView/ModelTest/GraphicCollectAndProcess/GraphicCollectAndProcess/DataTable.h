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

	void initWindows(std::map<QString, QSize>* cameraToFzSize);//初始化窗口
	void updateData(QString strCmeraType, BYTE* dataTemp);     //更新数据

private:
//	void readFZAnalysisFile();                                 //解析辅助数据文件
	void updateShowVecData(QString strName, std::vector<std::pair<QString, QString>>& vecData, BYTE* dataTemp); //更新将表格中的数据

private:
	Ui::DataTable ui;

	int iFzSataShowTime;                                   //控制辅助数据显示频率
	QPoint m_pressPos;                                     //窗口移动鼠标位置
	bool bPress;                                           //窗口移动鼠标是否按下
	//CodeDataAnalysis codeAnalysis;                         //解析文件
	std::map<QString, CodeDataAnalysis*> mapCametraToAnaly;//相机类型对应的解析对象
	std::map<QString, TableStringView*> mapCametraToTable; //相机类型对应的辅助数据表格
	std::map<QString, BYTE*> mapCameraToFZdata;            //相机类型对应的辅助数据
	std::map<QString, QSize>* cameraTypeToFZsize;          //相机类型对应辅助数据长度
	std::map<QString, std::vector<std::pair<QString, QString>>> mapShowData;  //表格将要显示的数据

public slots:
	void on_closeButton_clicked();
	void on_minButton_clicked();
	void on_maxButton_clicked();

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};
