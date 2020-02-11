#include "QQuickView.h"

QQuickView::QQuickView(QWidget *parent)
	: QMainWindow(parent)
{
	setDockNestingEnabled(true);
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_ShowModal);//±³¾°Í¸Ã÷
	ui.titleLabel->setText(ui.widget->configJson()["View"].toObject().value("AppTitle").toString());

	initConnetion();
	initWidgetProp();
	showMaximized();

	QFile qssFile(":/QQuickView/View/qss/default.qss");
	qssFile.open(QIODevice::ReadOnly);
	qApp->setStyleSheet(qssFile.readAll());
	qssFile.close();
}

void QQuickView::mousePressEvent(QMouseEvent *event)
{
	moveY = event->pos().y();
	int yTemp = 60;
	//gs_Resolution->SizeToProgram(xTemp,yTemp);
	if ((event->button() == Qt::LeftButton) && (moveY < yTemp))                         //µã»÷×ó±ßÊó±ê
	{
		dragPosition = event->globalPos() - frameGeometry().topLeft();
		this->setCursor(Qt::SizeAllCursor);
	}
	else
	{
		moveY = 0;
		return;
	}
}

void QQuickView::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event);

	moveY = 0;                          //·ÀÖ¹½çÃæÂÒÒÆ¶¯

	this->setCursor(Qt::ArrowCursor);
}

void QQuickView::mouseMoveEvent(QMouseEvent *event)
{
	if ((event->buttons() == Qt::LeftButton) && (moveY != 0)) //µ±Âú×ãÊó±ê×ó¼ü±»µã»÷Ê±¡£
	{
		QPoint pointPox = event->globalPos() - dragPosition;
		this->move(pointPox);
	}
}

void QQuickView::initWidgetProp()
{
	ui.topWidget->setProperty("nav", "top");
}

void QQuickView::initConnetion()
{

	connect(ui.closeButton, &QPushButton::clicked, [=]() {
		qApp->quit();
	});
	connect(ui.minButton, &QPushButton::clicked, [=]() {
		showMinimized();
	});

	connect(ui.widget, &MainWindow::updateTitle, [=](QString sTitle) {
		ui.titleLabel->setText(sTitle);
	});

	//        connect(ui->hideButton,&QPushButton::clicked, [=](){
	//            ui->toolWidget->setHidden(!ui->toolWidget->isHidden());
	//        });

			//connect(ui->DB_Tab, SIGNAL(dialogShow()), this, SLOT(onDialogShow()));
			//connect(ui->DB_Tab, SIGNAL(dialogHide()), this, SLOT(onDialogHide()));

	connect(ui.maxButton, &QPushButton::clicked, [=]() {
		if (window()->isMaximized())
		{
			showNormal();
			ui.maxButton->setStyleSheet("QPushButton{border-image: url(:/image/View/image/max.png);}"
				"QPushButton:hover{border-image: url(:/image/View/image/max_hover.png);}");
		}
		else
		{
			showMaximized();
			ui.maxButton->setStyleSheet("QPushButton{border-image: url(:/image/View/image/restore.png);}"
				"QPushButton:hover{border-image: url(:/image/View/image/restore_hover.png);}");
		}
	});

}
