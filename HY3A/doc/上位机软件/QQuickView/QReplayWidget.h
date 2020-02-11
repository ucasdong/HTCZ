#pragma once

#include <QWidget>
#include "ui_QReplayWidget.h"
#include "ReplayListFrame.h"

class QReplayWidget : public QDialog
{
	Q_OBJECT

public:
	QReplayWidget(QWidget *parent = Q_NULLPTR);
	~QReplayWidget();

public slots:
	void slot_showListWidget();
	void slot_playNext();
	void slot_playPre();
	void slot_playPause();
	void slot_playModeChange();
	void slot_setPosition(int position);

private:
	Ui::QReplayWidget ui;
	ReplayListFrame * m_ReplayListFrame;
	int m_ReplayListFrameX, m_ReplayListFrameY;

};
