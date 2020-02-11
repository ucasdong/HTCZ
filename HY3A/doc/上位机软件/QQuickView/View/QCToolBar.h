#pragma once

#include <QWidget>
#include "ui_QCToolBar.h"

class QCToolBar : public QWidget
{
	Q_OBJECT

public:
	QCToolBar(QWidget *parent = Q_NULLPTR);
	~QCToolBar();

private:
	Ui::QCToolBar ui;
};
