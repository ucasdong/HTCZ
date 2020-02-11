#include "GraphicCollectAndProcess.h"
#include <QFile>
#include <QMouseEvent>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "MyGroupBox.h"
#include "../../Model/DataAnalysisModel.h"

GraphicCollectAndProcess::GraphicCollectAndProcess(oneScreenParams* oneScreenTemp)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);//�ޱ߿� 
	//setMouseTracking(true);
	QString strPath = QApplication::applicationDirPath() + "\\style\\styles.qss";
	QFile qss(strPath);
	qss.open(QFile::ReadOnly);
	setStyleSheet(qss.readAll());
	qss.close();

	bStartOrStopGrab = false;
	showSettingWnd = NULL;
	dataTableFZ = NULL;
	//saveWnd = NULL;
	rePlayWnd = NULL;
	iKeyFlag = 1;//����״̬
	currentScreen = oneScreenTemp;
	//connect(ui.toolButton_ShowSetting, SIGNAL(triggered(QAction *)), this, SLOT(on_toolButton_ShowSetting_clicked()));
	initWindows();
	createMenu();

	//��ǰ����������ݱȽ϶൥�����ڣ��������ϵı����ʱ���ػ�������ʾң�����ݣ�
	ui.widget_Table->hide();
	ui.pushButton_hideOrshow->hide();

	iStartBit = 0;//����������ʼλ

	timer = new QTimer;
	connect(timer, SIGNAL(timeout()), this, SLOT(slot_TimerOut()));
}

GraphicCollectAndProcess::~GraphicCollectAndProcess()
{
	if (dataTableFZ != NULL)
	{
		dataTableFZ->hide();
		delete dataTableFZ;
		dataTableFZ = NULL;
	}
	if (showSettingWnd != NULL)
	{
		showSettingWnd->hide();
		delete showSettingWnd;
		showSettingWnd = NULL;
	}
}

//���ó�ʼ��
void GraphicCollectAndProcess::initConfigFile()
{
	
}

void GraphicCollectAndProcess::createMenu()
{
	pop_menu = new QMenu();
	QAction* action_graphic = new QAction(QIcon(":/GraphicCollectAndProcess/image/picture.png"),QStringLiteral("ͼ����ʾ"));
	QAction* action_table = new QAction(QIcon(":/GraphicCollectAndProcess/image/table_rightkey.png"), QStringLiteral("�����ʾ"));
	QAction* action_line = new QAction(QIcon(":/GraphicCollectAndProcess/image/line.png"), QStringLiteral("������ʾ"));
	QAction* action_DataType = new QAction(QIcon(":/GraphicCollectAndProcess/image/line.png"), QStringLiteral("ʮ������ʾ"));
	
	pop_menu->addAction(action_graphic);
	pop_menu->addAction(action_table);
	pop_menu->addAction(action_line);
	pop_menu->addAction(action_DataType);
	//�����ź����
	connect(action_graphic, SIGNAL(triggered()), this, SLOT(slot_ActionGraphic()));
	connect(action_table, SIGNAL(triggered()), this, SLOT(slot_ActionTable()));
	connect(action_line, SIGNAL(triggered()), this, SLOT(slot_ActionLine()));
	connect(action_DataType, SIGNAL(triggered()), this, SLOT(slot_DataShowType()));
}

void GraphicCollectAndProcess::setDataShowType(int iFlag)
{
	for (map<QString, MyGroupBox*>::iterator it = mapTypeToGraphicGroup.begin(); it != mapTypeToGraphicGroup.end(); it++)
	{
		it->second->setDataShowType(iFlag);

 		map<QString, bool>::iterator itFind = mapTypeToShowState.find(it->first);            //����-��ʾ��������
		if (itFind != mapTypeToShowState.end())
 		{
			if (itFind->second)
			{
 				it->second->setDataShowType(iFlag);
 			}
			else
 			{
				continue;
 			}
 		}
	}
}

