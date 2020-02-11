#include "QDisplayDataModel.h"
#include <QColor>
#include <QDebug>
#include <QBrush>
QDisplayDataModel::QDisplayDataModel(int nWidth, int nHeight, QObject *parent):
    QAbstractTableModel(parent),
	m_nWidth(nWidth),
	m_nHeight(nHeight),
	m_pData(nullptr)
{
    m_nStartBit = 0;
	m_dec = Hex;
}

void QDisplayDataModel::setRawData(unsigned char *pData)
{
    //beginResetModel();
    m_pData = pData;
    //endResetModel();
}

int QDisplayDataModel::rowCount(const QModelIndex &parent) const
{
    return m_nHeight;
}

int QDisplayDataModel::columnCount(const QModelIndex &parent) const
{
    return m_nWidth;
}

//QVariant PicDataModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    if (role != Qt::DisplayRole)
//        return QVariant();
//    return section;
//}

//bool PicDataModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
//{
//    return true;
//}

QVariant QDisplayDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::TextAlignmentRole)
    {
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    if (role == Qt::DisplayRole && m_pData != nullptr)
    {
		return QString("%1").arg((int)(m_pData[(index.row() * m_nWidth + index.column()) * 2] << 8) + (int)(m_pData[(index.row() * m_nWidth + index.column()) * 2 + 1]), 4, m_dec, QLatin1Char('0'));
    }

	if (role == Qt::BackgroundRole)
	{
		for (int i = 0; i < m_ColorConfig.size(); i++)
		{
			if (index.row() >= m_ColorConfig.at(i)["Row-Min"].toInt()-1 && index.row() <= m_ColorConfig.at(i)["Row-Max"].toInt()-1
				&& index.column() >= m_ColorConfig.at(i)["Col-Min"].toInt()-1 && index.column() <= m_ColorConfig.at(i)["Col-Max"].toInt()-1)
			{
				return QColor(m_ColorConfig.at(i)["Color"].toString());
			}
		}
		return QColor("#304458");
	}

    //if(role == Qt::ForegroundRole)
    //{
    //    return QVariant(QColor(Qt::white));
    //}

    return QVariant();

}

void QDisplayDataModel::onSetStartBit(int nBit)
{
    beginResetModel();
    m_nStartBit = nBit;
    endResetModel();
}

void QDisplayDataModel::setColorConfig(QJsonArray qArray)
{
	m_ColorConfig = qArray;
}

QVariant QDisplayDataModel::getColorFromConfig(int row, int col)const
{
	for (int i = 0; i < m_ColorConfig.size(); i++)
	{
		if (row >= m_ColorConfig.at(i)["Row-Min"].toInt() && row <= m_ColorConfig.at(i)["Row-Max"].toInt()
			&& col >= m_ColorConfig.at(i)["col-Min"].toInt() && row <= m_ColorConfig.at(i)["Row-Max"].toInt())
		{
			qDebug() << m_ColorConfig;
			int r = m_ColorConfig.at(i)["Color"].toString().left(2).toInt(nullptr, 16);
			int g = m_ColorConfig.at(i)["Color"].toString().mid(2, 2).toInt(nullptr, 16);
			int b = m_ColorConfig.at(i)["Color"].toString().right(2).toInt(nullptr, 16);
			return QVariant(QColor(r, g, b));
		}
	}

	return QVariant();
}
