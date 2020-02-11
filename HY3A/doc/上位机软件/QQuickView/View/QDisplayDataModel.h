#ifndef QDISPLAYDATAMODEL_H
#define QDISPLAYDATAMODEL_H

#include <QAbstractTableModel>
#include "CommonHeader.h"
#include <qjsonarray.h>
class QDisplayDataModel : public QAbstractTableModel
{
    //Q_OBJECT

public:
	QDisplayDataModel(int nWidth, int nHeight, QObject *parent = nullptr);

    void setRawData(unsigned char * pData);
    //void setHeaderList(QStringList hList);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    //QVariant headerData(int section, Qt::Orientation orientation,
	//                        int role = Qt::DisplayRole) const override;
//    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
//                       int role = Qt::EditRole) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    //bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

public:
    void onSetStartBit(int nBit);
	void setColorConfig(QJsonArray qArray);

private:
	QVariant getColorFromConfig(int row, int col) const;

private:
	int m_nWidth;
	int m_nHeight;
	QJsonArray m_ColorConfig;
    unsigned char * m_pData;
    int m_nStartBit;
	displayDec m_dec;

};

#endif // QDISPLAYDATAMODEL_H
