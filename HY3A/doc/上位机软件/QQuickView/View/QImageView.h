#pragma once

#include <QGraphicsView>
#include "pixitem.h"
//#include "GraphicLabel.h"
#include <QGraphicsTextItem>
#include "QBackRuleGraphicsItem.h"
#include "rulebar.h"
#include "QImageCallout.h"

class QImageView : public QGraphicsView
{
	Q_OBJECT

public:
	QImageView(QWidget *parent, QSize WndSize);
	~QImageView();

	PixItem *m_pixItem;
	QBackRuleGraphicsItem * m_pRuleItem;
	//GraphicLabel* graphicLabel;
	QGraphicsTextItem* textItem;
	void updateData();
	void recvGraphicData(unsigned char* data);                                  //接收并显示图像数据（通用）
    void onSetStartBit(int nBit);

	void setDataShowFlag(int iFlag);                                   //数据显示类型(十进制或者十六进制)

signals:
    void recvData(unsigned char*);
    void updataInfo(QString info);
	void positionChanged(int x, int y);

public slots:
	void translate(QPointF delta);  // 平移
	void zoomIn();  // 放大
	void zoomOut();  // 缩小
	void zoom(float scaleFactor); // 缩放 - scaleFactor：缩放的比例因子
    void create_Pic(QPixmap);
	void rotateLeft();
	void rotateRight();

protected:
	// 平移
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	// 放大/缩小
	void wheelEvent(QWheelEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	void scrollContentsBy(int dx, int dy) Q_DECL_OVERRIDE;
	void updateRuler();


private:
	bool m_bMouseTranslate;  // 视图平移标识
	bool m_bToolTipMove;     // 视图平移标识
	QPoint m_lastMousePos;   // 鼠标最后按下的位置
	qreal m_scale;           // 缩放值
	qreal m_zoomDelta;       // 缩放的增量
	int iDataTypeFlag;       //数据显示类型(十进制或者十六进制)
    bool m_bFirstResize;
	QSize wndSize;           //窗口大小
    QThread * m_pWorkThread;
	QGraphicsScene *m_graphicsScene;
	QtRuleBar *m_hruler;
	QtRuleBar *m_vruler;
	QtCornerBox * box;
	QImageCallout * m_pCallout;
    //QImageViewWorker * m_pWorker;
};
