#include "TableStringModel.h"

TableStringModel::TableStringModel()
{
	iRowCount = 0;
	iColumnnCount = 0;
	iDataTypeFlag = 1;
}

TableStringModel::~TableStringModel()
{
}

void TableStringModel::setHorizontalHeaderList(QStringList horizontalHeaderList)
{
	horizontal_header_list = horizontalHeaderList;
	iColumnnCount = horizontal_header_list.size();
}

void TableStringModel::setVerticalHeaderList(QStringList verticalHeaderList)
{
	vertical_header_list = verticalHeaderList;
	iRowCount = vertical_header_list.size();
}

int TableStringModel::rowCount(const QModelIndex &parent) const
{
	return iRowCount;
}

int TableStringModel::columnCount(const QModelIndex &parent) const
{
	return iColumnnCount;
}

QVariant TableStringModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::TextAlignmentRole)
	{
		return int(Qt::AlignLeft | Qt::AlignVCenter);
	}
	else if (role == Qt::DisplayRole)
	{
		if (vecAllData.size() <= 0 || index.column() > 1 || index.row() >= vecAllData.size() || index.column() >= 2)
		{
			return QVariant();
		}
		QString strShow = "";
		
		if (index.column() == 0)
		{
			std::pair<QString, QString> it = vecAllData.at(index.row());
			strShow = it.first;
		}
		else
		{
			std::pair<QString, QString> it = vecAllData.at(index.row());
			strShow = it.second;
		}
		return strShow;
	}
	return QVariant();
}

QVariant TableStringModel::headerData(int section, Qt::Orientation orientation, int role) const
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

Qt::ItemFlags TableStringModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	Qt::ItemFlags flag = QAbstractItemModel::flags(index);

	// flag|=Qt::ItemIsEditable // 设置单元格可编辑,此处注释,单元格无法被编辑  
	return flag;
}

//重设model数据
void TableStringModel::setModalDatas(std::vector<std::pair<QString, QString>>* allDataTemp)
{
	//mutex.lock();
	vecAllData = *allDataTemp;
	//mutex.unlock();
	refrushModel();
}

void TableStringModel::refrushModel()
{
	//beginResetModel();
	endResetModel();
}

void TableStringModel::setDataShowFlag(int iFlag)
{
	iDataTypeFlag = iFlag;
}

