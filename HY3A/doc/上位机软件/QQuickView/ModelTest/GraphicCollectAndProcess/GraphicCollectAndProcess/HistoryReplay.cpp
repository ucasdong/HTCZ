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
	QString filePath = QFileDialog::getOpenFileName(NULL, QStringLiteral("选择文件"), QApplication::applicationDirPath());
	if (filePath.isEmpty())
	{
		QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("未选择文件"), QMessageBox::Ok);
		return;
	}
	ui.lineEdit_Path->setText(filePath);
}

void HistoryReplay::on_pushButton_StartOrStop_clicked()
{ 
	QString strFile = ui.lineEdit_Path->text();
	QString strBtnText = ui.pushButton_StartOrStop->text();
	if (strBtnText.trimmed() == QStringLiteral("开始"))
	{
		ui.pushButton_StartOrStop->setText(QStringLiteral("停止"));
		emit signal_ReplayData(strFile, true);
	}
	else
	{
		ui.pushButton_StartOrStop->setText(QStringLiteral("开始"));
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
		//鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
		bPress = true;
	}
}

void HistoryReplay::mouseMoveEvent(QMouseEvent *event)
{
	if (bPress)
	{
		//鼠标相对于屏幕的位置
		QPoint move_pos = event->globalPos();
		this->move(move_pos - m_pressPos);
	}
}

void HistoryReplay::mouseReleaseEvent(QMouseEvent *event)
{
	bPress = false;
}
