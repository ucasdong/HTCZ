#pragma once

#include <QGraphicsView>
#include "pixitem.h"
//#include "GraphicLabel.h"
#include <QGraphicsTextItem>

class QImageView : public QGraphicsView
{
	Q_OBJECT

public:
	QImageView(QWidget *parent, QSize WndSize);
	~QImageView();

	PixItem *m_pixItem;
	//GraphicLabel* graphicLabel;
	QGraphicsTextItem* textItem;
	void recvData(BYTE* data, QPixmap* pixmap);                //接收并显示数据
	void setDataShowFlag(int iFlag);                           //数据显示类型(十进制或者十六进制)
	void changeShowWidth(int iWidth);

public slots:
	void translate(QPointF delta);  // 平移
	void zoomIn();  // 放大
	void zoomOut();  // 缩小
	void zoom(float scaleFactor); // 缩放 - scaleFactor：缩放的比例因子

protected:
	// 平移
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	// 放大/缩小
	void wheelEvent(QWheelEvent *event);

private:
	bool m_bMouseTranslate;  // 视图平移标识
	bool m_bToolTipMove;     // 视图平移标识
	QPoint m_lastMousePos;   // 鼠标最后按下的位置
	qreal m_scale;           // 缩放值
	qreal m_zoomDelta;       // 缩放的增量
	int iDataTypeFlag;       // 数据显示类型(十进制或者十六进制)
	int iShowWidth;
	QSize wndSize;           //窗口大小
};
