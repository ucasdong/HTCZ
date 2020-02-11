#include "MyGroupBox.h"
#include <QSize>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

MyGroupBox::MyGroupBox(oneCameraParams* oneData, bool bOneGroup)
{
	currentCameraData = oneData;
	setTitle(currentCameraData->Camera_Name);
	setAlignment(Qt::AlignCenter);
	initGroupBox(bOneGroup);
	iFlagType = 1;
	iStartBit = 0;

	bHaveNewData = false;
	bThreadStop = false;

	iCurrentTime = 1;
	bShowGraphic = true;                           //��ʾͼ��
	bShowTable = true;                             //��ʾ���
	bShowCurve = true;                             //��ʾ����
	//createBuffer();
	//fileWrite = fopen("d:/dataShow.raw", "w+b");
}

MyGroupBox::~MyGroupBox()
{
	//fclose(fileWrite);
	bHaveNewData = false;
	bThreadStop = true;
}

void MyGroupBox::showData()
{
//  	if (!bAnalysisFinish)
//  	{
//  		return;
//  	}
	if (iCurrentTime % 5 >= GRAPHICSHOWTIME)
	{
		//	bShowGraphic = true;                           //��ʾͼ��
		//	bShowTable = true;                             //��ʾ���
		bShowCurve = true;                             //��ʾ����
	}
	else
	{
		//	bShowGraphic = false;                           //��ʾͼ��
		//	bShowTable = false;                             //��ʾ���
		bShowCurve = false;                                 //��ʾ����
	}
	if (iFlagType == 1)
	{
		for (unordered_map<QString, oneCCDNameAndSize>::iterator itCCDParam = currentCameraData->mapCCD_NamesAndSizes.begin(); itCCDParam != currentCameraData->mapCCD_NamesAndSizes.end(); itCCDParam++)
		{
			map<QString, QImageView*>::iterator itGraphic = mapNameToGraphic.find(itCCDParam->first);
			if (itGraphic != mapNameToGraphic.end())
			{
				int iIndex = itCCDParam->second.iAnalysisIndex - 1;
				if (iIndex < 0)
				{
					iIndex = BUFFER_COUNT - 1;
				}
				//int iCount = iIndex * itCCDParam->second.CCD_Size.width() * itCCDParam->second.CCD_Size.height();
				//BYTE* dataShow = itCCDParam->second.analyticalCCDData + iCount;
				BYTE* dataShow = itCCDParam->second.CurAnalyticalCCDData;
				itGraphic->second->recvData(dataShow, &itCCDParam->second.pixMap);
			}
		}
	}
	else if (iFlagType == 2)
	{
		for (unordered_map<QString, oneCCDNameAndSize>::iterator itCCDParam = currentCameraData->mapCCD_NamesAndSizes.begin(); itCCDParam != currentCameraData->mapCCD_NamesAndSizes.end(); itCCDParam++)
		{
			map<QString, TableView*>::iterator itTable = mapNameToTable.find(itCCDParam->first);
			if (itTable != mapNameToTable.end())
			{
				int iIndex = itCCDParam->second.iAnalysisIndex - 1;
				if (iIndex < 0)
				{
					iIndex = BUFFER_COUNT - 1;
				}
					
				//int iCount = iIndex * itCCDParam->second.CCD_Size.width() * itCCDParam->second.CCD_Size.height() * currentCameraData->One_Pixel_Length;
				itTable->second->recvTableData(itCCDParam->second.CurOriginalCCDData);
			}
		}	
	}
	else if(iFlagType == 3)
	{
		for (unordered_map<QString, oneCCDNameAndSize>::iterator itCCDParam = currentCameraData->mapCCD_NamesAndSizes.begin(); itCCDParam != currentCameraData->mapCCD_NamesAndSizes.end(); itCCDParam++)
		{
			map<QString, CurveWidget*>::iterator itCurve = mapNameToCurve.find(itCCDParam->first);
			if (itCurve != mapNameToCurve.end())
			{
				int iIndex = itCCDParam->second.iAnalysisIndex - 1;
				if (iIndex < 0)
				{
					iIndex = BUFFER_COUNT - 1;
				}
				int iCount = iIndex * itCCDParam->second.CCD_Size.width() * itCCDParam->second.CCD_Size.height();
				//itCurve->second->recvData(itCCDParam->first, itCCDParam->second.analyticalCCDData + iCount);
			}
		}
	}

	iCurrentTime++;
	if (iCurrentTime >= GRAPHICSHOWTIME)
	{
		iCurrentTime = 0;
	}
}

