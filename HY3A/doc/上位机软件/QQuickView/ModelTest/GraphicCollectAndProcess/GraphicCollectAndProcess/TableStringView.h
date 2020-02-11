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

	int iRowCount;                //��ǰ��������
	int iColumnCount;             //��ǰ��������
	TableStringModel* tableModel; //���ģ��
	std::vector<std::pair<QString, QString>> vecAllDataView;//�������

	void initHHeader(QStringList strHeadlist);
	void initVHeader(QStringList strHeadlist);
	void changeValue(std::vector<std::pair<QString, QString>>* vecData);
};
