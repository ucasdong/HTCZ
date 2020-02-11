#ifndef ASSISTPICDATAMODEL_H
#define ASSISTPICDATAMODEL_H

#include <QAbstractTableModel>

enum SpectralType
{
	P,
	B
};

class AssistPicDataModel : public QAbstractTableModel
{
    //Q_OBJECT

public:
    AssistPicDataModel(QObject *parent = nullptr);

    void setRawData(unsigned char * pData);
    //void setHeaderList(QStringList hList);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
//    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
//                       int role = Qt::EditRole) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    //bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

private:
    unsigned char * m_pData;
	SpectralType m_Type;
	int m_nRowLength;

};

#endif // ASSISTPICDATAMODEL_H
