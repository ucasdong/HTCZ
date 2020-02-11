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
	PixItem(QSize WndSize);//���캯����ʼ���˱���pix
	~PixItem();

	QRectF boundingRect() const;    //ʵ���Լ���boundingRect ͼԪ�߽緽���������ͼԪ����ϵΪ���������������ص������Ĺ���
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget); //�ػ�ͼ�κ���
//	void wheelEvent(QGraphicsSceneWheelEvent *event);
//	void setZoomState(const int &zoomState);

  	//void mousePressEvent(QGraphicsSceneMouseEvent *event);
    //void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    //void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

//	int getScaleValue() const;
//	void setScaleValue(const int &);

	void setPixMap(QPixmap pixTemp);
	void recvData(BYTE* data, QPixmap* pixmap);              //��ʾ���������ͼ��
	QString getPointData(int iX, int iY, int iType);
private:
//	qreal m_scaleValue;   //����ֵ

	QSize graphicSize;    //ͼƬ��С
	QPixmap pix;          //��ΪͼԪ��ʾ��ͼƬ
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
