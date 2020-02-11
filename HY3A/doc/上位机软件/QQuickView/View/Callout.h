#include <QtCharts>
#include <QGraphicsItem>
#include <QFont>

QT_CHARTS_USE_NAMESPACE //相当于 using namespace QtCharts;

class Callout : public QGraphicsItem
{
public:
    Callout(QChart* chart);

    void setText(const QString &text); //消息框显示内容
    void setAnchor(QPointF point); //消息框作下角坐标
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
    QFont m_font;
    QChart *m_chart;
};
