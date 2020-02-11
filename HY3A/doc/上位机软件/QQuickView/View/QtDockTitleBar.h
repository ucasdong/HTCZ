#pragma once

#include <QWidget>
#include <QDockWidget>
#include <QMovie>
#include "ui_QtDockTitleBar.h"

class QtDockTitleBar : public QWidget
{
	Q_OBJECT

public:
	QtDockTitleBar(QWidget *parent = Q_NULLPTR);
	~QtDockTitleBar();

	void initConnect();

private:
	Ui::QtDockTitleBar ui;
	QMovie *m_pTitleMovie;
};
