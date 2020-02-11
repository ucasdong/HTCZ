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
//#include "commondef.h"
#include <QTimer>
#include <QThread>

class PixItem :public QGraphicsItem
{

public:
    PixItem(QSize WndSize, QGraphicsItem *parent = nullptr);//构造函数初始化了变量pix
	~PixItem();
    QPixmap pix;          //作为图元显示的图片

public slots:
    void recieve(unsigned char*);

public:
	QRectF boundingRect() const;    //实现自己的boundingRect 图元边界方法，完成以图元坐标系为基础增加两个像素点的冗余的工作
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget); //重画图形函数
//	void wheelEvent(QGraphicsSceneWheelEvent *event);
//	void setZoomState(const int &zoomState);

  	//void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    //void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

//	int getScaleValue() const;
//	void setScaleValue(const int &);

	void setPixMap(QPixmap pixTemp);
	void recvGraphicData(unsigned char* data);                                //显示多光谱数据图像
	QString getPointData(int iX, int iY, int iType);
    void onSetStartBit(int nBit);
	unsigned char* graphicData;
	unsigned char * m_imageData;
    QSize graphicSize;    //图片大小
    quint64 m_dataSize;
    int m_nStartBit;

private:
//	qreal m_scaleValue;   //缩放值

    QTimer *m_pTimerUpdata;
    QImage * m_pImage;
//	int m_zoomState;

	//bool m_isMove;
//	QPointF m_startPos;
	unsigned char* graphicData2711;

	QMutex mutex;
//	bool bShowGraph;
//	QLabel* toolTipLabel;
//	QString toolTipText;
//	bool bPress;
};
