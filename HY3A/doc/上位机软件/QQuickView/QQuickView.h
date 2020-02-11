#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QQuickView.h"
#include <QMouseEvent>

class QQuickView : public QMainWindow
{
	Q_OBJECT

public:
	QQuickView(QWidget *parent = Q_NULLPTR);

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void initWidgetProp();
	void initConnetion();

private:
	Ui::QQuickViewClass ui;
	int moveX, moveY;
	QPoint dragPosition;

};
