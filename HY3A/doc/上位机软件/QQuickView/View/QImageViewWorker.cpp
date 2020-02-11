#include "QImageViewWorker.h"
#include <QDebug>
#include <stdio.h>
#include <windows.h>
#include "QsLog.h"

QImageViewWorker::QImageViewWorker(int nDataSize)
	:m_pPicData(new unsigned char[nDataSize])
{

}

void QImageViewWorker::recvData(unsigned char* pData, int nWidth, int nHeight, int nStartBit)
{
	//QLOG_INFO() << QStringLiteral("captureAPic 1");

	for (int i = 0; i < nWidth * nHeight * 2; i = i + 2)
	{
		unsigned short stValue = (pData[i] << 8) + pData[i + 1];
		unsigned char  nValue = (unsigned char )((stValue >> (8 - nStartBit)) & 0xff);
		m_pPicData[i / 2] = nValue;
	}
	QImage image(m_pPicData, nWidth, nHeight, nWidth, QImage::Format_Grayscale8);
	//QLOG_INFO() << QStringLiteral("captureAPic 2");
	emit captureAPic(QPixmap::fromImage(image, Qt::NoFormatConversion));
	//QLOG_INFO() << QStringLiteral("captureAPic 3");

	//qDebug()<<"QImageViewWorker::append"<<GetCurrentThreadId();
}
