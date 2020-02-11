#include "CurveWidget.h"
#include <QVBoxLayout>

CurveWidget::CurveWidget(QWidget* parent, QSize wndSize)
	:QWidget(parent),
	m_qTitle(NULL),
	wndDataSize(wndSize)
//	m_CurveWidget(NULL)
{
	setMouseTracking(true);
	this->setContentsMargins(0, 0, 0, 0);
 	m_CurveWidget = new QCustomPlot(this);
 	QVBoxLayout* vBox = new QVBoxLayout;
 	vBox->setContentsMargins(0, 0, 0, 0);
 	vBox->addWidget(m_CurveWidget);
 	this->setLayout(vBox);
	m_CurveWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	//�����û����Զ�customPlot������Щ���� 
	m_CurveWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes/* | QCP::iSelectLegend | QCP::iSelectPlottables*/);

	//m_CurveWidget->axisRect()->setRangeDrag(Qt::Vertical);
	m_CurveWidget->axisRect()->setRangeZoom(Qt::Horizontal);

	//�����������ǩ����
	m_CurveWidget->xAxis->setLabelColor(QColor(255, 255, 255, 255));
	m_CurveWidget->xAxis->setLabel("x");
	m_CurveWidget->xAxis->setTickLabelColor(QColor(255, 255, 255, 255));
	m_CurveWidget->yAxis->setLabelColor(QColor(255, 255, 255, 255));
	m_CurveWidget->yAxis->setTickLabelColor(QColor(255, 255, 255, 255));
	m_CurveWidget->yAxis->setLabel(QStringLiteral("DN"));
	QVector<double> vecInit(wndDataSize.width(), 0);
	for (int i = 0;i < wndDataSize.width(); i++)
	{
		vecInit.replace(i, i);
	}
	vecGraphX = vecInit;
	vecGraphY = vecInit;
	iC = 0;
}

CurveWidget::~CurveWidget()
{
// 	if (m_CurveWidget)
// 	{
// 		removeAllLines();
// 		delete m_CurveWidget;
// 		m_CurveWidget = NULL;
// 	}
	if (m_qTitle)
	{
		delete m_qTitle;
		m_qTitle = NULL;
	}
}

//���ñ���
void CurveWidget::setTitle(QString strTitleName, QColor titleColor)
{
	if (m_qTitle = NULL)
	{
		delete m_qTitle;
		m_qTitle = NULL;
	}
	m_qTitle = new QCPPlotTitle(m_CurveWidget, strTitleName);
	m_qTitle->setTextColor(titleColor);
	m_CurveWidget->plotLayout()->insertRow(0);
	m_CurveWidget->plotLayout()->addElement(0, 0, m_qTitle);
}

//���ͼ�� 
void CurveWidget::addLegned(bool bAdd,int fontSize)
{
	bHaveLegned = bAdd;
	if (bAdd)
	{
		m_CurveWidget->legend->setVisible(true);
		QFont legendFont = font();
		legendFont.setPointSize(fontSize);
		m_CurveWidget->legend->setFont(legendFont);
		m_CurveWidget->legend->setSelectedFont(legendFont);
		m_CurveWidget->legend->setSelectableParts(QCPLegend::spItems);
	}
}

//������������ı�����ɫ
void CurveWidget::setDataAreacolor(QColor color0, QColor color1)
{
	QLinearGradient axisRectGradient;
	axisRectGradient.setStart(0, 0);
	axisRectGradient.setFinalStop(0, 350);
	axisRectGradient.setColorAt(0, color0);
	axisRectGradient.setColorAt(1, color1);
	m_CurveWidget->axisRect()->setBackground(axisRectGradient);
}

//������ȥ�ⲿ����ı�����ɫ
void CurveWidget::setOutAreacolor(QColor color0, QColor color1)
{
	QLinearGradient plotGradient;
	plotGradient.setStart(0, 0);
	plotGradient.setFinalStop(0, 350);
	plotGradient.setColorAt(0, color0);
	plotGradient.setColorAt(1, color1);
	m_CurveWidget->setBackground(plotGradient);
}

//�ؼ����������
void CurveWidget::addLine(int channelID, OneLineData oneLineData)
{
	QCPGraph* newGraph = m_CurveWidget->addGraph();
	newGraph->setPen(QPen(oneLineData.lineColor, oneLineData.dLineWidth));//����������ɫ
	newGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
		QPen(oneLineData.pointColor, oneLineData.dPointWidth),
		QBrush(oneLineData.brushColor), oneLineData.dBrushWidth));//���õ����ɫ�Ϳ��
	if (bHaveLegned)
	{
		newGraph->setName(oneLineData.strLengedName);//Ϊͼ������
	}

	m_CurveWidget->xAxis->setTickStep(1);      //���ü��1
	m_CurveWidget->xAxis->setRange(-1, 150);   //�����귶Χ
	m_CurveWidget->yAxis->setRange(-1, 256);   //�����귶Χ
	//this->replot();//��������
	mapGraphs.insert(std::pair<int, QCPGraph*>(channelID, newGraph));
}

//��������
void CurveWidget::updateData(int channelID, QVector<double>& vecX, QVector<double>& vecY)
{
	std::map<int, QCPGraph*>::iterator it = mapGraphs.find(channelID);
	if (it != mapGraphs.end())
	{
		it->second->clearData();
		it->second->setData(vecX, vecY);
		m_CurveWidget->replot();
	}
}

//��������ݸ���
void CurveWidget::recvData(int channelID, BYTE* dataShow)
{
	for (int i = 0; i < wndDataSize.width(); i++)
	{
		vecGraphY.replace(i, *(dataShow + i));
	}

	std::map<int, QCPGraph*>::iterator it = mapGraphs.find(channelID);
	if (it != mapGraphs.end())
	{
		it->second->clearData();
		it->second->setData(vecGraphX, vecGraphY);
	}
	m_CurveWidget->replot();//��������
}

//�Ƴ�����
void CurveWidget::removeLine(int channelID)
{
	std::map<int, QCPGraph*>::iterator it = mapGraphs.find(channelID);
	if (it != mapGraphs.end())
	{
		m_CurveWidget->removeGraph(it->second);
		mapGraphs.erase(it);
	}
}

//�Ƴ���������
void CurveWidget::removeAllLines()
{
	for (std::map<int, QCPGraph*>::iterator it = mapGraphs.begin(); it != mapGraphs.end();)
	{
		m_CurveWidget->removeGraph(it->second);
		it = mapGraphs.erase(it);
	}
}

