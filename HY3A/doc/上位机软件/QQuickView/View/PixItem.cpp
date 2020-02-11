#include "PixItem.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QDrag>
#include <math.h>

int i = 0;
int iPiece = 0;
int iArea = 0;

PixItem::PixItem(QSize WndSize, QGraphicsItem *parent)
    :QGraphicsItem(parent=0)
{
    setAcceptDrops(false);   //设置可拖拽
    graphicSize = WndSize;
    m_dataSize = graphicSize.width() * graphicSize.height() * 2;
    graphicData = new unsigned char[m_dataSize];
    memset(graphicData, 0x20, m_dataSize);
    m_imageData = new unsigned char[m_dataSize/2];
    memset(m_imageData, 0x20, m_dataSize/2);
    m_nStartBit = 2;
    //ui->tab_3->hideTopTools(false);


    m_pImage = new QImage(graphicData, graphicSize.width(), graphicSize.height(), QImage::Format_Grayscale8);
    //qDebug()<<m_pImage->depth();
    //QImage igScaled = image.scaled(graphicSize.width(), graphicSize.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);//按比例缩放
    setPixMap(QPixmap::fromImage(*m_pImage));
}

PixItem::~PixItem()
{
        delete[] graphicData;
        graphicData = NULL;
		delete[] m_imageData;

}

void PixItem::recieve(unsigned char *)
{

}

QRectF PixItem::boundingRect() const
{
    return QRectF(-pix.width() / 2, -pix.height() / 2, pix.width(), pix.height());
}

void PixItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	qDebug() << "mouseMoveEvent";
}

void PixItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //if (bShowGraph)
    {
        //mutex.lock();

        painter->drawPixmap(-pix.width() / 2, -pix.height() / 2, pix);

// 		QPen pen(QColor(255, 255, 255, 255));
// 		painter->setPen(pen);
// 		if (bPress)
// 		{
// 			painter->drawText(m_startPos, toolTipText);
// 		}
    }
}
void PixItem::setPixMap(QPixmap pixTemp)
{
    pix = pixTemp;
}

void PixItem::recvGraphicData(unsigned char* data)
{
    memcpy(graphicData, data, m_dataSize);
    for(int i = 0; i < m_dataSize;)
    {
        unsigned short stValue = graphicData[i] * 256 + graphicData[i+1];
		unsigned char nValue = (unsigned char)((stValue>>(8- m_nStartBit))&0xff);
        m_imageData[i/2] = nValue;
        i= i+2;
    }

    QImage image(m_imageData, graphicSize.width(), graphicSize.height(),graphicSize.width(), QImage::Format_Grayscale8);
    setPixMap(QPixmap::fromImage(image));
}

QString PixItem::getPointData(int iX, int iY, int iType)
{
    QString str = "";
    //mutex.lock();
    if (iType == 1)
    {
        str = QString("%1").arg(*(graphicData + iY * graphicSize.width()*2 + iX*2), 4, 16, QLatin1Char('0'));//数值
    }
    else if (iType == 0)
    {
        str = QString("%1").arg(*(graphicData + iY * graphicSize.width()*2 + iX*2), 4, 10, QLatin1Char('0'));//数值
    }
    //mutex.unlock();
    return str;
}

void PixItem::onSetStartBit(int nBit)
{
    m_nStartBit = nBit;
    for(int i = 0; i < m_dataSize;)
    {
        unsigned short stValue = graphicData[i] * 256 + graphicData[i+1];
		unsigned char nValue = (unsigned char)((stValue>>(8- m_nStartBit))&0xff);
        m_imageData[i/2] = nValue;
        i= i+2;
    }
    QImage image(m_imageData, graphicSize.width(), graphicSize.height(),graphicSize.width(), QImage::Format_Grayscale8);
    image.save("temmp.png");
    setPixMap(QPixmap::fromImage(image));
}