void MyGroupBox::setBitStart(int iStartBitTemp)
{
	iStartBit = iStartBitTemp;
}

//������ʾ����
void MyGroupBox::setDataShowType(int iFlag)
{
	iFlagType = iFlag;
	if (iFlag == 1)//��ʾͼ��
	{
		for (map<QString, QImageView*>::iterator it = mapNameToGraphic.begin(); it != mapNameToGraphic.end(); it++)//������-ͼ��������򴰿�ӳ��
		{
			it->second->show();
		}
		for (map<QString, TableView*>::iterator it = mapNameToTable.begin(); it != mapNameToTable.end(); it++) //������-��񴰿�ӳ��
		{
			it->second->hide();
		}
		for (map<QString, CurveWidget*>::iterator it = mapNameToCurve.begin(); it != mapNameToCurve.end(); it++)//������-���ߴ���ӳ��                  
		{
			it->second->hide();
		}
	}
	else if (iFlag == 2)//��ʾ���
	{
		for (map<QString, QImageView*>::iterator it = mapNameToGraphic.begin(); it != mapNameToGraphic.end(); it++)//������-ͼ��������򴰿�ӳ��
		{
			it->second->hide();
		}
		for (map<QString, TableView*>::iterator it = mapNameToTable.begin(); it != mapNameToTable.end(); it++) //������-��񴰿�ӳ��
		{
			it->second->show();
		}
		for (map<QString, CurveWidget*>::iterator it = mapNameToCurve.begin(); it != mapNameToCurve.end(); it++)//������-���ߴ���ӳ��                  
		{
			it->second->hide();
		}
	}
	else if (iFlag == 3)//��ʾ����
	{
		for (map<QString, QImageView*>::iterator it = mapNameToGraphic.begin(); it != mapNameToGraphic.end(); it++)//������-ͼ��������򴰿�ӳ��
		{
			it->second->hide();
		}
		for (map<QString, TableView*>::iterator it = mapNameToTable.begin(); it != mapNameToTable.end(); it++) //������-��񴰿�ӳ��
		{
			it->second->hide();
		}
		for (map<QString, CurveWidget*>::iterator it = mapNameToCurve.begin(); it != mapNameToCurve.end(); it++)//������-���ߴ���ӳ��                  
		{
			it->second->show();
		}
	}
}

void MyGroupBox::setDataShowFlag(int iFlag)
{
	for (map<QString, QImageView*>::iterator it = mapNameToGraphic.begin(); it != mapNameToGraphic.end(); it++)               //������-ͼ�񴰿�ӳ��2
	{
		it->second->setDataShowFlag(iFlag);
	}
	for (map<QString, TableView*>::iterator it = mapNameToTable.begin(); it != mapNameToTable.end(); it++)                    //������-��񴰿�ӳ��
	{
		it->second->setDataShowFlag(iFlag);
	}
}

