#pragma once

#include <QTableView>
#include "TableModel.h"
#include <QLabel>
#include <QMouseEvent>

class TableView : public QTableView
{
	Q_OBJECT

public:
	TableView(int iColumn, int iRow);
	~TableView();

	int iRowCount;              //��ǰ��������
	int iColumnCount;           //��ǰ��������
	TableModel* tableModel;     //���ģ��
	BYTE* graphicData;          //�������
	bool bPress;                //����Ƿ�ѹ
	QLabel* toolTipLabel;       //�����ʾ��
	int iDataTypeFlag;          //������ʾ����(ʮ���ƻ���ʮ������)
	bool bShowTip;              //�����Ƿ���ʾ��ʾ��Ĭ����ʾtrue��
	
	void initHHeader();
	void initVHeader();
	void changeValue(BYTE* dataShow);
	void recvTableData(BYTE* dataShow);                         //�����ʾ
	void setDataShowFlag(int iFlag);                            //������ʾ����(ʮ���ƻ���ʮ������)
	void setMouseTip(bool bHave);                               //���������ʾ
	void changeModel(int iModel);

// protected:
// 	void mousePressEvent(QMouseEvent *event);
// 	void mouseMoveEvent(QMouseEvent *event);
// 	void mouseReleaseEvent(QMouseEvent *event);
};
