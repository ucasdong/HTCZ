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
	int iShowColumn;
	TableModel* tableModel;     //���ģ��
	BYTE* graphicData;          //�������
	bool bPress;                //����Ƿ�ѹ
	QLabel* toolTipLabel;       //�����ʾ��
	int iDataTypeFlag;          //������ʾ����(ʮ���ƻ���ʮ������)
	bool bShowTip;              //�����Ƿ���ʾ��ʾ��Ĭ����ʾtrue��
	bool bChangeShow;

	void initHHeader();
	void initVHeader();
	void recvTableData(BYTE* dataShow);                         //�����ʾ
	void setDataShowFlag(int iFlag);                            //������ʾ����(ʮ���ƻ���ʮ������)
	void setMouseTip(bool bHave);                               //���������ʾ
	void changeShowWidth(int iWidth);                           

// protected:
// 	void mousePressEvent(QMouseEvent *event);
// 	void mouseMoveEvent(QMouseEvent *event);
// 	void mouseReleaseEvent(QMouseEvent *event);
};
