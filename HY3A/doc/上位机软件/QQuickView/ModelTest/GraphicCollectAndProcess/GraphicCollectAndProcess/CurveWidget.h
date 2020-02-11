#pragma once

#include <QWidget>
#include "qcustomplot.h"

struct OneLineData 
{
	QColor lineColor;     //线条颜色
	double dLineWidth;    //线条宽度
	QColor pointColor;    //点颜色
	double dPointWidth;   //点的直径
	QColor brushColor;   //点画刷颜色
	double dBrushWidth;  //点画刷宽度
	QString strLengedName;//图例名称
};
class CurveWidget : public QWidget
{
	Q_OBJECT

public:
	CurveWidget(QWidget* parent, QSize wndSize);
	~CurveWidget();

	void setTitle(QString strTitleName,QColor titleColor); //设置标题
	void addLegned(bool bAdd, int fontSize);               //添加图例 
	void setDataAreacolor(QColor color0, QColor color1);   //设置数据区域的背景颜色
	void setOutAreacolor(QColor color0, QColor color1);    //设置数去外部区域的背景颜色

	void addLine(int channelID, OneLineData oneLineData);                        //控件中添加线条
	void updateData(int channelID, QVector<double>& vecX, QVector<double>& vecY);//更新数据
	void recvData(int channelID, BYTE* dataShow);                                //数据更新
	void removeLine(int channelID);         //移除曲线
	void removeAllLines();                          //移除所有曲线

private:
	QCustomPlot* m_CurveWidget;                     //曲线图控件
	QCPPlotTitle* m_qTitle;                         //标题
	bool bHaveLegned;                               //是否有图例
	std::map<int, QCPGraph*> mapGraphs;         //当前所有的图线
	QVector<double> vecGraphX;                      //横坐标
	QVector<double> vecGraphY;                      //纵坐标
	int iC;
	QSize wndDataSize;                              //曲线横纵坐标范围
};


