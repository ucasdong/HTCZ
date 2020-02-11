#include "MainWindow.h"
#include "ui_mainwindow.h"
#include <QDockWidget>
#include <qsettings.h>
#include "QtDockTitleBar.h"
#include <qtoolbar.h>
#include "QsLog.h"
#include "QsLogDest.h"
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTabBar>
#include <QTabWidget>
#include "IBussiness.h"

using namespace QsLogging;
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	qRegisterMetaType<QList<QPointF>>("QList<QPointF>");
	ui->setupUi(this);
	m_pSaveRawConfigWidget = new QSaveDataConfigDialog(this);
	m_pSaveRawConfigWidget->hide();
	//QWidget* p = takeCentralWidget();
	//if (p)
	//	delete p;
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_ShowModal);//背景透明
	addToolWidget();
	m_pLogDock = new QDockWidget(QStringLiteral("日志"), this);
	m_pLogDock->setObjectName("LogDock");

	QtDockTitleBar * pTitleBar3 = new QtDockTitleBar(m_pLogDock);
	m_pLogDock->setTitleBarWidget(pTitleBar3);
	m_pLogDock->setFeatures(QDockWidget::AllDockWidgetFeatures);

	m_pLogTextEdit = new QTextEdit();
	m_pLogTextEdit->setReadOnly(true);
	m_pLogDock->setWidget(m_pLogTextEdit);
	addDockWidget(Qt::BottomDockWidgetArea, m_pLogDock);
	initLogger();

	QAction *action = new QAction(QStringLiteral("日志"), this);
	action->setCheckable(true);
	QVariant v = QVariant::fromValue((void *)m_pLogDock);
	action->setProperty("view", v);
	ui->viewMenu->addAction(action);

	m_pPlatFormIniConfig = new QSettings(QCoreApplication::applicationDirPath()+"\\DefaultConfig.ini", QSettings::IniFormat);
	m_pPlatFormIniConfig->setIniCodec("GB2312");
	QString sDefaultFile = m_pPlatFormIniConfig->value("Default/LastPrjtPath").toString();
	//m_pPlatFormIniConfig->deleteLater();
	onOpenPrjtFile(sDefaultFile);
	m_pAssistDialog = new assistDialog(this);
	m_pAssistDialog->setWindowTitle(QStringLiteral("辅助数据"));
	m_pHealthyDialog = new HealthyForm(this);
	m_pAssistDialog->setWindowTitle(QStringLiteral("健康检查"));

	m_ReplayListFrame = new ReplayListFrame(this);

	m_ReplayListFrame->hide();
	m_BottomWidget = new BottomWidget(this);

	QVBoxLayout *mainLayout = new QVBoxLayout(ui->widget);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	mainLayout->addWidget(m_BottomWidget);
	m_ReplayListFrameX = this->width() - m_ReplayListFrame->width();
	m_ReplayListFrameY = this->height() - m_ReplayListFrame->height() - m_BottomWidget->height();
	m_BottomWidget->setHidden(true);
	initConnections();

}


MainWindow::~MainWindow()
{
	if (!m_sLastPrjtFileName.isEmpty())
	{
		m_pPlatFormIniConfig->setValue("Default/LastPrjtPath", m_sLastPrjtFileName);
		delete m_pPlatFormIniConfig;
	}
	onClosePrjt();
}

QJsonObject MainWindow::configJson()
{
	return configRoot;
}

void MainWindow::btn_capture_clicked(bool checked)
{
	btn_capture->setIcon(QIcon(":/image/View/image/play-disabled.png"));
	btn_stop->setIcon(QIcon(":/image/View/image/stop.png"));
	btn_capture->setEnabled(false);
	btn_stop->setEnabled(true);
	QJsonDocument doc(configRoot["Driver"].toObject());
	IBussiness::getInstance().configModelDriver(configRoot["Model"].toArray(), doc.toJson().toStdString());
	foreach(QDisplayDockWidget * pWidget, m_pDisplayWidgetList)
	{
		IBussiness::getInstance().registDisplayView(pWidget, pWidget->property("config").toJsonObject());
	}


	QString sAutoTest = m_pPlatFormIniConfig->value("Default/AutoTest").toString();
	if (sAutoTest == "true")
	{

	}
	IBussiness::getInstance().startCapture();
	QLOG_INFO() << QStringLiteral("开始采集");
}

