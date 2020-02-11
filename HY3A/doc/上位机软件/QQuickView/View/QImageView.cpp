#include "QImageView.h"
#include <QGraphicsScene>
#include <QDebug>
#include "qslog.h"
#define VIEW_CENTER viewport()->rect().center()
#define VIEW_WIDTH  viewport()->rect().width()
#define VIEW_HEIGHT viewport()->rect().height()


QImageView::QImageView(QWidget *parent, QSize WndSize)
	: QGraphicsView(parent),
	m_pCallout(nullptr),
	wndSize(WndSize),
	m_scale(1.0),
	m_zoomDelta(0.1),
	m_bMouseTranslate(false),
	m_bFirstResize(false),
	m_bToolTipMove(false),
	//m_pWorker(new QImageViewWorker),
	m_pWorkThread(new QThread),
	iDataTypeFlag(1)
{
	m_hruler = new QtRuleBar(Qt::Horizontal, this, this);
	m_vruler = new QtRuleBar(Qt::Vertical, this, this);
	setCursor(Qt::PointingHandCursor);
	setRenderHint(QPainter::Antialiasing);
	//parent->setMouseTracking(true);
	m_graphicsScene = new QGraphicsScene;
	m_graphicsScene->setSceneRect(-wndSize.width() / 2.0, -wndSize.height() / 2.0, wndSize.width(), wndSize.height());
	QBrush brush(QColor(20, 20, 20));
	m_graphicsScene->setBackgroundBrush(brush);
	setScene(m_graphicsScene);

	m_pixItem = new PixItem(wndSize);
	m_graphicsScene->addItem(m_pixItem);
	m_pixItem->setPos(0, 0);
	//m_pixItem->setAcceptHoverEvents(false);
	//this->setAcceptDrops(true);

	setMouseTracking(true);

	box = new QtCornerBox(this);
	//m_pRuleItem = new QBackRuleGraphicsItem();
	//m_graphicsScene->addItem(m_pRuleItem);
	//m_pWorker->moveToThread(m_pWorkThread);
	//m_pWorker->setItem(m_pixItem);
	//m_pWorkThread->start();
	//connect(this, SIGNAL(recvData(unsigned char*)), m_pWorker, SLOT(recvData(unsigned char*)));
	//connect(m_pWorker, SIGNAL(captureAPic(QPixmap)), this, SLOT(create_Pic(QPixmap)));
}

QImageView::~QImageView()
{
	m_pWorkThread->deleteLater();
	delete m_hruler;
	delete m_vruler;
	delete box;
	delete m_graphicsScene;
}

void QImageView::updateData()
{
	//sm_pixItem->updateData();
	scene()->update();
}

void QImageView::recvGraphicData(unsigned char* data)
{
	//m_pixItem->recvGraphicData(data);
	emit recvData(data);
	scene()->update();
}

void QImageView::onSetStartBit(int nBit)
{
	m_pixItem->onSetStartBit(nBit);
}

void QImageView::setDataShowFlag(int iFlag)
{
	iDataTypeFlag = iFlag;
}

void QImageView::translate(QPointF delta)
{
	delta *= m_scale;

	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	QPoint newCenter(VIEW_WIDTH / 2 - delta.x(), VIEW_HEIGHT / 2 - delta.y());
	centerOn(mapToScene(newCenter));

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
	qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100)
		return;

	scale(scaleFactor, scaleFactor);
	m_scale *= scaleFactor;
	m_pCallout->setScale(1.0 / m_scale);
}

void QImageView::create_Pic(QPixmap temp)
{
	m_pixItem->setPixMap(temp);
	scene()->update();
}

void QImageView::rotateLeft()
{
	m_pixItem->setRotation(m_pixItem->rotation()+90);
	updateRuler();
}

void QImageView::rotateRight()
{
	m_pixItem->setRotation(m_pixItem->rotation() -90);
	updateRuler();
}

void QImageView::mouseMoveEvent(QMouseEvent *event)
{
	//QPointF point = mapToScene(event->pos());
	//int iX = int(point.x()) + wndSize.width() / 2;
	//int iY = int(point.y()) + wndSize.height() / 2;
	//if (iX < 0 || iX > wndSize.width() || iY < 0 || iY > wndSize.height())
	//{
	//	return;
	//}

	//QString strData = m_pixItem->getPointData(iX, iY, iDataTypeFlag);//???
	//QString strShow = QString("x:%1,y:%2, value:%3").arg(iX).arg(iY).arg(strData);
	//emit updataInfo(strShow);
	QPointF pt = mapToScene(event->pos());
	m_hruler->updatePosition(event->pos());
	m_vruler->updatePosition(event->pos());
	//emit positionChanged(pt.x(), pt.y());
	if (m_pCallout == nullptr)
		m_pCallout = new QImageCallout(m_pixItem);

	int iX = int(pt.x()) + wndSize.width() / 2;
	int iY = int(pt.y()) + wndSize.height() / 2;

	if (iX < 0 || iX > wndSize.width() || iY < 0 || iY > wndSize.height())
	{
		m_pCallout->hide();
	}
	else
	{
		m_pixItem->setZValue(2);
		m_pCallout->setText(QString("X:%1 \nY:%2 ").arg(iX).arg(iY));
		QPointF offset;
		offset.setX(iX < (wndSize.width() - 80) ? 10 : -50);
		offset.setY(iY < 80 ? 50 : -50);
		m_pCallout->setAnchor(pt, offset);
		m_pCallout->setZValue(0);
		m_pCallout->updateGeometry();
		m_pCallout->show();
		m_pCallout->setActive(true);
	}
	QGraphicsView::mouseMoveEvent(event);
}

