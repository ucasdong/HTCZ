#include "HistoryReplay.h"
#include <QFile>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>

HistoryReplay::HistoryReplay(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	//setWindowFlags(Qt::WindowStaysOnTopHint);//
	//setMouseTracking(true);
	QString strPath = QApplication::applicationDirPath() + "\\style\\styles.qss";
	QFile qss(strPath);
	qss.open(QFile::ReadOnly);
	setStyleSheet(qss.readAll());
	qss.close();

	ui.maxButton->hide();
}

HistoryReplay::~HistoryReplay()
{
}

void HistoryReplay::on_closeButton_clicked()
{
	this->close();
}

void HistoryReplay::on_minButton_clicked()
{
	this->showMinimized();
}

void HistoryReplay::on_maxButton_clicked()
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

void HistoryReplay::on_pushButton_ChoosePath_clicked()
{
	QString filePath = QFileDialog::getOpenFileName(NULL, QStringLiteral("ѡ���ļ�"), QApplication::applicationDirPath());
	if (filePath.isEmpty())
	{
		QMessageBox::warning(NULL, QStringLiteral("��ʾ"), QStringLiteral("δѡ���ļ�"), QMessageBox::Ok);
		return;
	}
	ui.lineEdit_Path->setText(filePath);
}

void HistoryReplay::on_pushButton_StartOrStop_clicked()
{ 
	QString strFile = ui.lineEdit_Path->text();
	QString strBtnText = ui.pushButton_StartOrStop->text();
	if (strBtnText.trimmed() == QStringLiteral("��ʼ"))
	{
		ui.pushButton_StartOrStop->setText(QStringLiteral("ֹͣ"));
		emit signal_ReplayData(strFile, true);
	}
	else
	{
		ui.pushButton_StartOrStop->setText(QStringLiteral("��ʼ"));
		emit signal_ReplayData(strFile, false);
	}
	
	//this->close();
	
}

void HistoryReplay::mousePressEvent(QMouseEvent *event)
{
	m_pressPos = event->pos();
	bPress = false;
	if ((event->button() == Qt::LeftButton) && m_pressPos.x() < (this->width() - 80) && m_pressPos.y() < (ui.topWidget->height()))
	{
		//�������ڴ����λ�ã�����ʹ��event->globalPos() - this->pos()��
		bPress = true;
	}
}

void HistoryReplay::mouseMoveEvent(QMouseEvent *event)
{
	if (bPress)
	{
		//����������Ļ��λ��
		QPoint move_pos = event->globalPos();
		this->move(move_pos - m_pressPos);
	}
}

void HistoryReplay::mouseReleaseEvent(QMouseEvent *event)
{
	bPress = false;
}
