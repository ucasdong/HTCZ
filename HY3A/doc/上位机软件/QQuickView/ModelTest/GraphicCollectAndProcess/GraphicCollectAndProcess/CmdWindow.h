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

	QPoint m_pressPos;              //��갴��λ��
	bool bPress;                    //����Ƿ���

protected://��������϶�
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

public slots:
	void on_closeButton_clicked();//�ر�
	void on_minButton_clicked();  //��С��
	void on_maxButton_clicked();  //���

};
