#include "MyScrollArea.h"
#include <QMouseEvent>
#include <QDebug>

MyScrollArea::MyScrollArea(QWidget *parent)
	: QScrollArea(parent)
{
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

MyScrollArea::~MyScrollArea()
{
}

void MyScrollArea::mouseMoveEvent(QMouseEvent *event)
{
	QPoint pos = event->pos();
	qDebug() << pos.x() << "," << pos.y() << '\n';
}
