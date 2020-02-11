#include "TableStringView.h"
#include <QHeaderView>

TableStringView::TableStringView(int iColumn, int iRow)
{
	iRowCount = iRow;
	iColumnCount = iColumn;

	this->setAlternatingRowColors(true);
	this->setStyleSheet("QTableView{background-color: rgb(250, 250, 115);"
		"alternate-background-color: rgb(141, 163, 215);}");
	//this->horizontalHeader()->setStretchLastSection(true);
	this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	this->horizontalHeader()->setStyleSheet("background-color:rgb(255,255,0)");
	this->horizontalHeader()->setHighlightSections(false);
	//this->verticalHeader()->setVisible(false);
	this->setShowGrid(true);//ÉèÖÃÍø¸ñÏß
	this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionMode(QAbstractItemView::NoSelection);

	tableModel = new TableStringModel();
	this->setModel(tableModel);
}

TableStringView::~TableStringView()
{
// 	if (graphicData != NULL)
// 	{
// 		delete[] graphicData;
// 		graphicData = NULL;
// 	}
}

void TableStringView::initHHeader(QStringList strHeadlist)
{
	if (strHeadlist.size() <= 0)
	{
		for (int i = 1; i <= iColumnCount; i++)
		{
			strHeadlist << QString::number(i);
		}
	}
	tableModel->setHorizontalHeaderList(strHeadlist);
}

void TableStringView::initVHeader(QStringList strHeadlist)
{
	if (strHeadlist.size() <= 0)
	{
		for (int i = 1; i <= iRowCount; i++)
		{
			strHeadlist << QString::number(i);
		}
	}
	tableModel->setVerticalHeaderList(strHeadlist);
}

void TableStringView::changeValue(std::vector<std::pair<QString, QString>>* vecData)
{
	tableModel->setModalDatas(vecData);
}
