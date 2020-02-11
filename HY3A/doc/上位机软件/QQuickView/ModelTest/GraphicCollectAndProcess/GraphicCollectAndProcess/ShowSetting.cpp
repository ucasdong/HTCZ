#include "ShowSetting.h"
#include <QFile>
#include <QMouseEvent>
#include <QRadioButton>

ShowSetting::ShowSetting(std::map<QString, int>* mapModel)
{
	ui.setupUi(this);
	//setWindowFlags(Qt::FramelessWindowHint);//无边框 
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	//setWindowFlags(Qt::WindowStaysOnTopHint);//
	//setMouseTracking(true);
	QString strPath = QApplication::applicationDirPath() + "\\style\\styles.qss";
	QFile qss(strPath);
	qss.open(QFile::ReadOnly);
	setStyleSheet(qss.readAll());
	qss.close();
	
	//ui.minButton->setDisabled(true);
	ui.maxButton->setDisabled(true);
	//ui.minButton->hide();
	ui.maxButton->hide();

	iCurrentIndex = 0;
	vecBitPushButton.clear();
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_MoveLeft->objectName(), ui.pushButton_MoveLeft));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_15bit->objectName(), ui.pushButton_15bit));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_14bit->objectName(), ui.pushButton_14bit));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_13bit->objectName(), ui.pushButton_13bit));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_12bit->objectName(), ui.pushButton_12bit));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_11bit->objectName(), ui.pushButton_11bit));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_10bit->objectName(), ui.pushButton_10bit));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_9bit->objectName(), ui.pushButton_9bit));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_8bit->objectName(), ui.pushButton_8bit));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_7bit->objectName(), ui.pushButton_7bit));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_6bit->objectName(), ui.pushButton_6bit));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_5bit->objectName(), ui.pushButton_5bit));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_4bit->objectName(), ui.pushButton_4bit));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_3bit->objectName(), ui.pushButton_3bit));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_2bit->objectName(), ui.pushButton_2bit));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_1bit->objectName(), ui.pushButton_1bit));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_0bit->objectName(), ui.pushButton_0bit));
	vecBitPushButton.push_back(std::pair<QString, QPushButton*>(ui.pushButton_MoveRight->objectName(), ui.pushButton_MoveRight));
	for (std::vector<std::pair<QString, QPushButton*>>::iterator it = vecBitPushButton.begin(); it != vecBitPushButton.end(); it++)
	{
		connect(it->second, SIGNAL(clicked()), this, SLOT(on_bitButton_clicked()));
	}
	vecBitPushButton.erase(vecBitPushButton.begin());
	vecBitPushButton.erase(--vecBitPushButton.end());
	setBtnsSheetStyle(0);

	QHBoxLayout* hLay = new QHBoxLayout;
	for (std::map<QString, int>::iterator itModel = mapModel->begin(); itModel != mapModel->end(); itModel++)
	{
		QRadioButton* radio = new QRadioButton(itModel->first);
		hLay->addWidget(radio);
		connect(radio, SIGNAL(clicked()), this, SLOT(on_modelChanged()));
	}
	//hLay->addStretch();
	ui.groupBox_ShowModel->setLayout(hLay);
}

ShowSetting::~ShowSetting()
{
}

int ShowSetting::iGetStartBit()
{
	return iCurrentIndex;
}

void ShowSetting::setBtnsSheetStyle(int index)
{
	int iCount = 0;
	for (std::vector<std::pair<QString, QPushButton*>>::iterator it = vecBitPushButton.begin();it != vecBitPushButton.end();it++)
	{
		QPushButton* btn = it->second;
		if (iCount >= iCurrentIndex && iCount <= iCurrentIndex + 7)
		{
			btn->setStyleSheet(
				"QPushButton{background-color: rgba(150, 150, 150, 255);color:white;}"
				"QPushButton:hover{background-color:#2ECBFF;color: white;}");
		}
		else
		{
			btn->setStyleSheet(
				"QPushButton{background-color: rgba(13, 24, 40, 255);color:white;}"
				"QPushButton:hover{background-color:#2ECBFF;color: white;}");
		}
		iCount++;
	}
}

void ShowSetting::on_closeButton_clicked()
{
	this->close();
}

void ShowSetting::on_minButton_clicked()
{
	this->showMinimized();
}

void ShowSetting::on_maxButton_clicked()
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

void ShowSetting::on_bitButton_clicked()
{
	QPushButton* btn = (QPushButton*)sender();
	if (btn->objectName() == ui.pushButton_MoveLeft->objectName())
	{
		iCurrentIndex--;
		if (iCurrentIndex < 0)
		{
			iCurrentIndex++;
			return;
		}
	}
	else if (btn->objectName() == ui.pushButton_MoveRight->objectName())
	{
		iCurrentIndex++;
		if ((iCurrentIndex + 7) > 15)
		{
			iCurrentIndex--;
			return;
		}
	}
	else
	{
		int iNum = 0;
		for (std::vector<std::pair<QString, QPushButton*>>::iterator it = vecBitPushButton.begin(); it != vecBitPushButton.end(); it++)
		{
			if (it->first == btn->objectName())
			{
				break;
			}
			iNum++;
		}
		if (iNum + 7 <= 15)
		{
			iCurrentIndex = iNum;
		}
		else if ((iNum - 7) >= 0)
		{
			iCurrentIndex = iNum - 7;
		}
		else
		{
			return;
		}
	}
	setBtnsSheetStyle(iCurrentIndex);
	signal_StartBitChanged(iCurrentIndex);
}

void ShowSetting::on_modelChanged()
{
	QRadioButton* radio = qobject_cast<QRadioButton*>(sender());
	if (radio)
	{
		emit signal_ChangeModel(radio->text());
	}
}

void ShowSetting::mousePressEvent(QMouseEvent *event)
{
	m_pressPos = event->pos();
	bPress = false;
	if ((event->button() == Qt::LeftButton) && m_pressPos.x() < (this->width() - 80) && m_pressPos.y() < (ui.topWidget->height()))
	{
		//鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
		bPress = true;
	}
}

void ShowSetting::mouseMoveEvent(QMouseEvent *event)
{
	if (bPress)
	{
		//鼠标相对于屏幕的位置
		QPoint move_pos = event->globalPos();
		this->move(move_pos - m_pressPos);
	}
}

void ShowSetting::mouseReleaseEvent(QMouseEvent *event)
{
	bPress = false;
}
