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
	void recvGraphicData(unsigned char* data);                                  //���ղ���ʾͼ�����ݣ�ͨ�ã�
    void onSetStartBit(int nBit);

	void setDataShowFlag(int iFlag);                                   //������ʾ����(ʮ���ƻ���ʮ������)

signals:
    void recvData(unsigned char*);
    void updataInfo(QString info);
	void positionChanged(int x, int y);

public slots:
	void translate(QPointF delta);  // ƽ��
	void zoomIn();  // �Ŵ�
	void zoomOut();  // ��С
	void zoom(float scaleFactor); // ���� - scaleFactor�����ŵı�������
    void create_Pic(QPixmap);
	void rotateLeft();
	void rotateRight();

protected:
	// ƽ��
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	// �Ŵ�/��С
	void wheelEvent(QWheelEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	void scrollContentsBy(int dx, int dy) Q_DECL_OVERRIDE;
	void updateRuler();


private:
	bool m_bMouseTranslate;  // ��ͼƽ�Ʊ�ʶ
	bool m_bToolTipMove;     // ��ͼƽ�Ʊ�ʶ
	QPoint m_lastMousePos;   // �������µ�λ��
	qreal m_scale;           // ����ֵ
	qreal m_zoomDelta;       // ���ŵ�����
	int iDataTypeFlag;       //������ʾ����(ʮ���ƻ���ʮ������)
    bool m_bFirstResize;
	QSize wndSize;           //���ڴ�С
    QThread * m_pWorkThread;
	QGraphicsScene *m_graphicsScene;
	QtRuleBar *m_hruler;
	QtRuleBar *m_vruler;
	QtCornerBox * box;
	QImageCallout * m_pCallout;
    //QImageViewWorker * m_pWorker;
};
