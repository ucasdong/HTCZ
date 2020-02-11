#include "QtDockTitleBar.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
QtDockTitleBar::QtDockTitleBar(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.widget->setProperty("nav", "top");
	QDockWidget *dw = qobject_cast<QDockWidget*>(parentWidget());
	ui.titleLabel->setText(dw->windowTitle());
	initConnect();
	ui.label->setAutoFillBackground(true);
	QGraphicsOpacityEffect *pButtonOpacity = new QGraphicsOpacityEffect(this);
	pButtonOpacity->setOpacity(1);
	ui.label->setGraphicsEffect(pButtonOpacity);

	QPropertyAnimation *pOpacityAnimation1 = new QPropertyAnimation(pButtonOpacity, "opacity");
	pOpacityAnimation1->setDuration(1000);
	pOpacityAnimation1->setStartValue(1);
	pOpacityAnimation1->setEndValue(0);
	pOpacityAnimation1->setLoopCount(-1);
	pOpacityAnimation1->start(QAbstractAnimation::KeepWhenStopped);
	//m_pTitleMovie = new QMovie(":/image/View/image/Rolling-1s-64px.gif");
	//m_pTitleMovie->resized(this->size());
	//ui.label->setMovie(m_pTitleMovie);
	//ui.label->setScaledContents(true);
	//m_pTitleMovie->start();
}

QtDockTitleBar::~QtDockTitleBar()
{
}

void QtDockTitleBar::initConnect()
{
	connect(ui.closeButton, &QPushButton::clicked, [=]() {
		QDockWidget *dw = qobject_cast<QDockWidget*>(parentWidget());
		dw->close();
	});

	connect(ui.maxButton, &QPushButton::clicked, [=]() {
		if (window()->isMaximized())
		{
			ui.maxButton->setStyleSheet("QPushButton{border-image: url(:/image/View/image/max.png);}"
				"QPushButton:hover{border-image: url(:/image/View/image/max_hover.png);}");
		}
		else
		{
			ui.maxButton->setStyleSheet("QPushButton{border-image: url(:/image/View/image/restore.png);}"
				"QPushButton:hover{border-image: url(:/image/View/image/restore_hover.png);}");
		}
		QDockWidget *dw = qobject_cast<QDockWidget*>(parentWidget());
		dw->setFloating(!dw->isFloating());
	});
}
