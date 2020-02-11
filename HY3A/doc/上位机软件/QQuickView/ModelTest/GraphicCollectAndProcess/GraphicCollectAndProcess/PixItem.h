#pragma once

#include <QGraphicsItem>
#include <QPixmap>
#include <QPainter>
#include <QRectF>
#include <QMouseEvent>
#include <QPointF>
#include <QDragEnterEvent>
#include <QGraphicsSceneWheelEvent>
#include <QMutex>
#include <QLabel>
#include "Common.h"

enum Enum_ZoomState {
	NO_STATE,
	RESET,
	ZOOM_IN,
	ZOOM_OUT
};

enum Enum_ZoomTimes {
	ZOOM_IN_TIMES = 10,
	ZOOM_OUT_TIMES = -10,
};

class PixItem : public QGraphicsItem
{
	//Q_OBJECT

public:
	PixItem(QSize WndSize);//构造函数初始化了变量pix
	~PixItem();

	QRectF boundingRect() const;    //实现自己的boundingRect 图元边界方法，完成以图元坐标系为基础增加两个像素点的冗余的工作
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget); //重画图形函数
//	void wheelEvent(QGraphicsSceneWheelEvent *event);
//	void setZoomState(const int &zoomState);

  	//void mousePressEvent(QGraphicsSceneMouseEvent *event);
    //void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    //void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

//	int getScaleValue() const;
//	void setScaleValue(const int &);

	void setPixMap(QPixmap pixTemp);
	void recvData(BYTE* data, QPixmap* pixmap);              //显示多光谱数据图像
	QString getPointData(int iX, int iY, int iType);
private:
//	qreal m_scaleValue;   //缩放值

	QSize graphicSize;    //图片大小
	QPixmap pix;          //作为图元显示的图片
//	int m_zoomState;

	//bool m_isMove;
//	QPointF m_startPos;

	BYTE* graphicData2711;
	BYTE* graphicData;
	QMutex mutex;
//	QLabel* toolTipLabel;
//	QString toolTipText;
//	bool bPress;
};