void MyGroupBox::changeModel(QString strModel)
{
	map<QString, int>::iterator itModel = currentCameraData->Camera_Model.find(strModel);
	if (itModel != currentCameraData->Camera_Model.end())
	{
		for (unordered_map<QString, oneCCDNameAndSize>::iterator itCCDParam = currentCameraData->mapCCD_NamesAndSizes.begin(); itCCDParam != currentCameraData->mapCCD_NamesAndSizes.end(); itCCDParam++)
		{
			itCCDParam->second.CCD_Size.setWidth(itModel->second);
		}
		//ͼ��
		for (map<QString, QImageView*>::iterator itGraphic = mapNameToGraphic.begin(); itGraphic != mapNameToGraphic.end(); itGraphic++)                //������-ͼ�񴰿�ӳ��
		{
			itGraphic->second->changeShowWidth(itModel->second);
		}
		//���
// 	for (map<int, TableView*>::iterator itTable = mapNameToTable.begin(); itTable != mapNameToTable.end(); itTable++)                   //������-��񴰿�ӳ��
// 	{
// 		itTable->second->changeShowWidth(itModel->second);
// 	}
	//����
		for (map<QString, CurveWidget*>::iterator itCurve = mapNameToCurve.begin(); itCurve != mapNameToCurve.end(); itCurve++)                 //������-���ߴ���ӳ��
		{

		}
	}
}

void MyGroupBox::showOrHide(QString strID, int iFlag)
{
	this->show();
	bool bHave = false;
	if (iFlag == 0)
	{
		bHave = true;
		for (map<QString, QWidget*>::iterator it = mapNameToWidget.begin(); it != mapNameToWidget.end(); it++)
		{
			it->second->show();
		}
	}
	else
	{
		for (map<QString, QWidget*>::iterator it = mapNameToWidget.begin(); it != mapNameToWidget.end(); it++)
		{
			if (it->first == strID)
			{
				bHave = true;
				it->second->show();
			}
			else
			{
				it->second->hide();
			}
		}
	}
	
	if (!bHave)
	{
		this->hide();
	}
}

void MyGroupBox::slot_btnClicked()
{
	QPushButton* btn = qobject_cast<QPushButton*>(sender());
	User* pUser = (User*)(btn->userData(Qt::UserRole));
	if (pUser->iFlag == 0)
	{
		pUser->iFlag = 1;
	}
	else
	{
		pUser->iFlag = 0;
	}
	emit btnMaxClicked(btn);
}

