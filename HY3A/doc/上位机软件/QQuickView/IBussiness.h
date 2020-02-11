#pragma once
#include <QObject>
#include <QJsonObject>
#include <qhash.h>
#include "QDisplayDockWidget.h"
#include "Model/DataAnalysisModel.h"
#include "QBaseDispalyStrategy.h"
#include "MTF.h"
#include <QTcpSocket>
#include <QMap>
#include "PayloadParser.h"

class IBussiness : public QObject
{
	Q_OBJECT
private:
	static IBussiness* m_pInstance;
private:
	IBussiness()
	{
		m_pDataAnalysisModel = nullptr;
	}
	~IBussiness();

	IBussiness& operator = (const IBussiness& t) = delete;
	IBussiness(const IBussiness &) = delete;

public slots:
	void startCapture();
	void stopCapture();
	void replayFile(QString fileName);
	void stopReplay(int iSaveType);
	void saveFile(QString fileName);
	void calMTF();
	void timerUpDateToDiJian();
	void readSocket();

public:
	static IBussiness& getInstance()
	{
		static IBussiness instance;
		return instance;
	}
	void configModelDriver(QJsonArray cameraConfig, std::string strDriverConfig);
	void registDisplayView(QDisplayDockWidget * pWidget, QJsonObject configObj);
	void unRegistDisplayView();
	void saveDataAfterCmd(QString savePath, QString cmdName, int nLoopIndex);
	void initConnectionToDijian();

	DataAnalysisModel* get_pDataAnalysisModel() {
		return m_pDataAnalysisModel;
	}

private:
	QList<QBaseDispalyStrategy *> m_pStrategyList;
	QList<QThread *> m_pThreadList;

	DataAnalysisModel* m_pDataAnalysisModel;

	QMap<QString, QMap<QString, quint64>> m_assistData;
	QMap<QString, PayloadParser*> m_pParsers;

	QHash<QString, MTF*> ID_MTF;
	QTimer *m_pTimerToDiJian;
	QTcpSocket *m_pSocketToDiJian;

};


