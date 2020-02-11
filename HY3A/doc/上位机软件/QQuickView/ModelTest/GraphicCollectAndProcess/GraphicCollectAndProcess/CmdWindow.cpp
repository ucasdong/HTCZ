#include "CmdWindow.h"
#include <QFile>
#include <QMouseEvent>

CmdWindow::CmdWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

	QString strPath = QApplication::applicationDirPath() + "\\style\\styles.qss";
	QFile qss(strPath);
	qss.open(QFile::ReadOnly);
	setStyleSheet(qss.readAll());
	qss.close();
}

CmdWindow::~CmdWindow()
{
}

void CmdWindow::mousePressEvent(QMouseEvent *event)
{
	m_pressPos = event->pos();
	bPress = false;
	if ((event->button() == Qt::LeftButton) && m_pressPos.x() < (this->width() - 80) && m_pressPos.y() < (ui.topWidget->height()))
	{
		//�������ڴ����λ�ã�����ʹ��event->globalPos() - this->pos()��
		bPress = true;
	}
}

void CmdWindow::mouseMoveEvent(QMouseEvent *event)
{
	if (bPress)
	{
		//����������Ļ��λ��
		QPoint move_pos = event->globalPos();
		this->move(move_pos - m_pressPos);
	}
}

void CmdWindow::mouseReleaseEvent(QMouseEvent *event)
{
	bPress = false;
}

void CmdWindow::on_closeButton_clicked()
{
	this->close();
}

void CmdWindow::on_minButton_clicked()
{
	this->showMinimized();
}

void CmdWindow::on_maxButton_clicked()
{
	if (this->isMaximized())
	{
		this->showNormal();
	}
	else
	{
		this->showMaximized();
	}
}
