#pragma once

#include <QWidget>
#include "qcustomplot.h"

struct OneLineData 
{
	QColor lineColor;     //������ɫ
	double dLineWidth;    //�������
	QColor pointColor;    //����ɫ
	double dPointWidth;   //���ֱ��
	QColor brushColor;   //�㻭ˢ��ɫ
	double dBrushWidth;  //�㻭ˢ���
	QString strLengedName;//ͼ������
};
class CurveWidget : public QWidget
{
	Q_OBJECT

public:
	CurveWidget(QWidget* parent, QSize wndSize);
	~CurveWidget();

	void setTitle(QString strTitleName,QColor titleColor); //���ñ���
	void addLegned(bool bAdd, int fontSize);               //���ͼ�� 
	void setDataAreacolor(QColor color0, QColor color1);   //������������ı�����ɫ
	void setOutAreacolor(QColor color0, QColor color1);    //������ȥ�ⲿ����ı�����ɫ

	void addLine(int channelID, OneLineData oneLineData);                        //�ؼ����������
	void updateData(int channelID, QVector<double>& vecX, QVector<double>& vecY);//��������
	void recvData(int channelID, BYTE* dataShow);                                //���ݸ���
	void removeLine(int channelID);         //�Ƴ�����
	void removeAllLines();                          //�Ƴ���������

private:
	QCustomPlot* m_CurveWidget;                     //����ͼ�ؼ�
	QCPPlotTitle* m_qTitle;                         //����
	bool bHaveLegned;                               //�Ƿ���ͼ��
	std::map<int, QCPGraph*> mapGraphs;         //��ǰ���е�ͼ��
	QVector<double> vecGraphX;                      //������
	QVector<double> vecGraphY;                      //������
	int iC;
	QSize wndDataSize;                              //���ߺ������귶Χ
};


