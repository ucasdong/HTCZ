#pragma once

#include <QScrollArea>

class MyScrollArea : public QScrollArea
{
	Q_OBJECT

public:
	MyScrollArea(QWidget *parent);
	~MyScrollArea();

protected:
	void mouseMoveEvent(QMouseEvent *event);
};
