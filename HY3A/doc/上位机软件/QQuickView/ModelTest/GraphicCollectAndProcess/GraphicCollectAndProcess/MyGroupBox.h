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
	virtual void showData();                                   //��ʾ����

	void showOrHide(QString strID, int iFlag);                 //�����ʾ
	void setBitStart(int iStartBitTemp);                       //�������ݽ�����ʼλ
	void setDataShowType(int iFlag);                           //������ʾ����(ͼƬ���������)
	void setDataShowFlag(int iFlag);                           //������ʾ����(ʮ���ƻ���ʮ������)
	void changeModel(QString strModel);                        
	oneCameraParams* currentCameraData;                         //ͼ�񴰿�����
	int iFlagType;                                             //��ʾ���ͣ�ͼ�񣬱������
	int iStartBit;                                             //���ݽ�����ʼλ

	bool bHaveNewData;
	bool bThreadStop;
	bool bAnalysisFinish;

	int iCurrentTime;                            //��ǰˢ�����ݴ���
	bool bShowGraphic;                           //��ʾͼ��
	bool bShowTable;                             //��ʾ���
	bool bShowCurve;                             //��ʾ����

private:	
	void initGroupBox(bool bOneGroup);   //��ʼ��ͼ�񴰿�
	void createBuffer();                                       //����������

	map<QString, QImageView*> mapNameToGraphic;                //������-ͼ�񴰿�ӳ��
	map<QString, TableView*> mapNameToTable;                   //������-��񴰿�ӳ��
	map<QString, CurveWidget*> mapNameToCurve;                 //������-���ߴ���ӳ��
	map<QString, QWidget*> mapNameToWidget;                    //������-���ڣ���󻯻�ԭ���ã�

signals:
	void btnMaxClicked(QPushButton*);

public slots:
	void slot_btnClicked();
};
