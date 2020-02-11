#pragma once

#include <QGraphicsView>
#include "pixitem.h"
//#include "GraphicLabel.h"
#include <QGraphicsTextItem>

class QImageView : public QGraphicsView
{
	Q_OBJECT

public:
	QImageView(QWidget *parent, QSize WndSize);
	~QImageView();

	PixItem *m_pixItem;
	//GraphicLabel* graphicLabel;
	QGraphicsTextItem* textItem;
	void recvData(BYTE* data, QPixmap* pixmap);                //���ղ���ʾ����
	void setDataShowFlag(int iFlag);                           //������ʾ����(ʮ���ƻ���ʮ������)
	void changeShowWidth(int iWidth);

public slots:
	void translate(QPointF delta);  // ƽ��
	void zoomIn();  // �Ŵ�
	void zoomOut();  // ��С
	void zoom(float scaleFactor); // ���� - scaleFactor�����ŵı�������

protected:
	// ƽ��
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	// �Ŵ�/��С
	void wheelEvent(QWheelEvent *event);

private:
	bool m_bMouseTranslate;  // ��ͼƽ�Ʊ�ʶ
	bool m_bToolTipMove;     // ��ͼƽ�Ʊ�ʶ
	QPoint m_lastMousePos;   // �������µ�λ��
	qreal m_scale;           // ����ֵ
	qreal m_zoomDelta;       // ���ŵ�����
	int iDataTypeFlag;       // ������ʾ����(ʮ���ƻ���ʮ������)
	int iShowWidth;
	QSize wndSize;           //���ڴ�С
};
