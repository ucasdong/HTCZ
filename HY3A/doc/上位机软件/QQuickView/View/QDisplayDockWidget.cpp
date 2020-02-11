#include "QDisplayDockWidget.h"
#include <QDebug>

QDisplayDockWidget::QDisplayDockWidget(QJsonObject obj, QWidget *parent)
	: QDockWidget(obj.value("Title").toString(), parent)
{
	QtDockTitleBar * pTitleBar = new QtDockTitleBar(this);
	setTitleBarWidget(pTitleBar);
	ui.setupUi(this);

	QHBoxLayout *horizontalLayout = new QHBoxLayout(ui.page);
	qDebug() << obj.value("Width").toInt() << obj.value("Height").toInt();
	m_pImageView = new QImageView(ui.page, QSize(obj.value("Width").toInt(), obj.value("Height").toInt()));
	m_pTableModel = new QDisplayDataModel(obj.value("Width").toInt(), obj.value("Height").toInt());
	ui.tableView->setModel(m_pTableModel);
	horizontalLayout->addWidget(m_pImageView);
	horizontalLayout->setMargin(0);
	ui.page->setLayout(horizontalLayout);
	setWidget(ui.stackedWidget);
	ui.tableView->horizontalHeader()->setDefaultSectionSize(60);
	ui.stackedWidget->setCurrentIndex(0);
	createMenu();
	ui.page_3->setTopToolsVisible(false);
	ui.tableView->setContextMenuPolicy(Qt::DefaultContextMenu);
	//ui.tableView->installEventFilter(this);
}

QDisplayDockWidget::~QDisplayDockWidget()
{
	delete m_pPop_menu;
	delete m_pAction_graphic;
	delete m_pAction_table;
	delete m_pAction_line;
}

void QDisplayDockWidget::recvGraphicData(BYTE* data)
{

}

void QDisplayDockWidget::recvPic(QPixmap pic)
{
	m_pImageView->create_Pic(pic);
}

void QDisplayDockWidget::updateTable(unsigned char *pData)
{
	m_pTableModel->setRawData(pData);
	ui.tableView->update();
}

void QDisplayDockWidget::updateLine(QList<QPointF> points)
{
	if (m_bFirstDrawLine)
	{
		ui.page_3->drawALine(QStringLiteral("DN值"), points);
		m_bFirstDrawLine = false;
	} 
	else
	{
		ui.page_3->updateLine(points);
	}
}

void QDisplayDockWidget::setTableModel(QDisplayDataModel * pModel)
{
	m_pTableModel = pModel;
	ui.tableView->setModel(pModel);
}

void QDisplayDockWidget::setDisplayModel(DisplayMode mode)
{
	ui.stackedWidget->setCurrentIndex(mode);
}

QJsonObject QDisplayDockWidget::getConfigure()
{
	return m_configureObj;
}

void QDisplayDockWidget::setConfigure(QJsonObject obj)
{
	m_configureObj = obj;
	m_pTableModel->setColorConfig(obj["TableColorDefine"].toArray());
}

DisplayMode QDisplayDockWidget::getCurrentDisplayMode()
{
	return DisplayMode(ui.stackedWidget->currentIndex());
}

void QDisplayDockWidget::closeEvent(QCloseEvent * event)
{
	this->hide();
	event->ignore();
}

void QDisplayDockWidget::mousePressEvent(QMouseEvent *event)
{
	if ((event->button() == Qt::RightButton))                         
	{
		if (m_pPop_menu)
		{
			m_pPop_menu->clear();
			m_pPop_menu->addAction(m_pAction_graphic);
			m_pPop_menu->addAction(m_pAction_table);
			m_pPop_menu->addAction(m_pAction_line);
			m_pPop_menu->addSeparator();
			m_pPop_menu->addAction(m_pAction_saveData);
			m_pPop_menu->addAction(m_pAction_MTF);

			switch (ui.stackedWidget->currentIndex())
			{
			case Image:
			{
				m_pPop_menu->addSeparator();
				m_pPop_menu->addAction(m_pAction_rotateLeft);
				m_pPop_menu->addAction(m_pAction_rotateRight);
				break;
			}
			case Table:
			{

				break;
			}
			case Line:
			{

				break;
			}

			default:
				break;
			}

			m_pPop_menu->exec(QCursor::pos());
		}
	}
	QDockWidget::mousePressEvent(event);
}

void QDisplayDockWidget::createMenu()
{
	m_pPop_menu = new QMenu();
	m_pAction_graphic  = new QAction(QIcon(":/image/View/image/file-pic.png"), QStringLiteral("图像显示"));
	m_pAction_table    = new QAction(QIcon(":/image/View/image/table.png"), QStringLiteral("表格显示"));
	m_pAction_line     = new QAction(QIcon(":/image/View/image/line1.png"), QStringLiteral("曲线显示"));

	m_pAction_saveData = new QAction(QIcon(":/image/View/image/save.png"), QStringLiteral("保存图像"));
	m_pAction_MTF = new QAction(QIcon(":/image/View/image/save.png"), QStringLiteral("MTF计算"));

	m_pAction_rotateLeft = new QAction(QIcon(":/image/View/image/rotate-left.png"), QStringLiteral("向左旋转"));
	m_pAction_rotateRight = new QAction(QIcon(":/image/View/image/rotate-right.png"), QStringLiteral("向右旋转"));

	connect(m_pAction_graphic, &QAction::triggered, [=]() {
		ui.stackedWidget->setCurrentIndex(Image);
	});

	connect(m_pAction_table, &QAction::triggered, [=]() {
		ui.stackedWidget->setCurrentIndex(Table);
	});

	connect(m_pAction_line, &QAction::triggered, [=]() {
		ui.stackedWidget->setCurrentIndex(Line);
	});

	connect(m_pAction_saveData, &QAction::triggered, [=]() {
		QString filepath = QFileDialog::getSaveFileName(this, QStringLiteral("选择保存文件"),
			"", tr("raw file (*.raw);;"));
		if (filepath.isEmpty())
		{
			return;
		}
		emit saveFileTrigger(filepath);
	});

	connect(m_pAction_MTF, &QAction::triggered, [=]() {
		emit cal_MTF_Trigger();
	});

	connect(m_pAction_rotateLeft, &QAction::triggered, m_pImageView, &QImageView::rotateLeft);
	connect(m_pAction_rotateRight, &QAction::triggered, m_pImageView, &QImageView::rotateRight);

}