void QImageView::mousePressEvent(QMouseEvent *event)
{
	//this->fitInView(m_pixItem, Qt::KeepAspectRatio);
	//m_pCallout->setScale(1);

	//scale(1, 1);
	//if (event->button() == Qt::LeftButton)
	//{
	//	QPointF point = mapToScene(event->pos());
	//	if (scene()->itemAt(point, transform()) == NULL)
	//	{
	//		m_bMouseTranslate = true;
	//		m_lastMousePos = event->pos();
	//	}
	//	else
	//	{
	//		int iX = int(point.x()) + wndSize.width() / 2;
	//		int iY = int(point.y()) + wndSize.height() / 2;
	//		if (iX < 0 || iX > wndSize.width() || iY < 0 || iY > wndSize.height())
	//		{
	//			return;
	//		}
	//		m_bToolTipMove = true;
	//	}
	//}

	QGraphicsView::mousePressEvent(event);
	updateRuler();
}

void QImageView::mouseReleaseEvent(QMouseEvent *event)
{
	//if (event->button() == Qt::LeftButton)
	//{
	//	m_bMouseTranslate = false;
	//	m_bToolTipMove = false;
	//}

	QGraphicsView::mouseReleaseEvent(event);
}

void QImageView::wheelEvent(QWheelEvent *event)
{
	// 获取当前鼠标相对于view的位置;
	QPointF cursorPoint = event->pos();
	// 获取当前鼠标相对于scene的位置;
	QPointF scenePos = this->mapToScene(QPoint(cursorPoint.x(), cursorPoint.y()));
	// 获取view的宽高;
	qreal viewWidth = this->viewport()->width();
	qreal viewHeight = this->viewport()->height();

	// 获取当前鼠标位置相当于view大小的横纵比例;
	qreal hScale = cursorPoint.x() / viewWidth;
	qreal vScale = cursorPoint.y() / viewHeight;

	QPoint scrollAmount = event->angleDelta();
	scrollAmount.y() > 0 ? zoomIn() : zoomOut();
	updateRuler();

	// 将scene坐标转换为放大缩小后的坐标;
	QPointF viewPoint = this->matrix().map(scenePos);
	// 通过滚动条控制view放大缩小后的展示scene的位置;
	horizontalScrollBar()->setValue(int(viewPoint.x() - viewWidth * hScale));
	verticalScrollBar()->setValue(int(viewPoint.y() - viewHeight * vScale));
}

void QImageView::resizeEvent(QResizeEvent *event)
{
	QGraphicsView::resizeEvent(event);

	this->setViewportMargins(RULER_SIZE - 1, RULER_SIZE - 1, 0, 0);
	m_hruler->resize(this->size().width() - RULER_SIZE - 1, RULER_SIZE);
	m_hruler->move(RULER_SIZE, 0);
	m_vruler->resize(RULER_SIZE, this->size().height() - RULER_SIZE - 1);
	m_vruler->move(0, RULER_SIZE);

	box->resize(RULER_SIZE, RULER_SIZE);
	box->move(0, 0);
	updateRuler();
}

void QImageView::scrollContentsBy(int dx, int dy)
{
	QGraphicsView::scrollContentsBy(dx, dy);
	updateRuler();
}

void QImageView::updateRuler()
{
	if (scene() == 0) return;
	QRectF viewbox = this->rect();
	qDebug() << viewbox;
	qDebug() << "scene()->sceneRect: "<<scene()->sceneRect();

	QPointF offset = mapFromScene(scene()->sceneRect().topLeft());
	qDebug() << "offset: "<<offset;

	double factor = 1. / transform().m11();
	double lower_x = factor * (viewbox.left() - offset.x());
	double upper_x = factor * (viewbox.right() - RULER_SIZE - offset.x());
	m_hruler->setRange(lower_x, upper_x, upper_x - lower_x);
	m_hruler->update();

	double lower_y = factor * (viewbox.top() - offset.y()) * -1;
	double upper_y = factor * (viewbox.bottom() - RULER_SIZE - offset.y()) * -1;

	m_vruler->setRange(lower_y, upper_y, upper_y - lower_y);
	m_vruler->update();
}
