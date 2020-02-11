#pragma once

#define LEFT_SPACE 30
#define TOP_SPACE 40
#define INDICATOR_HEIGHT 30

#include <QGraphicsItem>
#include <QPainter>
#include <QPointF>
#include <QRectF>
#include <QGraphicsWidget>

class QBackRuleGraphicsItem : public QGraphicsItem
{
public:
	QBackRuleGraphicsItem(QGraphicsItem *parent = nullptr);
	~QBackRuleGraphicsItem();

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget); //重画图形函数
	QRectF boundingRect() const;    //实现自己的boundingRect 图元边界方法，完成以图元坐标系为基础增加两个像素点的冗余的工作
private:
	void drawRule(QPainter* painter);
	void drawHandle(QPainter* painter);
	void drawHorizontalLine(QPainter* painter);
	void drawVerticalLines(QPainter* painter);

private:
	int m_nMin;
	int m_nMax;
	int m_length;
	qreal m_value;
	qreal m_longHand;
	qreal m_okHand;
	qreal m_shortHand;
	qreal m_handleWidth;
	qreal m_handleHeight;
	qreal m_indicatorLength;
	QPointF m_indicatorTopPot;
	QPointF m_indicatorLastPot;
	QPointF m_indicatorBottomLeftPot;
	QPointF m_indicatorBottomRightPot;
	QPointF m_lineLeftPot;
	QPointF m_lineRightPot;
	QRectF  m_handleRect;
};
