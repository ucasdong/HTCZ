#pragma once

#include <QtWidgets/QWidget>
#include "ui_GraphicCollectAndProcess.h"
#include "DataTable.h"
#include "ShowSetting.h"
#include "MyGroupBox.h"
#include "ConfigIni.h"
#include "HistoryReplay.h"
#include <QTimer>
#include <QMenu>
//#include "../../Model/DataAnalysisModel.h"

class DataAnalysisModel;
class GraphicCollectAndProcess : public QWidget
{
	Q_OBJECT

public:
	GraphicCollectAndProcess(oneScreenParams* oneScreenTemp);
	~GraphicCollectAndProcess();

	void initWindows();                                     //���ڳ�ʼ��
	void setDataAnalysisModel(DataAnalysisModel* dataModelTemp);

private:
	Ui::GraphicCollectAndProcessClass ui;
	DataAnalysisModel* dataModel;

	bool bStartOrStopGrab;          //��ʼ��ֹͣ�ɼ�
	QPoint m_pressPos;              //��갴��λ��
	bool bPress;                    //����Ƿ���
	int iKeyFlag;                   //esc������־��1������״̬��2�����ع�������3�����ر�����
	QTimer* timer;                   
	QMenu* pop_menu;                //�Ҽ��˵�
	int iFlagDataType;              //�б�ʮ���ƻ���ʮ��������ʾ(0:ʮ���ƣ�1ʮ������)
	oneScreenParams* currentScreen;  //��ǰ��Ļ���ò���
	int iStartBit;                  //���ݽ�����ʼλ

	ShowSetting* showSettingWnd;    //��ʾ���ý���
	//SaveSettingWnd* saveWnd;        //�洢����
	DataTable* dataTableFZ;         //���������б����
	HistoryReplay* rePlayWnd;       //��ʷ�طŽ���

	map<QString, QSize> cameraTypeToFZsize;           //������Ͷ�Ӧ�������ݳ���
	map<QString, MyGroupBox*> mapTypeToGraphicGroup;      //����-ͼ�񴰿�ӳ��
	map<QString, bool> mapTypeToShowState;                //����-��ʾ��������                          

	void initConfigFile();          //���ó�ʼ��
	void createMenu();              //�����˵�
	void setDataShowType(int iFlag);//������ʾ����

public slots:
	void on_closeButton_clicked();//�ر�
	void on_minButton_clicked();  //��С��
	void on_maxButton_clicked();  //���
	void on_pushButton_hideOrshow_clicked();//������ʾ�Ҳศ�������б�

	void on_toolButton_YaoceYaokong_clicked();//���ң��ң��
	void on_toolButton_TableDisplay_clicked();//����������ʾ����
	void on_toolButton_DataReplay_clicked();  //������ݻط�
	void on_toolButton_ShowSetting_clicked(); //��ʾ���ô���
	void on_toolButton_SaveSetting_clicked(); //�洢����
	void on_toolButton_StartColl_clicked();   //��ʼֹͣ�ɼ�
	void slot_HistoryReplay(QString strFilePath, bool bStartOrStop); //���ݻط�

	void slot_ActionGraphic();                //ͼ����ʾ
	void slot_ActionTable();                  //�����ʾ
	void slot_ActionLine();                   //������ʾ
	void slot_DataShowType();                 //�����������(ʮ���ƻ���ʮ������)

	void slot_StartBitChanged(int iStartBitTemp);                  //���ݽ���ƫ��
	void slot_ModelChanged(QString model);                         //����ģʽ�л�
	void slot_MaxWnd(QPushButton* btn);                            //�������
	void slot_TimerOut();

 signals:
 	void signal_StartOrStopGrab(bool);                             //��ʼ����ֹͣ�ɼ� 
	void signal_SaveData();
	void signal_ReplayData(QString, bool);
	void signal_ChangeModel(int iGraphicWidth);

protected://��������϶�
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	void contextMenuEvent(QContextMenuEvent *event);
	void keyPressEvent(QKeyEvent *event);

	//void resizeEvent(QResizeEvent *event);
};
