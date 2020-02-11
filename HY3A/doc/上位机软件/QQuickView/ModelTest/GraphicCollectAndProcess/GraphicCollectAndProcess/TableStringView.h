#pragma once

#include <QTableView>
#include "TableStringModel.h"
#include <QLabel>
#include <QMouseEvent>

class TableStringView : public QTableView
{
	Q_OBJECT

public:
	TableStringView(int iColumn, int iRow);
	~TableStringView();

	int iRowCount;                //当前窗口行数
	int iColumnCount;             //当前窗口列数
	TableStringModel* tableModel; //表格模型
	std::vector<std::pair<QString, QString>> vecAllDataView;//表格数据

	void initHHeader(QStringList strHeadlist);
	void initVHeader(QStringList strHeadlist);
	void changeValue(std::vector<std::pair<QString, QString>>* vecData);
};
