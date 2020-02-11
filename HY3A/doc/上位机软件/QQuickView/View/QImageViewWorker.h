#pragma once

#include <QObject>
#include <QImage>
#include <QPixmap>

class QImageViewWorker : public QObject
{
	Q_OBJECT
public:
	QImageViewWorker(int nDataSize);

public slots:
	void recvData(unsigned char* pData, int nWidth, int nHeight, int nStartBit);
signals:
	void captureAPic(QPixmap);
public:
	unsigned char * m_pPicData;
};