#include "TableView.h"
#include <QHeaderView>
#include <QScrollBar>

TableView::TableView(int iColumn, int iRow)
{
	iRowCount = iRow;
	iColumnCount = iColumn;
	bShowTip = true;
	iDataTypeFlag = 1;

	this->setAlternatingRowColors(true);
	this->setStyleSheet("QTableView{background-color: rgb(250, 250, 115);"
		"alternate-background-color: rgb(141, 163, 215);}");
	this->horizontalHeader()->setStretchLastSection(true);
	this->horizontalHeader()->setStyleSheet("background-color:rgb(255,255,0)");
	this->horizontalHeader()->setHighlightSections(false);
	//this->verticalHeader()->setVisible(false);
	this->setShowGrid(true);//设置网格线
	this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionMode(QAbstractItemView::NoSelection);
	//	this->setSelectionBehavior(QAbstractItemView::SelectRows);

	graphicData = NULL;
	graphicData = new BYTE[iRowCount * iColumnCount * PIXLENGTH];
	memset(graphicData, 0x00, iRowCount * iColumnCount * PIXLENGTH);

	tableModel = new TableModel();
	this->setModel(tableModel);
	this->initHHeader();
	this->initVHeader();

	tableModel->setModalDatas(graphicData);

	toolTipLabel = new QLabel(this);
	toolTipLabel->setFixedSize(80, 20);
	toolTipLabel->setAlignment(Qt::AlignCenter);
	toolTipLabel->hide();
	connect(this->horizontalScrollBar(), SIGNAL(valueChanged(int)), tableModel, SLOT(slot_CurrentColumn(int)));
	connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), tableModel, SLOT(slot_CurrenetRow(int)));
}

TableView::~TableView()
{
	if (graphicData != NULL)
	{
		delete[] graphicData;
		graphicData = NULL;
	}
}

void TableView::initHHeader()
{
	QStringList header;
	for (int i = 1; i <= iColumnCount; i++)
	{
		header << QString::number(i);
	}
	tableModel->setHorizontalHeaderList(header);
}

void TableView::initVHeader()
{
	QStringList header;
	for (int i = 1; i <= iRowCount; i++)
	{
		header << QString::number(i);
	}

	tableModel->setVerticalHeaderList(header);
}

void TableView::changeValue(BYTE* dataShow)
{
	tableModel->setModalDatas(dataShow);
}

//多光谱表格显示
void TableView::recvTableData(BYTE* dataShow)
{
	//memcpy(graphicData, dataShow, iRowCount * iColumnCount * DGP_PIXLENNTH);
	tableModel->setModalDatas(dataShow);
	//tableModel->refrushModel();
	//this->reset();
	this->viewport()->update();
}

void TableView::setDataShowFlag(int iFlag)
{
	iDataTypeFlag = iFlag;
	tableModel->setDataShowFlag(iFlag);
}

void TableView::setMouseTip(bool bHave)
{
	bShowTip = bHave;
}

void TableView::changeModel(int iModel)
{
	tableModel->changeModel(iModel);
}
/*
void TableView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton || !bShowTip)
	{
		return;
	}
	bPress = true;

	int iX = event->pos().x();
	int iY = event->pos().y();
	QModelIndex index = indexAt(event->pos());
	int iRow = index.row();
	int iColumn = index.column();

	if (iRow < 0 || iColumn < 0)
	{
		return;
	}
	QString strShow = "";
	if (iDataTypeFlag == 1)
	{
		QString strData = QString("%1").arg(*(graphicData + iRow * iColumnCount + iColumn), 2, 16, QLatin1Char('0'));//数值
		strShow = strData + QString("(") + QString::number(iColumn + 1) + QString(",") + QString::number(iRow + 1) + QString(")");
		
	}
	else if(iDataTypeFlag == 0)
	{
		QString strData = QString("%1").arg(*(graphicData + iRow * iColumnCount + iColumn), 2, 10, QLatin1Char('0'));//数值
		strShow = strData + QString("(") + QString::number(iColumn + 1) + QString(",") + QString::number(iRow + 1) + QString(")");
	}
	toolTipLabel->setText(strShow);
	if ((iX > this->width() - 80) || iY > this->height() - 20)
	{
		toolTipLabel->move(iX - toolTipLabel->width() - 5, iY - toolTipLabel->height() - 5);
	}
	else
	{
		toolTipLabel->move(iX + 45, iY + 25);
	}
	toolTipLabel->show();
}

void TableView::mouseMoveEvent(QMouseEvent *event)
{
	if (!bPress || !bShowTip)
	{
		return;
	}
	else
	{
		int iX = event->pos().x();
		int iY = event->pos().y();
		QModelIndex index = indexAt(event->pos());
		int iRow = index.row();
		int iColumn = index.column();
		if (iRow < 0 || iColumn < 0)
		{
			return;
		}
		if (iDataTypeFlag == 1)
		{
			QString strData = QString("%1").arg(*(graphicData + iRow * iColumnCount + iColumn), 2, 16, QLatin1Char('0'));//数值
			QString strShow = strData + QString("(") + QString::number(iColumn + 1) + QString(",") + QString::number(iRow + 1) + QString(")");
			toolTipLabel->setText(strShow);
		}
		else if (iDataTypeFlag == 0)
		{
			QString strData = QString("%1").arg(*(graphicData + iRow * iColumnCount + iColumn), 2, 10, QLatin1Char('0'));//数值
			QString strShow = strData + QString("(") + QString::number(iColumn + 1) + QString(",") + QString::number(iRow + 1) + QString(")");
			toolTipLabel->setText(strShow);
		}
		if ((iX > this->width() - 80) || iY > this->height() - 20)
		{
			toolTipLabel->move(iX - toolTipLabel->width() - 5, iY - toolTipLabel->height() - 5);
		}
		else
		{
			toolTipLabel->move(iX + 45, iY + 25);
		}
	}
}

void TableView::mouseReleaseEvent(QMouseEvent *event)
{
	toolTipLabel->hide();
	bPress = false;
}
*/