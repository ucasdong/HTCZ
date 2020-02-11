#pragma once

#include <QWidget>
#include "ui_QCommonDockWidget.h"

class QCommonDockWidget : public QWidget
{
	Q_OBJECT

public:
	QCommonDockWidget(QWidget *parent = Q_NULLPTR);
	~QCommonDockWidget();

private:
	Ui::QCommonDockWidget ui;
};
