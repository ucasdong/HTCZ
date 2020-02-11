#pragma once

#include <QWidget>
#include "ui_HistoryReplay.h"

class HistoryReplay : public QWidget
{
	Q_OBJECT

public:
	HistoryReplay(QWidget *parent = Q_NULLPTR);
	~HistoryReplay();

private:
	Ui::HistoryReplay ui;

	QPoint m_pressPos;
	bool bPress;

public slots:
	void on_closeButton_clicked();
	void on_minButton_clicked();
	void on_maxButton_clicked();

	void on_pushButton_ChoosePath_clicked();           //选择数据路径
	void on_pushButton_StartOrStop_clicked();          //点击开始

signals:
	void signal_ReplayData(QString, bool);

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};
