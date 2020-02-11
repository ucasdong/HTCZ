#pragma once

#include <QLabel>
#include "Common.h"
#include <QGraphicsWidget>

class GraphicLabel : public QLabel/*, public QGraphicsWidget*/
{
	Q_OBJECT

public:
	GraphicLabel(QWidget* parent, QSize sizeTemp);
	~GraphicLabel();

	BYTE* graphicData;
	QSize labelSize;
	QLabel* toolTipLabel;
	//QImage* imageGraphic;
	bool bPress;
	void recvData(BYTE* data, QPixmap* pixmap);              //��ʾ���������ͼ��

protected:
	void mouseMoveEvent(QMouseEvent *ev);
	void mousePressEvent(QMouseEvent *ev);
	void mouseReleaseEvent(QMouseEvent *ev);
};
