#include "AssistPicDataModel.h"
#include <QColor>

QStringList headList = QStringList()<<QStringLiteral("行头")<<QStringLiteral("片谱")<<QStringLiteral("行计数")<<QStringLiteral("帧号")<<QStringLiteral("时间计数")
                                   <<"b13"<<"b14"<<"b15"<<"b16"<<"b17"<<"b18"<<"b19"<<"b20"<<"b21"<<"b22"<<"b23"<<"b24"<<"b25"<<"b26"<<"b27"<<"b28"<<"b29"<<"b30"<<"b31" << "b32" << "b33" << "b34" << "b35" << "b36" << "b37" << "b38" << "b39"
                                  <<QStringLiteral("暗像元(32-47)")<<"Sss"<<"sds";
AssistPicDataModel::AssistPicDataModel(QObject *parent):
    QAbstractTableModel(parent)
{
    m_pData = nullptr;
}

void AssistPicDataModel::setRawData(unsigned char *pData)
{
    beginResetModel();
    m_pData = pData;
    endResetModel();
}

int AssistPicDataModel::rowCount(const QModelIndex &parent) const
{
    return 32;
}

int AssistPicDataModel::columnCount(const QModelIndex &parent) const
{
    return 33;
}

QVariant AssistPicDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
    {
        return QVariant();
    }
    if (orientation == Qt::Horizontal)
    {
        //if(headList.size() <= section)
        //    return QVariant();
        return headList.at(section);
    }
    else
    {
        return section + 1;
    }
    return QVariant();
}

//bool PicDataModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
//{
//    return true;
//}

