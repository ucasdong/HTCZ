#include "GraphicLabel.h"
#include <QMouseEvent>
#include <QLabel>
#include <QDebug>
#include "ConfigIni.h"

GraphicLabel::GraphicLabel(QWidget* parent, QSize sizeTemp)
{
	//setAutoFillBackground(true);
	labelSize = sizeTemp;
	this->setFixedSize(labelSize);
	graphicData = NULL;
	graphicData = new BYTE[labelSize.width() * labelSize.height()];
	memset(graphicData, 0x00 + 200, labelSize.width() * labelSize.height());
	QImage image(graphicData, labelSize.width(), labelSize.height(), labelSize.width(), QImage::Format_Indexed8);
	setPixmap(QPixmap::fromImage(image));

	toolTipLabel = new QLabel(this);
	toolTipLabel->setFixedSize(80, 20);
	toolTipLabel->setAlignment(Qt::AlignCenter);
	toolTipLabel->hide();
}

GraphicLabel::~GraphicLabel()
{
	if (graphicData != NULL)
	{
		delete[] graphicData;
		graphicData = NULL;
	}
	
	if (toolTipLabel)
	{
		toolTipLabel->hide();
		delete toolTipLabel;
		toolTipLabel = NULL;
	}
}

void GraphicLabel::recvData(BYTE* data, QPixmap* pixmap)
{
	memcpy(graphicData, data, labelSize.width() * labelSize.height());
	setPixmap(*pixmap);
}

void GraphicLabel::mousePressEvent(QMouseEvent *ev)
{
	int iX = ev->pos().x();
	int iY = ev->pos().y();
	bPress = true;
	QString strData = QString("%1").arg(*(graphicData + iX * labelSize.width() + iY), 2, 16, QLatin1Char('0'));//数值
	QString strShow = strData + QString("(") + QString::number(iX) + QString(",") + QString::number(iY) + QString(")");
	toolTipLabel->setText(strShow);
	if ((iX > this->width() - toolTipLabel->width()) || iY > this->height() - toolTipLabel->height())
	{
		toolTipLabel->move(iX - toolTipLabel->width() - 5, iY - toolTipLabel->height() - 5);
	}
	else
	{
		toolTipLabel->move(iX + 5, iY + 5);
	}
	toolTipLabel->show();
}

void GraphicLabel::mouseMoveEvent(QMouseEvent *ev)
{
	if (bPress)
	{
		int iX = ev->pos().x();
		int iY = ev->pos().y();
		if (iX < 0 || iX > this->width() || iY < 0|| iY > this->height())
		{
			return;
		}
		QString strData = QString("%1").arg(*(graphicData + iX * labelSize.width() + iY), 2, 16, QLatin1Char('0'));//数值
		QString strShow = strData + QString("(") + QString::number(iX) + QString(",") + QString::number(iY) + QString(")");
		toolTipLabel->setText(strShow);
		if ((iX > this->width() - toolTipLabel->width()) || iY > this->height() - toolTipLabel->height())
		{
			toolTipLabel->move(iX - toolTipLabel->width() - 5, iY - toolTipLabel->height() - 5);
		}
		else
		{
			toolTipLabel->move(iX + 5, iY + 5);
		}
	}
}

void GraphicLabel::mouseReleaseEvent(QMouseEvent *ev)
{
	toolTipLabel->hide();

}