void MainWindow::btn_stop_clicked(bool checked)
{
	btn_capture->setIcon(QIcon(":/image/View/image/play.png"));
	btn_stop->setIcon(QIcon(":/image/View/image/stop-disabled.png"));
	btn_capture->setEnabled(true);
	btn_replay->setEnabled(true);
	btn_stop->setEnabled(false);
	IBussiness::getInstance().stopCapture();
	//IBussiness::getInstance().stop
	QLOG_INFO() << QStringLiteral("停止采集/回放");
}

void MainWindow::btn_ykyc_clicked()
{
	if (ykyc_window == nullptr)
	{
		QString ykyc_config_path = configRoot["YkycPath"].toString();
		map<int, oneScreen>* mapScreens = ConfigIni::getInstance(ykyc_config_path)->getAllWndParams();
		static oneScreen currentScreen = (*mapScreens)[1];
		ykyc_window = new CDlgComm(this, &currentScreen, ConfigIni::getInstance(ykyc_config_path)->getYaoKongWndLay(), ConfigIni::getInstance(ykyc_config_path)->getYaoceData(), ykyc_config_path);
	}
	ykyc_window->show();
}

void MainWindow::loadViews(QJsonObject objConfig)
{
	QString sTitle = objConfig["View"].toObject().value("AppTitle").toString();
	emit updateTitle(sTitle);
	foreach(QJsonValue jValue, objConfig["View"].toObject()["SubWindows"].toArray())
	{
		QJsonObject obj = jValue.toObject();
		QString sType = obj.value("Type").toString();
		if (sType == "DisplayData")
		{
			QDisplayDockWidget * pDisplayWidget = new QDisplayDockWidget(obj, this);
			pDisplayWidget->setFeatures(QDockWidget::AllDockWidgetFeatures);
			pDisplayWidget->setObjectName(obj.value("ID").toString());
			pDisplayWidget->setWindowTitle(obj.value("Title").toString());
			pDisplayWidget->setProperty("config", obj);
			pDisplayWidget->setConfigure(obj);
			addDockWidget(Qt::BottomDockWidgetArea, pDisplayWidget);

			QAction *action = new QAction(obj.value("Title").toString(), this);
			action->setCheckable(true);
			QVariant v = QVariant::fromValue((void *)pDisplayWidget);
			action->setProperty("view", v);
			ui->viewMenu->addAction(action);
			m_pDisplayWidgetList.append(pDisplayWidget);
		}
		else if (sType == "Log")
		{

		}
		else
		{

		}
	}

}

