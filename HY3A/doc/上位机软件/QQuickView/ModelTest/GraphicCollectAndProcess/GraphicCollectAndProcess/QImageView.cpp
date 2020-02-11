#include "QImageView.h"
#include <QGraphicsScene>

#define VIEW_CENTER viewport()->rect().center()
#define VIEW_WIDTH  viewport()->rect().width()
#define VIEW_HEIGHT viewport()->rect().height()


QImageView::QImageView(QWidget *parent, QSize WndSize)
	: QGraphicsView(parent),
	wndSize(WndSize),
	m_scale(1.0),
	m_zoomDelta(0.1),
	m_bMouseTranslate(false),
	m_bToolTipMove(false),
	iDataTypeFlag(1)	
{
	// 去掉滚动条
	//setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setCursor(Qt::PointingHandCursor);
	setRenderHint(QPainter::Antialiasing);
	iShowWidth = WndSize.width();
	QGraphicsScene *m_graphicsScene = new QGraphicsScene;
	m_graphicsScene->setSceneRect(-wndSize.width() / 2.0, -wndSize.height() / 2.0, wndSize.width(), wndSize.height());
	QBrush brush(QColor(0, 0, 0));
	m_graphicsScene->setBackgroundBrush(brush);
	setScene(m_graphicsScene);
	m_pixItem = new PixItem(wndSize);
	m_graphicsScene->addItem(m_pixItem);
	m_pixItem->setPos(0, 0);
// 	graphicLabel = new GraphicLabel(NULL, wndSize);
// 	m_graphicsScene->addWidget(graphicLabel);
	textItem = new QGraphicsTextItem;
	textItem->setPlainText("");
	textItem->setDefaultTextColor(QColor(0, 255, 0, 255));
	textItem->adjustSize();
	textItem->setTextWidth(150);
	textItem->hide();
	textItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);

	m_graphicsScene->addItem(textItem);

	centerOn(-wndSize.width() / 2.0, -wndSize.height() / 2.0);
}

QImageView::~QImageView()
{
}

//接收并显示数据
void QImageView::recvData(BYTE* data, QPixmap* pixmap)
{
	m_pixItem->recvData(data, pixmap);
	scene()->update();
// 	graphicLabel->recvData(data, pixmap);
// 	scene()->update();
}

void QImageView::setDataShowFlag(int iFlag)
{
	iDataTypeFlag = iFlag;
}

void QImageView::changeShowWidth(int iWidth)
{
	iShowWidth = iWidth;
}

void QImageView::translate(QPointF delta)
{
	// 根据当前 zoom 缩放平移数
	delta *= m_scale;
	//delta *= m_translateSpeed;

	// view 根据鼠标下的点作为锚点来定位 scene
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	QPoint newCenter(VIEW_WIDTH / 2 - delta.x(), VIEW_HEIGHT / 2 - delta.y());
	centerOn(mapToScene(newCenter));

	// scene 在 view 的中心点作为锚点
	setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}

void QImageView::zoomIn()
{
	zoom(1 + m_zoomDelta);
}

void QImageView::zoomOut()
{
	zoom(1 - m_zoomDelta);
}

void QImageView::zoom(float scaleFactor)
{
	// 防止过小或过大
	qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100)
		return;

	scale(scaleFactor, scaleFactor);
	m_scale *= scaleFactor;
}

void QImageView::mouseMoveEvent(QMouseEvent *event)
{
	if (m_bMouseTranslate) 
	{
		QPointF mouseDelta = mapToScene(event->pos()) - mapToScene(m_lastMousePos);
		translate(mouseDelta);
	}
	else if (m_bToolTipMove)
	{
		QPointF point = mapToScene(event->pos());
		int iX = int(point.x()) + wndSize.width() / 2;
		int iY = int(point.y()) + wndSize.height() / 2;
		if (point.x() < 0)
		{
			iX--;
		}
		if (point.y() < 0)
		{
			iY--;
		}
		if (iX < 0 || iX >= iShowWidth || iY < 0 || iY >= wndSize.height())
		{
			return;
		}

		QPointF mouseDelta = mapToScene(event->pos()) - mapToScene(m_lastMousePos);
		textItem->moveBy(mouseDelta.x(), mouseDelta.y());
		
		QString strData = m_pixItem->getPointData(iX, iY,iDataTypeFlag);//数值
		QString strShow = QString("   ") + strData + QString("(") + QString::number(iX + 1) + QString(",") + QString::number(iY + 1) + QString(")");
		textItem->setPlainText(strShow);
	}
	m_lastMousePos = event->pos();
	QGraphicsView::mouseMoveEvent(event);
}

void QImageView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		// 当光标底下没有 item 时，才能移动
		QPointF point = mapToScene(event->pos());
		if (scene()->itemAt(point, transform()) == NULL) 
		{
			m_bMouseTranslate = true;
			m_lastMousePos = event->pos();
		}
		else
		{
			int iX = int(point.x()) + wndSize.width() / 2;
			int iY = int(point.y()) + wndSize.height() / 2;
			if (point.x() < 0)
			{
				iX--;
			}
			if (point.y() < 0)
			{
				iY--;
			}
			if (iX < 0 || iX >= iShowWidth || iY < 0 || iY >= wndSize.height())
			{
				return;
			}
			m_bToolTipMove = true;
			textItem->setPos(point);
			textItem->show();
			QString strData = m_pixItem->getPointData(iX, iY, iDataTypeFlag);//数值
			QString strShow = QString("   ") + strData + QString("(") + QString::number(iX + 1) + QString(",") + QString::number(iY + 1) + QString(")");
			textItem->setPlainText(strShow);
		}
	}
	QGraphicsView::mousePressEvent(event);
}

void QImageView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_bMouseTranslate = false;
		m_bToolTipMove = false;
		textItem->hide();
	}
	QGraphicsView::mouseReleaseEvent(event);
}

void QImageView::wheelEvent(QWheelEvent *event)
{
	// 滚轮的滚动量
	QPoint scrollAmount = event->angleDelta();
	// 正值表示滚轮远离使用者（放大），负值表示朝向使用者（缩小）
	scrollAmount.y() > 0 ? zoomIn() : zoomOut();
}

