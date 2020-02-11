#pragma once

#include <QTableView>
#include "TableModel.h"
#include <QLabel>
#include <QMouseEvent>

class TableView : public QTableView
{
	Q_OBJECT

public:
	TableView(int iColumn, int iRow);
	~TableView();

	int iRowCount;              //当前窗口行数
	int iColumnCount;           //当前窗口列数
	int iShowColumn;
	TableModel* tableModel;     //表格模型
	BYTE* graphicData;          //表格数据
	bool bPress;                //鼠标是否按压
	QLabel* toolTipLabel;       //点击提示框
	int iDataTypeFlag;          //数据显示类型(十进制或者十六进制)
	bool bShowTip;              //设置是否显示提示框（默认提示true）
	bool bChangeShow;

	void initHHeader();
	void initVHeader();
	void recvTableData(BYTE* dataShow);                         //表格显示
	void setDataShowFlag(int iFlag);                            //数据显示类型(十进制或者十六进制)
	void setMouseTip(bool bHave);                               //设置鼠标提示
	void changeShowWidth(int iWidth);                           

// protected:
// 	void mousePressEvent(QMouseEvent *event);
// 	void mouseMoveEvent(QMouseEvent *event);
// 	void mouseReleaseEvent(QMouseEvent *event);
};
