#pragma once

#include <QWidget>
#include "ui_ShowSetting.h"

class ShowSetting : public QWidget
{
	Q_OBJECT

public:
	ShowSetting(std::map<QString,int>* mapModel);
	~ShowSetting();

	int iGetStartBit();

private:
	Ui::ShowSetting ui;
	QPoint m_pressPos;
	bool bPress;

	int iCurrentIndex;//背景块数据是12位，取8位偏移量
	std::vector<std::pair<QString, QPushButton*>> vecBitPushButton;
	void setBtnsSheetStyle(int index);

public slots:
	void on_closeButton_clicked();
	void on_minButton_clicked();
	void on_maxButton_clicked();
	void on_bitButton_clicked();
	void on_modelChanged();

signals:
	void signal_StartBitChanged(int);
	void signal_ChangeModel(QString);

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};