QVariant AssistPicDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::TextAlignmentRole)
    {
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    if (role == Qt::DisplayRole && m_pData!= nullptr)
    {
        QString sData;
        switch (index.column() ) {
        case 0:
        {
            int nData = (int)m_pData[(index.row() * m_nRowLength + 0)*2]*256+(int)m_pData[(index.row() * m_nRowLength + 0)*2 +1];
            sData = QString("%1").arg(QString::number(nData, 16).toUpper(), 2,  QLatin1Char('0'));
            nData = (int)m_pData[(index.row() * m_nRowLength + 1)*2]*256+(int)m_pData[(index.row() * m_nRowLength + 1)*2 +1];
            sData += QString("%1").arg(QString::number(nData, 16).toUpper(), 2,  QLatin1Char('0'));
            nData = (int)m_pData[(index.row() * m_nRowLength + 2)*2]*256+(int)m_pData[(index.row() * m_nRowLength + 2)*2 +1];
            sData += QString("%1").arg(QString::number(nData, 16).toUpper(), 2,  QLatin1Char('0'));
            break;
        }
        case 1:
        {
            int nData = (int)m_pData[(index.row() * m_nRowLength + 3)*2]*256+(int)m_pData[(index.row() * m_nRowLength + 3)*2 +1];
            sData = QString("%1").arg(QString::number(nData, 16).toUpper(), 2,  QLatin1Char('0'));
            break;
        }
        case 2:
        {
            int nData = (int)m_pData[(index.row() * m_nRowLength + 4)*2]*256+(int)m_pData[(index.row() * m_nRowLength + 4)*2 +1];
            sData = QString("%1").arg(QString::number(nData, 16).toUpper(), 2,  QLatin1Char('0'));
            nData = (int)m_pData[(index.row() * m_nRowLength + 5)*2]*256+(int)m_pData[(index.row() * m_nRowLength + 5)*2 +1];
            sData += QString("%1").arg(QString::number(nData, 16).toUpper(), 2,  QLatin1Char('0'));
            nData = (int)m_pData[(index.row() * m_nRowLength + 6)*2]*256+(int)m_pData[(index.row() * m_nRowLength + 6)*2 +1];
            sData += QString("%1").arg(QString::number(nData, 16).toUpper(), 2,  QLatin1Char('0'));
            nData = (int)m_pData[(index.row() * m_nRowLength + 7)*2]*256+(int)m_pData[(index.row() * m_nRowLength + 7)*2 +1];
            sData += QString("%1").arg(QString::number(nData, 16).toUpper(), 2,  QLatin1Char('0'));
            break;
        }
        case 3:
        {
            int nData = (int)m_pData[(index.row() * m_nRowLength + 8)*2]*256+(int)m_pData[(index.row() * m_nRowLength + 8)*2 +1];
            sData = QString("%1").arg(QString::number(nData, 16).toUpper(), 2,  QLatin1Char('0'));
            break;
        }
        case 4:
        {
            int nData = (int)m_pData[(index.row() * m_nRowLength + 9)*2]*256+(int)m_pData[(index.row() * m_nRowLength + 9)*2 +1];
            sData = QString("%1").arg(QString::number(nData, 16).toUpper(), 2,  QLatin1Char('0'));
            nData = (int)m_pData[(index.row() * m_nRowLength + 10)*2]*256+(int)m_pData[(index.row() * m_nRowLength + 10)*2 +1];
            sData += QString("%1").arg(QString::number(nData, 16).toUpper(), 2,  QLatin1Char('0'));
            nData = (int)m_pData[(index.row() * m_nRowLength + 11)*2]*256+(int)m_pData[(index.row() * m_nRowLength + 11)*2 +1];
            sData += QString("%1").arg(QString::number(nData, 16).toUpper(), 2,  QLatin1Char('0'));
            nData = (int)m_pData[(index.row() * m_nRowLength + 12)*2]*256+(int)m_pData[(index.row() * m_nRowLength + 12)*2 +1];
            sData += QString("%1").arg(QString::number(nData, 16).toUpper(), 2,  QLatin1Char('0'));
            break;
        }
        case 24:
        {
            break;
        }
        default:
        {
            int nData = (int)m_pData[(index.row() * m_nRowLength + index.column()+8)*2]*256+(int)m_pData[(index.row() * m_nRowLength + index.column()+8)*2 +1];
            sData = QString("%1").arg(QString::number(nData, 16).toUpper(), 2,  QLatin1Char('0'));
            break;
        }

        }

        return sData;

    }

    if(role == Qt::ForegroundRole)
    {
        return QVariant(QColor(Qt::white));
    }

    if(role == Qt::BackgroundColorRole)
    {
        if((index.row() < 2 && index.column() > 4 && index.column() < 24) || (index.row() == 2 && index.column() > 4 && index.column() < 10))
            return QVariant(QColor(255,128,192));
        if((index.row() > 2 && index.row() < 18 && index.column() > 4 && index.column() < 24) || (index.row() == 2 && index.column() > 9 && index.column() < 24)
                || (index.row() == 18 && index.column() == 5))
            return QVariant(QColor(128,236,128));
        if((index.row() > 18 && index.row() < 24 && index.column() > 4 && index.column() < 24) || (index.row() == 18 && index.column() > 5 && index.column() < 24)
                || (index.row() == 24 && index.column() > 4 && index.column() < 7))
            return QVariant(QColor(Qt::cyan));
            //return QVariant(QColor(255,255,128));

        if(index.row() == 26 && index.column() > 4 && index.column() < 7)
            return QVariant(QColor(0,192,255));
        if(index.row() == 26 && index.column() == 7)
            return QVariant(QColor(200,200,255));
        if(index.row() == 26 && index.column() == 8)
            return QVariant(QColor(0,232,0));
        if(index.row() == 26 && index.column() == 9)
            return QVariant(QColor(255,128,192));
        if(index.row() == 26 && index.column() == 10)
            return QVariant(QColor(128,236,128));
        if(index.row() == 27 && index.column() > 4 && index.column() < 11)
            return QVariant(QColor(Qt::cyan));
            //return QVariant(QColor(255,255,128));

        if(index.row() == 29 && index.column() > 4 && index.column() < 10)
            return QVariant(QColor(0,192,255));
        if(index.row() == 29 && index.column() > 9 && index.column() < 15)
            return QVariant(QColor(200,200,255));
        if(index.row() == 29 && index.column() == 15)
            return QVariant(QColor(0,232,0));
        if(index.row() == 29 && index.column() == 16)
            return QVariant(QColor(255,128,192));

        if(index.row() == 30 && index.column() > 4 && index.column() < 15)
            return QVariant(QColor(Qt::cyan));
           //eturn QVariant(QColor(255,255,128));


        if(index.row() == 31 && index.column() > 4 && index.column() < 7)
            return QVariant(QColor(0,192,255));
        if(index.row() == 31 && index.column() > 6 && index.column() < 11)
            return QVariant(QColor(200,200,255));
        if(index.row() == 31 && index.column() > 10 && index.column() < 15)
            return QVariant(QColor(0,232,0));
        if(index.row() == 31 && index.column() > 14 && index.column() < 19)
            return QVariant(QColor(255,128,192));
        if(index.row() == 31 && index.column() > 18 && index.column() < 23)
            return QVariant(QColor(128,236,128));
    }
    return QVariant();

}
