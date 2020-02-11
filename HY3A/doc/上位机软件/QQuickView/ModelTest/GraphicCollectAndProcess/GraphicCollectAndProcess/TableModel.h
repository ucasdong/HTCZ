#pragma once

#include <QAbstractTableModel>
#include "Common.h"

class TableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	TableModel();
	~TableModel();

	void setHorizontalHeaderList(QStringList horizontalHeaderList);

	void setVerticalHeaderList(QStringList verticalHeaderList);

	int rowCount(const QModelIndex &parent) const;

	int columnCount(const QModelIndex &parent) const;

	QVariant data(const QModelIndex &index, int role) const;

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;

	void setModalDatas(BYTE* dataTemp);

	void refrushModel();

	void setDataShowFlag(int iFlag);                                 //数据显示类型(十进制或者十六进制)
	void changeModel(int iModel);

private:
	QStringList horizontal_header_list;
	QStringList vertical_header_list;
	BYTE* showData;

	int iRowCount;
	int iColumnnCount;
	int iCurrentRow;
	int iCurrentColumn;
	int iCurrentModel;

	int iDataTypeFlag;       //数据显示类型(十进制或者十六进制)
signals:
	void updateCount(int);

public slots:
	void slot_CurrenetRow(int iRow);
	void slot_CurrentColumn(int iColumn);
};
