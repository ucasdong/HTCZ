#include "StaticCheckWidget.h"
#include "ui_StaticCheckWidget.h"

StaticCheckWidget::StaticCheckWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StaticCheckWidget)
{
    ui->setupUi(this);
}

StaticCheckWidget::~StaticCheckWidget()
{
    delete ui;
}

StaticChecktDialog::StaticChecktDialog(QWidget *parent):
    Dialog(parent),
    m_pMainWidget(new StaticCheckWidget)
{
    setMainWidget(m_pMainWidget);
    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowStaysOnTopHint);
    Dialog::setTitleStr(QStringLiteral("生成报告中..."));
    Dialog::setHideButtonVisible(false);
    Dialog::setCloseButtonVisible(false);
    Dialog::resize(800, 80);
}

StaticChecktDialog::~StaticChecktDialog()
{

}

int StaticChecktDialog::exec()
{
    return 0;
}
