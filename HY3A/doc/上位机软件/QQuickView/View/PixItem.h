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
    PixItem(QSize WndSize, QGraphicsItem *parent = nullptr);//���캯����ʼ���˱���pix
	~PixItem();
    QPixmap pix;          //��ΪͼԪ��ʾ��ͼƬ

public slots:
    void recieve(unsigned char*);

public:
	QRectF boundingRect() const;    //ʵ���Լ���boundingRect ͼԪ�߽緽���������ͼԪ����ϵΪ���������������ص������Ĺ���
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget); //�ػ�ͼ�κ���
//	void wheelEvent(QGraphicsSceneWheelEvent *event);
//	void setZoomState(const int &zoomState);

  	//void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    //void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

//	int getScaleValue() const;
//	void setScaleValue(const int &);

	void setPixMap(QPixmap pixTemp);
	void recvGraphicData(unsigned char* data);                                //��ʾ���������ͼ��
	QString getPointData(int iX, int iY, int iType);
    void onSetStartBit(int nBit);
	unsigned char* graphicData;
	unsigned char * m_imageData;
    QSize graphicSize;    //ͼƬ��С
    quint64 m_dataSize;
    int m_nStartBit;

private:
//	qreal m_scaleValue;   //����ֵ

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