//���ڳ�ʼ��
void GraphicCollectAndProcess::initWindows()
{
	if (currentScreen->oneScreenLayout.height() <= 0 || currentScreen->oneScreenLayout.width() <= 0)
	{
		return;
	}

	QVBoxLayout* vBox = new QVBoxLayout(ui.widget);
	vBox->setContentsMargins(0, 5, 0, 0);
	vector<QHBoxLayout*> vecHBoxs;
	for (int i = 0;i < currentScreen->oneScreenLayout.height();i++)
	{
		QHBoxLayout* hBox = new QHBoxLayout;
		hBox->setContentsMargins(2, 0, 0, 2);
		vecHBoxs.push_back(hBox);
	}	

	int iNum = 0;
	bool bOneGroup = false;
	if (currentScreen->vecCameraParams.size() == 1)
	{
		bOneGroup = true;
	}
	for (vector<oneCameraParams*>::iterator it = currentScreen->vecCameraParams.begin(); it != currentScreen->vecCameraParams.end(); it++)
	{
		int iIndex = iNum / currentScreen->oneScreenLayout.width();
		QString strID = (*it)->Camera_ID;    //����ID
		QString strName = (*it)->Camera_Name;//�������

		MyGroupBox* groupbox = new MyGroupBox(*it, bOneGroup);
		connect(groupbox, SIGNAL(btnMaxClicked(QPushButton*)), this, SLOT(slot_MaxWnd(QPushButton*)));
		vecHBoxs[iIndex]->addWidget(groupbox);

		mapTypeToShowState.insert(pair<QString, bool>(strID, true));
		mapTypeToGraphicGroup.insert(pair<QString, MyGroupBox*>(strID, groupbox));
		//cameraTypeToFZsize.insert(pair<QString, QSize>(strID, it->WINDOW_FZSize));
	
		iNum++;
	}
	for (int i = 0; i < vecHBoxs.size(); i++)
	{
		vBox->addLayout(vecHBoxs[i]);
		//vBox->setStretch(i, 1);
	}
	ui.widget->setLayout(vBox);
}

void GraphicCollectAndProcess::setDataAnalysisModel(DataAnalysisModel* dataModelTemp)
{
	dataModel = dataModelTemp;
}
/*
void GraphicCollectAndProcess::insertGraphicData(QString iCameraID, BYTE* dataTemp)
{
	if (!bStartOrStopGrab)
	{
		return;
	}
	map<QString, bool>::iterator itShow = mapTypeToShowState.find(iCameraID);            //����-��ʾ��������
	if (itShow->second)
	{
		map<QString, MyGroupBox*>::iterator it = mapTypeToGraphicGroup.find(iCameraID);      //����-ͼ�񴰿�ӳ��
		if (it != mapTypeToGraphicGroup.end())
		{
			it->second->insertGraphicData(dataTemp);
		}
	}	
}
*/

//���ݽ���ƫ��
void GraphicCollectAndProcess::slot_StartBitChanged(int iStartBitTemp)
{
	iStartBit = iStartBitTemp;
	dataModel->setStartBits(iStartBit);
}

void GraphicCollectAndProcess::slot_ModelChanged(QString model)
{
	//QString strModel = model;
	map<QString, int>::iterator itFind = currentScreen->Camera_ALLModel.find(model);
	if (itFind != currentScreen->Camera_ALLModel.end())
	{
		//int iModelWidth = itFind->second;
		for (map<QString, MyGroupBox*>::iterator itGroupBox = mapTypeToGraphicGroup.begin(); itGroupBox != mapTypeToGraphicGroup.end(); itGroupBox++)
		{
			itGroupBox->second->changeModel(model);
		}
	}
	emit signal_ChangeModel(itFind->second);
}

void GraphicCollectAndProcess::slot_MaxWnd(QPushButton* btn)
{
	if (btn)
	{
		User* pUser = (User*)(btn->userData(Qt::UserRole));
		QString strId = pUser->nID;
		QString strParentID = pUser->strParentID;

		for (map<QString, bool>::iterator itShowState = mapTypeToShowState.begin(); itShowState != mapTypeToShowState.end(); itShowState++)
		{
			if (pUser->iFlag == 0)
			{
				itShowState->second = true;
			}
			else
			{
				if (itShowState->first == strParentID)
				{
					itShowState->second = true;
				}
				else
				{
					itShowState->second = false;
				}
			}
			
		}
		for (map<QString, MyGroupBox*>::iterator it = mapTypeToGraphicGroup.begin(); it != mapTypeToGraphicGroup.end(); it++)//����-ͼ�񴰿�ӳ��)
		{
			if (strParentID == it->first)
			{
				it->second->showOrHide(strId, pUser->iFlag);
			}
			else
			{
				if (pUser->iFlag == 0)
				{
					it->second->show();
				}
				else
				{
					it->second->hide();
				}
			}
		}
	}
}

