#pragma once

#include <QAbstractTableModel>
#include "Common.h"
#include <QMutex>

class TableStringModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	TableStringModel();
	~TableStringModel();

	void setHorizontalHeaderList(QStringList horizontalHeaderList);

	void setVerticalHeaderList(QStringList verticalHeaderList);

	int rowCount(const QModelIndex &parent) const;

	int columnCount(const QModelIndex &parent) const;

	QVariant data(const QModelIndex &index, int role) const;

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;

	void setModalDatas(std::vector<std::pair<QString, QString>>* allDataTemp);

	void refrushModel();

	void setDataShowFlag(int iFlag);                                 //������ʾ����(ʮ���ƻ���ʮ������)
	
	QMutex m_mutex;
private:
	QStringList horizontal_header_list;
	QStringList vertical_header_list;
	std::vector<std::pair<QString, QString>> vecAllData;

	int iRowCount;
	int iColumnnCount;
	int iDataTypeFlag;       //������ʾ����(ʮ���ƻ���ʮ������)
	

signals:
	void updateCount(int);
};
