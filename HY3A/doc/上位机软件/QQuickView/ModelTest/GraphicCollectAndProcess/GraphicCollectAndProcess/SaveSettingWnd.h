#pragma once

#include <QWidget>
#include "ui_SaveSettingWnd.h"
#include "SaveData.h"
#include "Common.h"
#include "CodeDataAnalysis.h"
#include "ConfigIni.h"
#include <QCheckBox>
#include <QRadioButton>

class SaveSettingWnd : public QWidget
{
	Q_OBJECT

public:
	SaveSettingWnd();
	~SaveSettingWnd();

	void insertSaveData(QString strName, BYTE* saveDataTemp, int iLen); //����Ҫ�洢������
	//int getSaveFlag();
private:
	Ui::SaveSettingWnd ui;

	CodeDataAnalysis codeAnalysis;           //���ݽ���

	QPoint m_pressPos;                       //�����ƶ����λ��
	bool bPress;                             //�����ƶ�����Ƿ���
	
    //�������-���ѡ��
	map<QString, QCheckBox*> mapCameraIdToCheckBox;
	map<QString, bool> mapDataSaveFlag;

	//�������-�׶�ѡ��
	map<QString, QRadioButton*> mapPDIdToRadioBtn;
    map<QString, bool> mapPDFlag;
	int iPDFlag;                             //0:ԭʼ���ݣ�1�����ն�
	//�������-��������  
	bool bSaveSetting;                       //�洢�����Ƿ�ɹ�
	QString strSavePath;                     //�������ݴ洢·��
	QString strFzPath;                       //�������ݴ��·��
//	int iSaveFlag;

	bool bSaveType;                          //�洢���ͣ���ʱ��洢/Լ�������洢
	bool bStartOrStop;                       //��ʼ����ֹͣ�洢
	bool bPauseOrContinue;                   //��ͣ���߼���
	//�������-��ʱ��洢����
	QString strInformation1;                 //�Զ���˵������
	double dSaveTime;                        //�洢ʱ��
	//�������-Լ�������洢����
	int iSaveCount;                          //�洢����
	QString strInformation2;                 //�Զ���˵������
	int iFileType;                           //�ļ���ʽ 1:Raw, 2:Bmp, 3:TiFF
	QString strFileFormat;

	//���ƴ洢����
	qint64 qintStartTime;                    //��ʼ�洢ʱ��
	qint64 iPauseCount;                      //Լ����������
	qint64 iPauseCountFZ;                    //Լ����������
	//���նδ洢
	QStringList strNameList;
//	BYTE* P_Data;                                //p�ն�����
//	BYTE* B1_Data;                               //b1�ն�����
//	BYTE* B2_Data;                               //b2�ն�����
//	BYTE* B3_Data;                               //b3�ն�����
//	BYTE* B4_Data;                               //b4�ն�����

	//������ʱ��洢ʹ���ڴ�ӳ���ļ��Ĵ洢��ʽ
	std::map<QString, SaveData*> mapTypeToAllData;   //�洢��������
	std::map<QString, SaveData*> mapTypeToFZData;    //�洢��������
	std::map<QString, std::map<QString, SaveData*>> mapTypeToPuDuan;    //�洢�ն�����
	//Լ�������洢����ǰ���÷������Ȱ��������ݴ浽һ���ļ��У��洢��������ļ�������Ҫ��ǰ�����ļ�����
	QString qintStartPauseTime;                      //��ʼ�洢Լ�����ļ�ʱ��
	std::map<QString, QString> mapPauseCountFile;    //Լ�������ļ�·��  
	std::map<QString, SaveData*> mapTypeToPauseData; //�洢Լ����������

//	void PausefilesSplit();                          //Լ���ļ����
//	QString getFZFileName(BYTE* dataTemp);

public slots:
	void on_closeButton_clicked();
	void on_minButton_clicked();
	void on_maxButton_clicked();
	void on_pushButton_ChoosePath_clicked();           //ѡ�����ݴ洢·��
	//void on_pushButton_ChoosePathFz_clicked();         //ѡ�������ݴ洢·��
	void on_pushButton_sure_clicked();                 //�����������
	void on_pushButton_StartOrStop_clicked();          //�����ʼ�洢
	void on_pushButton_PauseOrContinue_clicked();      //�����ͣ�洢
	void on_groupBox_LongTime_clicked(bool bCheck);    //�����ʱ��洢ģʽ
	void on_groupBox_FixedRow_clicked(bool bCheck);    //���Լ������ģʽ
	void on_PDRadioButton_clicked();
//	void on_pushButton_SaveCCSDS_clicked();

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};