void MyGroupBox::createBuffer()
{
	int iFrameCount = currentCameraData->One_Frame_LineCount;
	int iFrameLength = currentCameraData->One_Frame_LineLength;
	int iPixelLen = currentCameraData->One_Pixel_Length;

	BYTE* dataCamera = new BYTE[iFrameCount * iFrameLength * iPixelLen * BUFFER_COUNT];
	memset(dataCamera, 0x00, iFrameCount * iFrameLength * iPixelLen * BUFFER_COUNT);
	
	currentCameraData->BHaveNewData = false;
	currentCameraData->iDataIndexOfBuffer = 0;
	currentCameraData->cameraData = dataCamera;
	currentCameraData->camera_FZ_data = NULL;//�������ݴ���	


	/*
	CameraData cameraDataTemp;
	cameraDataTemp.strCamera_ID = currentCameraData->Camera_ID;
	cameraDataTemp.iCamera_Analysis_ID = currentCameraData->Camera_Analysis_ID;
	cameraDataTemp.cameraData = dataCamera;
	cameraDataTemp.camera_FZ_data = NULL;//�������ݴ���	
	cameraDataTemp.BHaveNewData = false;	
	cameraDataTemp.iDataIndexOfBuffer = 0;
	
	//mapCameraIDToBuffer.insert(std::pair<int, CameraData>(currentCameraData->Camera_Analysis_ID, cameraDataTemp));//������ݻ�����
	*/
	for (unordered_map<QString, oneCCDNameAndSize>::iterator itCCDParam = currentCameraData->mapCCD_NamesAndSizes.begin(); itCCDParam != currentCameraData->mapCCD_NamesAndSizes.end(); itCCDParam++)
	{
		/*
		CurGraphicCCDData oneCCDData;
		oneCCDData.strCCD_ID = currentCameraData->CCD_NamesAndSizes[i].strCCD_ID;
		oneCCDData.iCCD_Analysis_ID = currentCameraData->CCD_NamesAndSizes[i].iCCD_Analysis_ID;		
		oneCCDData.strCCD_Name = currentCameraData->CCD_NamesAndSizes[i].strCCD_Name;
		oneCCDData.iCCD_DataStartByte = currentCameraData->CCD_NamesAndSizes[i].iCCD_CCD_StartByte;
		int iId = currentCameraData->CCD_NamesAndSizes[i].iCCD_Analysis_ID;
		int iGraphicW = currentCameraData->CCD_NamesAndSizes[i].CCD_Size.width();
		int iGraphicH = currentCameraData->CCD_NamesAndSizes[i].CCD_Size.height();
		oneCCDData.iCCD_Width = iGraphicW;
		oneCCDData.iCCD_Height = iGraphicH;
		oneCCDData.iCCD_FZ_Length = currentCameraData->One_Line_FZ_Length;
		oneCCDData.iCCD_FZ_Lines = currentCameraData->One_FZ_Lines;
		oneCCDData.originalCCDData = tempOriginalData;
		oneCCDData.analyticalCCDData = tempAnalysisData;
		oneCCDData.iAnalysisIndex = 0;
		QImage Pimage(tempAnalysisData, iGraphicW, iGraphicH, iGraphicW, QImage::Format_Indexed8);
		oneCCDData.pixMap = QPixmap::fromImage(Pimage);
		mapGraphicData.insert(std::pair<int, CurGraphicCCDData>(iId, oneCCDData));
		*/
		int iGraphicW = itCCDParam->second.CCD_Size.width();
		int iGraphicH = itCCDParam->second.CCD_Size.height();
		BYTE* tempOriginalData = new BYTE[iGraphicW * iGraphicH * iPixelLen * BUFFER_COUNT];
		BYTE* tempAnalysisData = new BYTE[iGraphicW * iGraphicH * BUFFER_COUNT];
		memset(tempOriginalData, 0x00, iGraphicW * iGraphicH * iPixelLen * BUFFER_COUNT);
		memset(tempAnalysisData, 0x00, iGraphicW * iGraphicH * BUFFER_COUNT);
		itCCDParam->second.originalCCDData = tempOriginalData;
		itCCDParam->second.analyticalCCDData = tempAnalysisData;
		itCCDParam->second.iAnalysisIndex = 0;
		QImage Pimage(tempAnalysisData, iGraphicW, iGraphicH, iGraphicW, QImage::Format_Indexed8);
		itCCDParam->second.pixMap = QPixmap::fromImage(Pimage);
	}	
}