void GraphicCollectAndProcess::slot_TimerOut()
{
	oneScreenParams* currentScreenaa = currentScreen;  //��ǰ��Ļ���ò���;  //��ǰ��Ļ���ò���
	for (map<QString, MyGroupBox*>::iterator itWnd = mapTypeToGraphicGroup.begin(); itWnd != mapTypeToGraphicGroup.end(); itWnd++)
	{
		map<QString, bool>::iterator itShow = mapTypeToShowState.find(itWnd->first);
		if (itShow->second)
		{
			itWnd->second->showData();
		}	

// 		//����������ʾ
// 		map<QString, QSize>::iterator fzFind = cameraTypeToFZsize.find(itWnd->first);
// 		if (fzFind != cameraTypeToFZsize.end())
// 		{
// 			if (dataTableFZ)
// 			{
// 				dataTableFZ->updateData(itWnd->first, currentData);
// 			}
// 		}	
	}
}

void GraphicCollectAndProcess::on_closeButton_clicked()
{
	//this->close();
	qApp->exit(1);
}

void GraphicCollectAndProcess::on_minButton_clicked()
{
	this->showMinimized();
}

void GraphicCollectAndProcess::on_maxButton_clicked()
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

//����������ʾ����
void GraphicCollectAndProcess::on_toolButton_TableDisplay_clicked()
{
	if (dataTableFZ == NULL)
	{
		dataTableFZ = new DataTable(&cameraTypeToFZsize);
	}
	dataTableFZ->showMaximized();
}

//������ݻط�
void GraphicCollectAndProcess::on_toolButton_DataReplay_clicked()
{
	if (rePlayWnd == NULL)
	{
		rePlayWnd = new HistoryReplay;
		connect(rePlayWnd, SIGNAL(signal_ReplayData(QString, bool)), this, SLOT(slot_HistoryReplay(QString, bool)));
	}
	rePlayWnd->showNormal();
}

//��ʾ���ô���
void GraphicCollectAndProcess::on_toolButton_ShowSetting_clicked()
{
	if (showSettingWnd == NULL)
	{
		showSettingWnd = new ShowSetting(&currentScreen->Camera_ALLModel);
		connect(showSettingWnd, SIGNAL(signal_StartBitChanged(int)), this, SLOT(slot_StartBitChanged(int)));
		connect(showSettingWnd, SIGNAL(signal_ChangeModel(QString)), this, SLOT(slot_ModelChanged(QString)));
	}
	showSettingWnd->showNormal();
}

void GraphicCollectAndProcess::on_toolButton_SaveSetting_clicked()
{
	emit signal_SaveData();
//	if (saveWnd == NULL)
//	{
//		saveWnd = new SaveSettingWnd;
//	}
//	saveWnd->showNormal();
}

void GraphicCollectAndProcess::on_toolButton_StartColl_clicked()
{
	QString strText = ui.toolButton_StartColl->text();
	if (strText == QStringLiteral("��ʼ�ɼ�"))
	{
		ui.toolButton_StartColl->setText(QStringLiteral("ֹͣ�ɼ�"));
		ui.toolButton_StartColl->setIcon(QIcon(":/GraphicCollectAndProcess/image/startCollect.png"));
		bStartOrStopGrab = true;
		timer->start(100);
		//emit signal_StartOrStopGrab(true);
	}
	else
	{
		ui.toolButton_StartColl->setText(QStringLiteral("��ʼ�ɼ�"));
		ui.toolButton_StartColl->setIcon(QIcon(":/GraphicCollectAndProcess/image/stopCollect.png"));
		bStartOrStopGrab = false;
		timer->stop();
		//emit signal_StartOrStopGrab(false);
	}
}

