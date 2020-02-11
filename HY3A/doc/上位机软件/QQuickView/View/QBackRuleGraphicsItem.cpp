#include "QBackRuleGraphicsItem.h"

QBackRuleGraphicsItem::QBackRuleGraphicsItem(QGraphicsItem *parent)
	: QGraphicsItem(parent)
{
	m_nMin = 0;
	m_nMax = 100;
	m_length = m_nMax - m_nMin;
	m_value = 0;


}

QBackRuleGraphicsItem::~QBackRuleGraphicsItem()
{
}

void QBackRuleGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	//m_longHand = widget->width() / 3;
	//m_okHand = parentWidget()->size().height() / 4;
	//m_shortHand = parentWidget()->size().height() / 5;
	//m_handleWidth = parentWidget()->size().width() / 15;
	//m_handleHeight = parentWidget()->size().height() / 5;
	//m_indicatorLength = parentWidget()->size().height() / 5;

	painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	//resetVariables(&painter);
	//drawBackground(painter);
	drawRule(painter);
	drawHandle(painter);
	//drawTip(&painter);
}

QRectF QBackRuleGraphicsItem::boundingRect() const
{
	return QRectF(1000, 1000, 1000, 1000);
}

void QBackRuleGraphicsItem::drawRule(QPainter* painter)
{
	painter->save();
	drawHorizontalLine(painter);
	drawVerticalLines(painter);
	painter->restore();
}

void QBackRuleGraphicsItem::drawHandle(QPainter* painter)
{

}

void QBackRuleGraphicsItem::drawHorizontalLine(QPainter* painter)
{
	painter->save();
	painter->setPen(Qt::white);

	QPointF leftPot(LEFT_SPACE, TOP_SPACE + m_longHand);
	m_lineLeftPot = leftPot;

	QPointF rightPot(1000 - LEFT_SPACE, TOP_SPACE + m_longHand);
	m_lineRightPot = rightPot;
	painter->drawLine(leftPot, rightPot);
	painter->restore();
}

void QBackRuleGraphicsItem::drawVerticalLines(QPainter* painter)
{
	painter->save();
	painter->setPen(Qt::white);

	int length = m_nMax - m_nMin;
	qreal startX = LEFT_SPACE;
	qreal bottomY = m_lineLeftPot.y();
	qreal lineLenght = m_lineRightPot.x() - m_lineLeftPot.x();

	qreal increment = lineLenght / length;
	int startValue = m_nMin;
	QString strValue;
	for (int i = m_nMin; i <= m_nMax; i++)
	{
		if (i % 10 == 0)
		{
			QPointF topPot(startX, bottomY - m_longHand);
			QPointF bottomPot(startX, bottomY);
			painter->drawLine(topPot, bottomPot);
			strValue = QString("%1").arg(startValue);
			qreal textWidth = 15;
			qreal textHeight = 15;
			QPointF textPot(startX - textWidth / 2, bottomY + textHeight);
			painter->drawText(textPot, strValue);

			startValue += 10;
		}
		else if (i % 5 == 0)
		{
			QPointF topPot(startX, bottomY - m_okHand);
			QPointF bottomPot(startX, bottomY);
			painter->drawLine(topPot, bottomPot);
		}
		else
		{
			QPointF topPot(startX, bottomY - m_shortHand);
			QPointF bottomPot(startX, bottomY);
			painter->drawLine(topPot, bottomPot);
		}

		startX += increment;
	}
	painter->restore();
}

