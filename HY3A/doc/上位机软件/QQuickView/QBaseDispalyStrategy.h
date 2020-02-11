#pragma once
#include <QObject>
#include <QTimer>
#include <QPixmap>
#include "Model/DataAnalysisModel.h"
#include "QDisplayDockWidget.h"
#include <QImage>
#include "QImageViewWorker.h"
#include <QThread>

class QBaseDispalyStrategy : public QObject
{
	Q_OBJECT

signals:
	void updatePic(QPixmap);
	void updateTable(unsigned char *pData);
	void updateLine(QList<QPointF> points);

public:
	QBaseDispalyStrategy(QDisplayDockWidget * pWidget = nullptr);
	virtual QPixmap displayPic() = 0;
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual ~QBaseDispalyStrategy();
	void setModelData(DataAnalysisModel* pModel);
	void setDisplayWidget(QDisplayDockWidget * pWidget);
	QTimer * m_pUpdataTimer;
	DataAnalysisModel* m_pDataAnalysisModel;
	QDisplayDockWidget * m_pWidget;
};

class QCommonDispalyStrategy : public QBaseDispalyStrategy
{
	Q_OBJECT
public:
	QCommonDispalyStrategy(QDisplayDockWidget * pWidget = nullptr);
	virtual QPixmap displayPic();
	virtual void start();
	virtual void stop();
	virtual ~QCommonDispalyStrategy();

signals:
	void recvData(unsigned char*, int, int, int);
public slots:
	void timeupSlot();

private:
	QThread * m_pImageWorkThread;
	QImageViewWorker * m_pImageWorker;
	QList<QPointF> m_PointList;
	QString m_sDataSourceType;
	QString m_sDataSourceID;
	BYTE* m_pByteData;
};