//��ʼ��ͼ�񴰿�
void MyGroupBox::initGroupBox(bool bOneGroup)
{
	QVBoxLayout* vBox = new QVBoxLayout;
	vBox->setContentsMargins(0, 15, 0, 0);
	vector<QHBoxLayout*> vecHBoxs;
	for (int i = 0; i < currentCameraData->Graphic_Lay.size(); i++)//ˮƽ���ֵĸ���
	{
		QHBoxLayout* hBox = new QHBoxLayout;
		hBox->setContentsMargins(2, 0, 0, 2);
		vecHBoxs.push_back(hBox);
	}

	int iIndex = 0;
	int i = 0;
	for (unordered_map<QString, oneCCDNameAndSize>::iterator itCCD = currentCameraData->mapCCD_NamesAndSizes.begin(); itCCD != currentCameraData->mapCCD_NamesAndSizes.end(); itCCD++)
	{
		int iSubSize = 0; //�Ӵ��ڸ���
		for (int j = 0; j < currentCameraData->Graphic_Lay.size(); j++) //���㵱ǰ���������ĸ�ˮƽ����
		{
			iSubSize += currentCameraData->Graphic_Lay[j];

			if (i <= (iSubSize - 1))
			{
				iIndex = j;
				break;
			}
		}
		QVBoxLayout* vBoxAddSubWnd = new QVBoxLayout;
		vBoxAddSubWnd->setContentsMargins(0, 0, 0, 0);

		//ͼ��QImageView	
		QImageView* imageView = new QImageView(this, itCCD->second.CCD_Size);
		imageView->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Ignored);
		//���
		TableView* tableView = new TableView(itCCD->second.CCD_Size.width(), itCCD->second.CCD_Size.height());
		tableView->hide();  //�������
		/*
		//����
		CurveWidget* curveWidget = new CurveWidget(this, itCCD->second.CCD_Size);
		//curveWidget->setTitle(QStringLiteral("����ͼ"),QColor(199, 21, 133)); //���߱���
		curveWidget->addLegned(false, 8);                                       //���ͼ��
		curveWidget->setDataAreacolor(QColor(80, 80, 80), QColor(30, 30, 30)); //��������������ɫ
		curveWidget->setOutAreacolor(QColor(80, 80, 80), QColor(50, 50, 50));  //���������ⲿ��ɫ

		OneLineData oneLineData;
		oneLineData.lineColor = QColor(220, 220, 220, 255);
		oneLineData.dLineWidth = 2;
		oneLineData.pointColor = QColor(255, 0, 0, 255);
		oneLineData.dPointWidth = 3;
		oneLineData.brushColor = QColor(255, 0, 0, 255);
		oneLineData.dBrushWidth = 3;
		oneLineData.strLengedName = "";
		curveWidget->addLine(itCCD->second.iCCD_Analysis_ID, oneLineData);
		curveWidget->hide();//��������
		*/
		//ͼ�����+��ť
		if (!bOneGroup || (currentCameraData->mapCCD_NamesAndSizes.size() != 1))
		{			
			QLabel* label = new QLabel(itCCD->second.strCCD_Name);
			label->setAlignment(Qt::AlignCenter);

			QPushButton* btn = new QPushButton();
			btn->setStyleSheet( //���ð�ť��ʽ
				"QPushButton{border-image:url(:/GraphicCollectAndProcess/image/max.png);}"//��ť�߿�ͼƬ
				"QPushButton:hover{border-image:url(:/GraphicCollectAndProcess/image/max_hover.png);}");//���ѡ�к�߿򱳾�ͼƬ
			btn->setFixedWidth(15);
			User *pUser = new User();
			pUser->nID = itCCD->second.strCCD_ID;
			pUser->strParentID = currentCameraData->Camera_ID;
			pUser->iFlag = 0;
			pUser->strName = itCCD->second.strCCD_Name;
			btn->setUserData(Qt::UserRole, pUser);
			QHBoxLayout* hbTemp = new QHBoxLayout;
			hbTemp->addStretch(0);
			hbTemp->addWidget(label);
			//hbTemp->addStretch(0);
			hbTemp->addWidget(btn);
			hbTemp->addStretch(0);
			connect(btn, SIGNAL(clicked()), this, SLOT(slot_btnClicked()));
			vBoxAddSubWnd->addLayout(hbTemp);
		}		
		vBoxAddSubWnd->addWidget(imageView);
		vBoxAddSubWnd->addWidget(tableView);
		//vBoxAddSubWnd->addWidget(curveWidget);
		QWidget* widget = new QWidget;
		widget->setContentsMargins(0, 0, 0, 0);
		widget->setLayout(vBoxAddSubWnd);

		vecHBoxs[iIndex]->addWidget(widget);

		mapNameToGraphic.insert(pair<QString, QImageView*>(itCCD->second.strCCD_ID, imageView));
		mapNameToTable.insert(pair<QString, TableView*>(itCCD->second.strCCD_ID, tableView));
		//mapNameToCurve.insert(pair<QString, CurveWidget*>(itCCD->second.strCCD_ID, curveWidget));
		mapNameToWidget.insert(pair<QString, QWidget*>(itCCD->second.strCCD_ID, widget));
		i++;
	}
	for (int i = 0; i < vecHBoxs.size(); i++)
	{
		vBox->addLayout(vecHBoxs[i]);
		//vBox->setStretch(i, 1);
	}
	setLayout(vBox);
}


