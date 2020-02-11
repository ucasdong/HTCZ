#pragma once

#include <QGroupBox>
#include "ConfigIni.h"
#include "Common.h"
#include "QImageView.h"
#include "CurveWidget.h"
#include "TableView.h"
#include "CodeDataAnalysis.h"
#include <vector>
#include <map>

class MyGroupBox : public QGroupBox
{
	Q_OBJECT

public:
	MyGroupBox(oneCameraParams* oneData, bool bOneGroup);
	~MyGroupBox();

	FILE* fileWrite;  
	virtual void showData();                                   //显示数据

	void showOrHide(QString strID, int iFlag);                 //最大化显示
	void setBitStart(int iStartBitTemp);                       //设置数据解析起始位
	void setDataShowType(int iFlag);                           //设置显示类型(图片、表格、曲线)
	void setDataShowFlag(int iFlag);                           //数据显示类型(十进制或者十六进制)
	void changeModel(QString strModel);                        
	oneCameraParams* currentCameraData;                         //图像窗口配置
	int iFlagType;                                             //显示类型：图像，表格，曲线
	int iStartBit;                                             //数据解析起始位

	bool bHaveNewData;
	bool bThreadStop;
	bool bAnalysisFinish;

	int iCurrentTime;                            //当前刷新数据次数
	bool bShowGraphic;                           //显示图像
	bool bShowTable;                             //显示表格
	bool bShowCurve;                             //显示曲线

private:	
	void initGroupBox(bool bOneGroup);   //初始化图像窗口
	void createBuffer();                                       //创建缓冲区

	map<QString, QImageView*> mapNameToGraphic;                //子类型-图像窗口映射
	map<QString, TableView*> mapNameToTable;                   //子类型-表格窗口映射
	map<QString, CurveWidget*> mapNameToCurve;                 //子类型-曲线窗口映射
	map<QString, QWidget*> mapNameToWidget;                    //子类型-窗口（最大化还原设置）

signals:
	void btnMaxClicked(QPushButton*);

public slots:
	void slot_btnClicked();
};
