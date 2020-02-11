#include <QGraphicsItem>
#include <QFont>
#include "PixItem.h"

class QImageCallout : public QGraphicsItem
{
public:
	QImageCallout(PixItem* pixItem);

    void setText(const QString &text); //消息框显示内容
    void setAnchor(QPointF point, QPointF offset); //消息框作下角坐标
    void updateGeometry();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    QString m_text;
    QRectF m_textRect;
    QRectF m_rect;
    QPointF m_anchor;
	QPointF m_anchorOffset;

    QFont m_font;
	PixItem *m_pPixItem;
};
