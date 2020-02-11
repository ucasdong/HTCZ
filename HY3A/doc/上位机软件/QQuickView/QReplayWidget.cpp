#include "QReplayWidget.h"

QReplayWidget::QReplayWidget(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	m_ReplayListFrame = new ReplayListFrame(this);
	m_ReplayListFrameX = this->width() - m_ReplayListFrame->width();
	m_ReplayListFrameY = this->height() - m_ReplayListFrame->height() - ui.bottomWidget->height();
	m_ReplayListFrame->hide();

	connect(ui.bottomWidget->listBtn, &QPushButton::clicked, this, &QReplayWidget::slot_showListWidget);
	connect(ui.bottomWidget->playBtn, &QPushButton::clicked, this, &QReplayWidget::slot_playPause);
	connect(ui.bottomWidget->playModeBtn, &QPushButton::clicked, this, &QReplayWidget::slot_playModeChange);
	connect(ui.bottomWidget->playTimeSlider, &QSlider::valueChanged, this, &QReplayWidget::slot_setPosition);
	connect(ui.bottomWidget->nextBtn, &QPushButton::clicked, this, &QReplayWidget::slot_playNext);
	connect(ui.bottomWidget->lastBtn, &QPushButton::clicked, this, &QReplayWidget::slot_playPre);

}

QReplayWidget::~QReplayWidget()
{
}

void QReplayWidget::slot_showListWidget()
{
	if (m_ReplayListFrame->isVisible())
		m_ReplayListFrame->hide();
	else
	{
		m_ReplayListFrame->setGeometry(width() - m_ReplayListFrame->width(), height() - m_ReplayListFrame->height() - ui.bottomWidget->height(), 580, 470);
		m_ReplayListFrame->show();
		m_ReplayListFrame->raise();
	}
}

void QReplayWidget::slot_playNext()
{

}

void QReplayWidget::slot_playPre()
{

}

void QReplayWidget::slot_playPause()
{

}

void QReplayWidget::slot_playModeChange()
{

}

void QReplayWidget::slot_setPosition(int position)
{

}
