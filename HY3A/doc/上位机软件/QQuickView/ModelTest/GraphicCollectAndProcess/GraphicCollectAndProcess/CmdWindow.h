#pragma once

#include <QWidget>
#include "ui_CmdWindow.h"

class CmdWindow : public QWidget
{
	Q_OBJECT

public:
	CmdWindow(QWidget *parent = Q_NULLPTR);
	~CmdWindow();

private:
	Ui::CmdWindow ui;

	QPoint m_pressPos;              //鼠标按下位置
	bool bPress;                    //鼠标是否按下

protected://界面鼠标拖动
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

public slots:
	void on_closeButton_clicked();//关闭
	void on_minButton_clicked();  //最小化
	void on_maxButton_clicked();  //最大化

};