void MainWindow::addToolWidget()
{
	btn_capture = new QToolButton(this);
	btn_capture->setObjectName(QStringLiteral("btn_capture"));

	btn_stop = new QToolButton(this);
	btn_stop->setObjectName(QStringLiteral("btn_stop"));
	btn_stop->setEnabled(false);

	btn_replay = new QToolButton(this);
	btn_replay->setObjectName(QStringLiteral("btn_replay"));
	btn_replay->setCheckable(true);
	btn_replay->setChecked(false);

	btn_pic = new QToolButton(this);
	btn_pic->setObjectName(QStringLiteral("btn_pic"));
	btn_pic->setCheckable(true);
	btn_pic->setChecked(true);
	btn_table = new QToolButton(this);
	btn_table->setObjectName(QStringLiteral("btn_pic"));
	btn_table->setCheckable(true);

	btn_line = new QToolButton(this);
	btn_line->setObjectName(QStringLiteral("btn_pic"));
	btn_line->setCheckable(true);

	btn_ykyc = new QToolButton(this);
	btn_ykyc->setObjectName(QStringLiteral("btn_ykyc"));

	btn_auxData = new QToolButton(this);
	btn_auxData->setObjectName(QStringLiteral("btn_auxData"));

	btn_report = new QToolButton(this);
	btn_report->setObjectName(QStringLiteral("btn_report"));

	initAction(btn_capture, QStringLiteral("开始"), QStringLiteral(":/image/View/image/play.png"));
	initAction(btn_stop, QStringLiteral("停止"), QStringLiteral(":/image/View/image/stop-disabled.png"));
	initAction(btn_replay, QStringLiteral("采集/回放"), QStringLiteral(":/image/View/image/replay.png"));
	initAction(btn_pic, QStringLiteral("图像显示"), QStringLiteral(":/image/View/image/file-pic.png"));
	initAction(btn_table, QStringLiteral("表格显示"), QStringLiteral(":/image/View/image/table.png"));
	initAction(btn_line, QStringLiteral("曲线显示"), QStringLiteral(":/image/View/image/line1.png"));
	initAction(btn_ykyc, QStringLiteral("遥控遥测"), QStringLiteral(":/image/View/image/ykyc.png"));
	initAction(btn_auxData, QStringLiteral("辅助数据"), QStringLiteral(":/image/View/image/auData.png"));
	initAction(btn_report, QStringLiteral("健康检查"), QStringLiteral(":/image/View/image/health.png"));

	QToolBar*fontToolBar = new QToolBar();
	fontToolBar->addWidget(btn_capture);
	fontToolBar->addWidget(btn_stop);
	fontToolBar->addWidget(btn_replay);
	fontToolBar->addWidget(btn_pic);
	fontToolBar->addWidget(btn_table);
	fontToolBar->addWidget(btn_line);
	fontToolBar->addWidget(btn_ykyc);
	fontToolBar->addWidget(btn_auxData);
	fontToolBar->addWidget(btn_report);

	addToolBar(fontToolBar);
	
}

void MainWindow::initAction(QToolButton * pButton, QString sIconText, QString iconPath)
{
	QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(pButton->sizePolicy().hasHeightForWidth());
	pButton->setSizePolicy(sizePolicy1);
	pButton->setMinimumSize(QSize(0, 45));
	pButton->setMaximumSize(QSize(1000, 45));
	pButton->setText(sIconText);
	QIcon icon5;
	icon5.addFile(iconPath, QSize(20 ,20), QIcon::Normal, QIcon::Off);
	pButton->setIcon(icon5);
	pButton->setIconSize(QSize(20, 20));
	pButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	pButton->setAutoRaise(true);

	pButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

}

void MainWindow::initLogger()
{
	// 1. init the logging mechanism
	Logger& logger = Logger::instance();
	logger.setLoggingLevel(QsLogging::TraceLevel);
	//设置log位置
	const QString sLogPath(QDir(QCoreApplication::applicationDirPath()).filePath("log.txt"));

	// 2. add two destinations
	DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(
		sLogPath, EnableLogRotation, MaxSizeBytes(512), MaxOldLogCount(2)));
	DestinationPtr debugDestination(DestinationFactory::MakeDebugOutputDestination());
	//DestinationPtr functorDestination(DestinationFactory::MakeFunctorDestination(&logFunction));

	//这样和槽函数连接
	DestinationPtr sigsSlotDestination(DestinationFactory::MakeFunctorDestination(this, SLOT(logSlot(QString, int))));

	logger.addDestination(debugDestination);
	logger.addDestination(fileDestination);
	//logger.addDestination(functorDestination);
	logger.addDestination(sigsSlotDestination);

	// 3. start logging
	QLOG_INFO() << QStringLiteral("软件启动");

}

