#include "TableModel.h"
#include <QMutex>

QMutex mutex;

TableModel::TableModel()
{
	iRowCount = 0;
	iColumnnCount = 0;
	iDataTypeFlag = 1;
	iCurrentRow = 0;
	iCurrentColumn = 0;
	showData = NULL;
}

TableModel::~TableModel()
{
}

void TableModel::setHorizontalHeaderList(QStringList horizontalHeaderList)
{
	horizontal_header_list = horizontalHeaderList;
	iColumnnCount = horizontal_header_list.size();
}

void TableModel::setVerticalHeaderList(QStringList verticalHeaderList)
{
	vertical_header_list = verticalHeaderList;
	iRowCount = vertical_header_list.size();
}

int TableModel::rowCount(const QModelIndex &parent) const
{
	return iRowCount;
}

int TableModel::columnCount(const QModelIndex &parent) const
{
	return iColumnnCount;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (NULL == showData)
		return QVariant();
	
	if (index.row() < (iCurrentRow - 2) || index.row() > (iCurrentRow + 45) || index.column() < (iCurrentColumn - 2) || index.column() > (iCurrentColumn + 25))
	{
		return QVariant();
	}

	if (role == Qt::TextAlignmentRole)
	{
		return int(Qt::AlignLeft | Qt::AlignVCenter);
	}
	else if (role == Qt::DisplayRole)
	{
// 		if (index.row() >= arr_row_list->size())
// 			return QVariant();
// 		if (index.column() >= arr_row_list->at(0).size())
// 			return QVariant();
//		return arr_row_list->at(index.row()).at(index.column());
		//mutex.lock();
		BYTE* dataPtr = showData + (iColumnnCount * index.row() + index.column()) * 2;
		unsigned int dataShow = *(dataPtr + 0) * 256 + *(dataPtr + 1);
		//BYTE byteData = *(showData + iColumnnCount * index.row() * DGP_PIXLENNTH + index.column());
		
		//mutex.unlock();
		//return QString(byteData, 16).toUpper();
		if (dataShow >= 0)
		{
			if (iDataTypeFlag == 1)
			{
				return  QString("%1").arg(dataShow, 4, 16, QLatin1Char('0'));
			}
			else if(iDataTypeFlag == 0)
			{
				return  QString("%1").arg(dataShow, 4, 10, QLatin1Char('0'));
			}
		}
	}
	return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal) // 水平表头  
		{
			if (horizontal_header_list.size() > section)
				return horizontal_header_list[section];
			else
				return QVariant();
		}
		else
		{
			if (vertical_header_list.size() > section)
				return vertical_header_list[section]; // 垂直表头  
			else
				return QVariant();
		}
	}
	return QVariant();
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	Qt::ItemFlags flag = QAbstractItemModel::flags(index);

	// flag|=Qt::ItemIsEditable // 设置单元格可编辑,此处注释,单元格无法被编辑  
	return flag;
}

void TableModel::setModalDatas(BYTE* dataTemp)
{
	showData = dataTemp;
	/*
	if (showData == NULL)
	{
		showData = new BYTE[iRowCount * iColumnnCount * DGP_PIXLENNTH];
		memset(showData, 0, iRowCount * iColumnnCount * DGP_PIXLENNTH);
	}
	if (dataTemp != NULL)
	{
		//mutex.lock();
		showData = dataTemp;
		//memcpy(showData, dataTemp, iRowCount * iColumnnCount);
		//mutex.unlock();
	}
	//refrushModel();
	*/
}

void TableModel::refrushModel()
{
	beginResetModel();
	endResetModel();
}

void TableModel::setDataShowFlag(int iFlag)
{
	iDataTypeFlag = iFlag;
}

void TableModel::slot_CurrenetRow(int iRow)
{
	iCurrentRow = iRow;
}

void TableModel::slot_CurrentColumn(int iColumn)
{
	iCurrentColumn = iColumn;
}

