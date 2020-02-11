#include "QQuickView.h"
#include <QtWidgets/QApplication>
#include <QSplashScreen>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
//#include<vld.h>£¬
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QPixmap pixmap(":/image/View/image/splash.png");
	QSplashScreen splash(pixmap);
	splash.show();
	splash.showMessage("Loading...   ", Qt::AlignBottom | Qt::AlignHCenter, Qt::white);

	a.processEvents();
	QQuickView w;

	QPropertyAnimation *animation = new QPropertyAnimation(&w, "windowOpacity");
	animation->setDuration(200);
	animation->setStartValue(0);
	animation->setEndValue(1);
	//animation->start();

	QPropertyAnimation *animation2 = new QPropertyAnimation(&splash, "windowOpacity");
	animation2->setDuration(200);
	animation2->setStartValue(1);
	animation2->setEndValue(0);
	//animation2->start();
	QParallelAnimationGroup *group = new QParallelAnimationGroup;
	group->addAnimation(animation);
	group->addAnimation(animation2);
	group->start();
	w.show();
	splash.finish(&w);
	return a.exec();
}