void MainWindow::initConnections()
{
	connect(ui->openPrjtaction, &QAction::triggered, [=]() {
		QString filepath = QFileDialog::getOpenFileName(this, QStringLiteral("选择工程文件"),
			"", tr("Project files (*.prjt);;"));
		if (filepath.isEmpty())
		{
			return;
		}
		m_sLastPrjtFileName = filepath;
		onOpenPrjtFile(filepath);
	});

	connect(ui->saveDataConfig, &QAction::triggered, [=]() {
		m_pSaveRawConfigWidget->showNormal();
	});

	connect(ui->viewMenu, &QMenu::aboutToShow, [=]() {
		foreach(QAction * action, ui->viewMenu->actions())
		{
			QVariant v = action->property("view");
			QDockWidget* view = (QDockWidget*)v.value<void *>();
			action->setChecked(view->isVisible());
		}
	});
	connect(ui->viewMenu, SIGNAL(triggered(QAction*)),
		this, SLOT(upDateViewVisible(QAction*)));    //连接menu的triggered信号到槽函数
	connect(btn_capture, SIGNAL(clicked(bool)), this, SLOT(btn_capture_clicked(bool)));
	connect(btn_stop, SIGNAL(clicked(bool)), this, SLOT(btn_stop_clicked(bool)));

	connect(btn_pic, &QToolButton::clicked, [=](bool checked) {
		btn_pic->setChecked(true);
		foreach(QDisplayDockWidget * pWidget, m_pDisplayWidgetList)
			pWidget->setDisplayModel(Image);
		if (checked)
		{
			btn_table->setChecked(false);
			btn_line->setChecked(false);
		}
	});
	connect(btn_table, &QToolButton::clicked, [=](bool checked) {
		btn_table->setChecked(true);
		foreach(QDisplayDockWidget * pWidget, m_pDisplayWidgetList)
			pWidget->setDisplayModel(Table);
		if (checked)
		{
			btn_pic->setChecked(false);
			btn_line->setChecked(false);
		}
	});
	connect(btn_line, &QToolButton::clicked, [=](bool checked) {
		btn_line->setChecked(true);
		foreach(QDisplayDockWidget * pWidget, m_pDisplayWidgetList)
			pWidget->setDisplayModel(Line);
		if (checked)
		{
			btn_table->setChecked(false);
			btn_pic->setChecked(false);
		}
	});
	connect(btn_replay, &QToolButton::clicked, [=](bool checked) {
		m_BottomWidget->setHidden(!btn_replay->isChecked());
		//btn_replay->setChecked(!btn_replay->isChecked());


		btn_capture->setEnabled(!btn_replay->isChecked());
		btn_stop->setEnabled(!btn_replay->isChecked());

		if (btn_replay->isChecked())
		{
			btn_capture->setIcon(QIcon(":/image/View/image/play-disabled.png"));
			btn_stop->setIcon(QIcon(":/image/View/image/stop-disabled.png"));
		}
		else
		{
			btn_capture->setIcon(QIcon(":/image/View/image/play.png"));
			btn_stop->setIcon(QIcon(":/image/View/image/stop-disabled.png"));
		}

		

	});

	connect(btn_auxData, &QToolButton::clicked, [=]() {
		if (!m_pAssistDialog->isVisible())
			m_pAssistDialog->show();
	});

	connect(btn_report, &QToolButton::clicked, [=]() {
		if (!m_pHealthyDialog->isVisible())
			m_pHealthyDialog->show();
		});

	connect(m_BottomWidget->listBtn, &QPushButton::clicked, this, &MainWindow::slot_showListWidget);
	connect(m_BottomWidget->playBtn, &QPushButton::clicked, this, &MainWindow::slot_playPause);
	connect(m_BottomWidget->playModeBtn, &QPushButton::clicked, this, &MainWindow::slot_playModeChange);
	connect(m_BottomWidget->playTimeSlider, &QSlider::valueChanged, this, &MainWindow::slot_setPosition);
	connect(m_BottomWidget->nextBtn, &QPushButton::clicked, this, &MainWindow::slot_playNext);
	connect(m_BottomWidget->lastBtn, &QPushButton::clicked, this, &MainWindow::slot_playPre);
	connect(m_BottomWidget->openBtn, &QPushButton::clicked, this, &MainWindow::slot_openRawFile);
	connect(btn_ykyc, &QToolButton::clicked, this, &MainWindow::btn_ykyc_clicked);
}

