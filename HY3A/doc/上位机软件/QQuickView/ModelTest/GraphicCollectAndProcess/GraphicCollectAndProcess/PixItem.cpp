#include "PixItem.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QDrag>
#include <math.h>

int i = 0;
int iPiece = 0;
int iArea = 0;

PixItem::PixItem(QSize WndSize)
{
	setAcceptDrops(false);   //设置可拖拽

	graphicSize = WndSize;
	graphicData = NULL;
	graphicData = new BYTE[graphicSize.width() * graphicSize.height()];
	memset(graphicData, 0x00, graphicSize.width() * graphicSize.height());

	QImage image(graphicData, graphicSize.width(), graphicSize.height(), graphicSize.width(), QImage::Format_Grayscale8);
	setPixMap(QPixmap::fromImage(image));
}

PixItem::~PixItem()
{
	if (graphicData)
	{
		delete[] graphicData;
		graphicData = NULL;
	}
}

QRectF PixItem::boundingRect() const
{
	return QRectF(-pix.width() / 2, -pix.height() / 2, pix.width(), pix.height());
}

void PixItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (graphicData != NULL)
	{
		painter->drawPixmap(-pix.width() / 2, -pix.height() / 2, pix);
		//bShowGraph = false;
	}	
}
/*
void PixItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
	setZoomState(NO_STATE);

	int scaleValue = m_scaleValue;
	if (event->delta() > 0)  //delta（）为正，滚轮向上滚
	{
		scaleValue++;
	}
	else
	{
		scaleValue--;
	}

	if (scaleValue > ZOOM_IN_TIMES || scaleValue < ZOOM_OUT_TIMES)
		return;

	m_scaleValue = scaleValue;
	
	qreal s;
	if (m_scaleValue > 0)
	{
		s = pow(1.1, m_scaleValue);        //放大 计算x的y方次 参数都是double类型
	}
	else
	{
		s = pow(1 / 1.1, -m_scaleValue);      //缩小
	}
	setScale(s);
	//setTransformOriginPoint(event->pos().x(), event->pos().y());
	
}

void PixItem::setZoomState(const int &zoomState)
{
	m_zoomState = zoomState;
	if (m_zoomState == RESET)
	{
		m_scaleValue = 0;
		setScale(1);
		setTransformOriginPoint(0, 0);
	}
}


void PixItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_startPos = event->pos();
		int iX = event->pos().x();
		int iY = event->pos().y();
		bPress = true;
		QString strData = QString("%1").arg(*(graphicData2711 + iX * graphicSize.width() + iY), 2, 16, QLatin1Char('0'));//数值
		toolTipText = strData + QString("(") + QString::number(iX) + QString(",") + QString::number(iY) + QString(")");
	}
	
	//toolTipLabel->setText(strShow);
// 	if ((iX > this->width() - toolTipLabel->width()) || iY > this->height() - toolTipLabel->height())
// 	{
// 		toolTipLabel->move(iX - toolTipLabel->width() - 5, iY - toolTipLabel->height() - 5);
// 	}
// 	else
// 	{
// 		toolTipLabel->move(iX + 5, iY + 5);
// 	}
	//toolTipLabel->show();

// 	if (event->button() == Qt::LeftButton)
// 	{
// 		m_isMove = true;
// 	}
}

// void PixItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
// {
// 	if (m_isMove)
// 	{
// 		QPointF point = event->pos() - m_startPos;
// 		moveBy(point.x(), point.y());
// 	}
// }

void PixItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		bPress = false;
	}
}

/*
int PixItem::getScaleValue() const
{
	return m_scaleValue;

}

void PixItem::setScaleValue(const int &scaleValue)
{
	if (scaleValue > ZOOM_IN_TIMES || scaleValue < ZOOM_OUT_TIMES)
		return;

	m_scaleValue = scaleValue;

	qreal s;
	if (m_scaleValue > 0)
	{
		s = pow(1.1, m_scaleValue);        //放大 计算x的y方次 参数都是double类型
	}
	else
	{
		s = pow(1 / 1.1, -m_scaleValue);      //缩小
	}

	setScale(s);
}
*/
void PixItem::setPixMap(QPixmap pixTemp)
{
	pix = pixTemp;
}

void PixItem::recvData(BYTE* data, QPixmap* pixmap)
{
	//mutex.lock();
	//memcpy(graphicData, data, graphicSize.width() * graphicSize.height());
	//mutex.unlock();
	graphicData = data;
	setPixMap(*pixmap);
}

QString PixItem::getPointData(int iX, int iY, int iType)
{
	QString str = "";
	//mutex.lock();
	if (iType == 1)
	{
		str = QString("%1").arg(*(graphicData + iY * graphicSize.width() + iX), 2, 16, QLatin1Char('0'));//数值
	}
	else if (iType == 0)
	{
		str = QString("%1").arg(*(graphicData + iY * graphicSize.width() + iX), 2, 10, QLatin1Char('0'));//数值
	}
	//mutex.unlock();
	return str;
}
