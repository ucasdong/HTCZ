#include "Dialog.h"
#include "ui_Dialog.h"
#include <windowsx.h>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->closeButton, &QPushButton::clicked, [=](){
        onCancel();
        this->reject();
    });
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::setMainWidget(QWidget *pWidget)
{
    QHBoxLayout *horizontalLayout = new QHBoxLayout(ui->mainWidget);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
    horizontalLayout->setContentsMargins(0, 0, 0, 0);
    horizontalLayout->addWidget(pWidget);
}

void Dialog::setTitleStr(QString sTitle)
{
    ui->label->setText(sTitle);
}

void Dialog::setHideButtonVisible(bool visible)
{
    ui->frame->setVisible(visible);
}

void Dialog::setCloseButtonVisible(bool bisible)
{
    ui->closeButton->setVisible(bisible);
}

void Dialog::mousePressEvent(QMouseEvent *event)
{
    moveY = event->pos().y();
    int yTemp = 60;
    //gs_Resolution->SizeToProgram(xTemp,yTemp);
    if((event->button() == Qt::LeftButton)&&(moveY < yTemp))                         //点击左边鼠标
    {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        this->setCursor(Qt::SizeAllCursor);
    }
    else
    {
        moveY = 0;
        return ;
    }
}

void Dialog::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    moveY = 0;                          //防止界面乱移动

    this->setCursor(Qt::ArrowCursor);
}

void Dialog::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() == Qt::LeftButton)&&(moveY != 0)) //当满足鼠标左键被点击时。
    {
        QPoint pointPox = event->globalPos() - dragPosition;
        this->move(pointPox);
    }
}

void Dialog::onOK()
{

}

void Dialog::onCancel()
{

}

bool Dialog::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);
    MSG* msg = reinterpret_cast<MSG*>(message);

    switch(msg->message)
    {
    case WM_NCHITTEST:
        int xPos = GET_X_LPARAM(msg->lParam) - this->frameGeometry().x();
        int yPos = GET_Y_LPARAM(msg->lParam) - this->frameGeometry().y();
        if(this->childAt(xPos,yPos) == 0)
        {
            *result = HTCAPTION;
        }else{
            return false;
        }
        if(xPos > 18 && xPos < 22)
            *result = HTLEFT;
        if(xPos > (this->width() - 22) && xPos < (this->width() - 18))
            *result = HTRIGHT;
        if(yPos > 18 && yPos < 22)
            *result = HTTOP;
        if(yPos > (this->height() - 22) && yPos < (this->height() - 18))
            *result = HTBOTTOM;
        if(xPos > 18 && xPos < 22 && yPos > 18 && yPos < 22)
            *result = HTTOPLEFT;
        if(xPos > (this->width() - 22) && xPos < (this->width() - 18) && yPos > 18 && yPos < 22)
            *result = HTTOPRIGHT;
        if(xPos > 18 && xPos < 22 && yPos > (this->height() - 22) && yPos < (this->height() - 18))
            *result = HTBOTTOMLEFT;
        if(xPos > (this->width() - 22) && xPos < (this->width() - 18) && yPos > (this->height() - 22) && yPos < (this->height() - 18))
            *result = HTBOTTOMRIGHT;

        return true;
    }
    return false;
}

void Dialog::on_OKpushButton_clicked()
{
    onOK();
    this->accept();
}

void Dialog::on_CancelpushButton_clicked()
{
    onCancel();
    this->reject();
}