void GraphicCollectAndProcess::slot_HistoryReplay(QString strFilePath, bool bStartOrStop)
{
// 	if (bStartOrStop && bStartOrStopGrab)
// 	{
// 		ui.toolButton_StartColl->setText(QStringLiteral("��ʼ�ɼ�"));
// 		ui.toolButton_StartColl->setIcon(QIcon(":/GraphicCollectAndProcess/image/stopCollect.png"));
// 		bStartOrStopGrab = false;
// 		timer->stop();
// 		emit signal_StartOrStopGrab(false);
// 	}
//	char dataPath[100];
//	memcpy(dataPath, strFilePath.toStdString().c_str(), strFilePath.length());
	if (bStartOrStop)
	{
		dataModel->ReplayDataStart((char*)strFilePath.toStdString().c_str());
	}
	else
	{
		dataModel->ReplayDataStop();
	}
	//emit signal_ReplayData(strFilePath, bStartOrStop);
}

//���ң��ң��
void GraphicCollectAndProcess::on_toolButton_YaoceYaokong_clicked()
{

}

//ͼ����ʾ
void GraphicCollectAndProcess::slot_ActionGraphic()
{
	setDataShowType(1);
}

//�����ʾ
void GraphicCollectAndProcess::slot_ActionTable()
{
	setDataShowType(2);
}

//������ʾ
void GraphicCollectAndProcess::slot_ActionLine()
{
	setDataShowType(3);
}

//�����������
void GraphicCollectAndProcess::slot_DataShowType()
{
	QAction* currentAction = dynamic_cast<QAction*>(sender());
	if (currentAction)
	{
		if (currentAction->text() == QStringLiteral("ʮ������ʾ"))
		{
			currentAction->setText(QStringLiteral("ʮ��������ʾ"));
			iFlagDataType = 0;
		}
		else
		{
			currentAction->setText(QStringLiteral("ʮ������ʾ"));
			iFlagDataType = 1;
		}
		for (map<QString, MyGroupBox*>::iterator it = mapTypeToGraphicGroup.begin(); it != mapTypeToGraphicGroup.end(); it++)//����-ͼ�񴰿�ӳ��
		{
			it->second->setDataShowFlag(iFlagDataType);
		}
	}
}

void GraphicCollectAndProcess::on_pushButton_hideOrshow_clicked()
{
	if (ui.widget_Table->isVisible())
	{
		ui.widget_Table->hide();
		ui.pushButton_hideOrshow->setText("<");
	}
	else
	{
		ui.widget_Table->show();
		ui.pushButton_hideOrshow->setText(">");
	}
}

void GraphicCollectAndProcess::mousePressEvent(QMouseEvent *event)
{
	m_pressPos = event->pos();
	bPress = false;
	if ((event->button() == Qt::LeftButton) && m_pressPos.x() < (this->width() - 80) && m_pressPos.y() < (ui.topWidget->height()))
	{
		//�������ڴ����λ�ã�����ʹ��event->globalPos() - this->pos()��
		bPress = true;
	}
}

void GraphicCollectAndProcess::mouseMoveEvent(QMouseEvent *event)
{
	if (bPress)
	{
		//����������Ļ��λ��
		QPoint move_pos = event->globalPos();
		this->move(move_pos - m_pressPos);
	}
}

void GraphicCollectAndProcess::mouseReleaseEvent(QMouseEvent *event)
{
	bPress = false;
}


void GraphicCollectAndProcess::contextMenuEvent(QContextMenuEvent *event)
{
	if (pop_menu)
	{
		pop_menu->exec(QCursor::pos());
	}
}

void GraphicCollectAndProcess::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Escape:
	{
		if (iKeyFlag == 1)
		{
			ui.ToolWidget->hide();
			ui.topWidget->show();
			iKeyFlag = 2;
		}
		else if (iKeyFlag == 2)
		{
			ui.ToolWidget->hide();
			ui.topWidget->hide();
			iKeyFlag = 3;
		}
		else
		{
			ui.ToolWidget->show();
			ui.topWidget->show();
			iKeyFlag = 1;
		}
	}		
		break;

	default:
		QWidget::keyPressEvent(event);
	}
}