void MainWindow::logSlot(const QString &message, int level)
{
	switch (level)
	{
	case TraceLevel:
		m_pLogTextEdit->setTextColor(QColor("gray"));
		break;
	case DebugLevel:
		m_pLogTextEdit->setTextColor(QColor("green"));
		break;
	case InfoLevel:
		m_pLogTextEdit->setTextColor(QColor("green"));
		break;
	case WarnLevel:
		m_pLogTextEdit->setTextColor(QColor("yellow"));
		break;
	case ErrorLevel:
		m_pLogTextEdit->setTextColor(QColor("red"));
		break;
	case FatalLevel:
		m_pLogTextEdit->setTextColor(QColor("red"));
		break;
	default:
		break;
	}
	m_pLogTextEdit->append(message);
}

void MainWindow::upDateViewVisible(QAction* action)
{
	QVariant v = action->property("view");
	QDockWidget* view = (QDockWidget*)v.value<void *>();
	view->setVisible(!view->isVisible());
}

void MainWindow::onOpenPrjtFile(QString sFileName)
{
	QFile loadFile(sFileName);

	if (!loadFile.open(QIODevice::ReadOnly)) {
		qDebug() << "could't open projects json";
		return;
	}

	QByteArray allData = loadFile.readAll();
	loadFile.close();
	QString str = QString::fromLocal8Bit(allData.data());
	QJsonParseError json_error;
	QJsonDocument jsonDoc(QJsonDocument::fromJson(str.toStdString().data(), &json_error));

	if (json_error.error != QJsonParseError::NoError) {
		qDebug() << "json error!";
		return;
	}
	onClosePrjt();
	configRoot = jsonDoc.object();

	loadViews(configRoot);


	QSettings settings("QuickView", configRoot["ProjectName"].toString());
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::onClosePrjt()
{
	for each (QDisplayDockWidget* pDockWidget in m_pDisplayWidgetList)
	{
		this->removeDockWidget(pDockWidget);
		delete pDockWidget;
	}
	m_pDisplayWidgetList.clear();
	for (int i = 1; i < ui->viewMenu->actions().count(); i++)
	{
		QAction * action = ui->viewMenu->actions().at(i);
		ui->viewMenu->removeAction(action);
		delete action;
	}
	IBussiness::getInstance().unRegistDisplayView();
	QSettings settings("QuickView", configRoot["ProjectName"].toString());
	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState());
}

void MainWindow::slot_showListWidget()
{
	if (m_ReplayListFrame->isVisible())
		m_ReplayListFrame->hide();
	else
	{
		m_ReplayListFrame->setGeometry(width() - m_ReplayListFrame->width(), height() - m_ReplayListFrame->height() - m_BottomWidget->height(), 580, 470);
		m_ReplayListFrame->show();
		m_ReplayListFrame->raise();
	}
}

void MainWindow::slot_playNext()
{

}

void MainWindow::slot_playPre()
{

}

void MainWindow::slot_playPause()
{

}

void MainWindow::slot_playModeChange()
{

}

void MainWindow::slot_setPosition(int position)
{

}

void MainWindow::slot_openRawFile()
{
	QString filepath = QFileDialog::getOpenFileName(this, QStringLiteral("选择回放文件"),
		"", tr("Raw files (*.raw);;"));
	if (filepath.isEmpty())
	{
		return;
	}

	QJsonDocument doc(configRoot["Driver"].toObject());
	IBussiness::getInstance().configModelDriver(configRoot["Model"].toArray(), doc.toJson().toStdString());
	foreach(QDisplayDockWidget * pWidget, m_pDisplayWidgetList)
	{
		IBussiness::getInstance().registDisplayView(pWidget, pWidget->property("config").toJsonObject());
	}


	QString sAutoTest = m_pPlatFormIniConfig->value("Default/AutoTest").toString();
	if (sAutoTest == "true")
	{

	}

	IBussiness::getInstance().replayFile(filepath);
}
