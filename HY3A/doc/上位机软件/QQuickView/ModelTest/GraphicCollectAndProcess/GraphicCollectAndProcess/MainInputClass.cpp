#include "MainInputClass.h"
#include "../../Model/DataAnalysisModel.h"

MainInputClass::MainInputClass()
{
}

void MainInputClass::startWindow()
{
	//进度提示框
	QPixmap pixmap;
	pixmap.load(":/GraphicCollectAndProcess/image/splash_old.png");
	//pixmap.scaled(400, 560, Qt::KeepAspectRatioByExpanding);
	QSplashScreen splash(pixmap);
	//splash.setWindowOpacity(0.9);
	splash.showMessage("Loading......", Qt::AlignCenter, Qt::white);
	splash.show();
	
	//获取当前屏幕个数
	QDesktopWidget *desktop = QApplication::desktop();
	int iScreenCount = desktop->screenCount();

	map<int, oneScreenParams>* mapScreens = ConfigIni::getInstance()->getAllWndParams();
	int iScreenSize = mapScreens->size();//配置文件中的屏幕个数
	if (iScreenSize != 1)
	{
		if (iScreenCount != iScreenSize)
		{
			QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("当前屏幕个数与配置不一致！"));
			return;
		}
	}
	
	dataAnalysisModel = new DataAnalysisModel();
	for (map<int, oneScreenParams>::iterator it = mapScreens->begin(); it != mapScreens->end(); it++)
	{
		GraphicCollectAndProcess *Wnd = new GraphicCollectAndProcess(&(it->second));
		Wnd->setDataAnalysisModel(dataAnalysisModel);
		for (vector<oneCameraParams*>::iterator itWnd = it->second.vecCameraParams.begin(); itWnd != it->second.vecCameraParams.end(); itWnd++)
		{
			vecAllCameraParams.push_back(*itWnd);
			mapCameraToGraphicWnd.insert(pair<QString, GraphicCollectAndProcess*>((*itWnd)->Camera_ID, Wnd));
		}
		
		if (mapScreens->size() == 1)
		{
			Wnd->setGeometry(desktop->screenGeometry(desktop->primaryScreen()));
		}
		else
		{
			Wnd->setGeometry(desktop->screenGeometry(it->first));
		}
		Wnd->showMaximized();
		splash.finish(Wnd);
	}
	dataAnalysisModel->AddCameraList(vecAllCameraParams);

	dataAnalysisModel->InitDriverAndDataAnalysis();
	//dataAnalysisModel->StartGrabData();
}

MainInputClass::~MainInputClass()
{
	delete dataAnalysisModel;
	dataAnalysisModel = NULL;
}

