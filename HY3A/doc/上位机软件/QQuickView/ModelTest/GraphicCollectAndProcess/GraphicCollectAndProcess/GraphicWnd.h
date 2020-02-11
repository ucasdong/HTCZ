#pragma once

#include <QFrame>
#include "ui_GraphicWnd.h"

class GraphicWnd : public QFrame
{
	Q_OBJECT

public:
	GraphicWnd(QWidget *parent = Q_NULLPTR);
	~GraphicWnd();

	void addGraphic();
private:
	Ui::GraphicWnd ui;
};
