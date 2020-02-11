#pragma once

#include <QObject>
#include "GraphicCollectAndProcess.h"

class DataAnalysisModel;
class MainInputClass : public QObject
{
	Q_OBJECT

public:
	MainInputClass();
	void startWindow();//∆Ù∂ØΩÁ√Ê
	~MainInputClass();

	DataAnalysisModel* dataAnalysisModel;
	map<QString, GraphicCollectAndProcess*> mapCameraToGraphicWnd;
	vector<oneCameraParams*> vecAllCameraParams;
};
