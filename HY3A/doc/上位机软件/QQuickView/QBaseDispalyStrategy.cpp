#include "QBaseDispalyStrategy.h"
#include <QDebug>

QBaseDispalyStrategy::QBaseDispalyStrategy(QDisplayDockWidget * pWidget)
	:m_pWidget(pWidget)
{
	m_pUpdataTimer = new QTimer(this);
}

QBaseDispalyStrategy::~QBaseDispalyStrategy()
{
}

void QBaseDispalyStrategy::setModelData(DataAnalysisModel* pModel)
{
	m_pDataAnalysisModel = pModel;
}

void QBaseDispalyStrategy::setDisplayWidget(QDisplayDockWidget * pWidget)
{
	m_pWidget = pWidget;
}

QCommonDispalyStrategy::QCommonDispalyStrategy(QDisplayDockWidget * pWidget)
	:QBaseDispalyStrategy(pWidget)
{
	m_pImageWorker = new QImageViewWorker(m_pWidget->getConfigure()["Height"].toInt() * m_pWidget->getConfigure()["Width"].toInt());
	connect(m_pUpdataTimer, SIGNAL(timeout()), this, SLOT(timeupSlot()));
	m_pUpdataTimer->start(20);
	m_pImageWorkThread = new QThread;
	m_pImageWorker->moveToThread(m_pImageWorkThread);
	connect(m_pImageWorker, SIGNAL(captureAPic(QPixmap)), this, SIGNAL(updatePic(QPixmap)));
	connect(this, SIGNAL(recvData(unsigned char*, int, int, int)), m_pImageWorker, SLOT(recvData(unsigned char*, int, int, int)));
	m_PointList.clear();
	m_sDataSourceType = m_pWidget->getConfigure()["DataSourceType"].toString();
	m_sDataSourceID = m_pWidget->getConfigure()["DataSourceID"].toString();
}

QPixmap QCommonDispalyStrategy::displayPic()
{
	return QPixmap();
}

void QCommonDispalyStrategy::start()
{
	//m_pUpdataTimer->start(20);
	m_pImageWorkThread->start();
}

void QCommonDispalyStrategy::stop()
{
	m_pUpdataTimer->stop();
	while (!m_pImageWorkThread->isFinished())
	{
	}
	m_pImageWorkThread->exit();
}

QCommonDispalyStrategy::~QCommonDispalyStrategy()
{
	stop();
	m_pImageWorker->deleteLater();
	m_pImageWorkThread->deleteLater();
}

void QCommonDispalyStrategy::timeupSlot()
{
	BYTE* pByteData;

	if (m_sDataSourceType == "Camera")
	{
		CameraParams* pData = m_pDataAnalysisModel->GetDataByCameraID(m_sDataSourceID);

		pByteData = pData->CurCameraData;
	}
	else if (m_sDataSourceType == "Sensor")
	{
		SensorParams* pCCD = m_pDataAnalysisModel->GetDataBySensor_ID(m_sDataSourceID);
		pByteData = pCCD->CurOriginalSensorData;

	}
	else if (m_sDataSourceType == "Spectral")
	{
		SpectralParams* pSpectral = m_pDataAnalysisModel->GetDataBySpectralID(m_sDataSourceID);
		pByteData = pSpectral->CurOriginalSpectralData;
	}

	switch (m_pWidget->getCurrentDisplayMode())
	{
	case DisplayMode::Image:
	{
		m_pUpdataTimer->start(20);
		emit recvData(pByteData, m_pWidget->getConfigure()["Width"].toInt(),
			m_pWidget->getConfigure()["Height"].toInt(), 8);
		break;
	}
	case DisplayMode::Table:
	{
		m_pUpdataTimer->start(100);
		emit updateTable(pByteData);
		break;
	}
	case DisplayMode::Line:
	{
		m_PointList.clear();
		m_pUpdataTimer->start(1000);
		for (int i = 0; i < m_pWidget->getConfigure()["Width"].toInt(); i++)
		{
			QPointF point(i, (int)(pByteData[i*2]) * 256 + (int)(pByteData[2*i + 1]));
			m_PointList.append(point);
		}
		emit updateLine(m_PointList);
		break;
	}
	default:
		break;
	}
	//m_pByteData = pByteData;
}
