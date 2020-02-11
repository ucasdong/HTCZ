#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <windows.h>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

    void setMainWidget(QWidget *pWidget);
    void setTitleStr(QString sTitle);

    void setHideButtonVisible(bool visible);

    void setCloseButtonVisible(bool bisible);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    virtual void onOK();

    virtual void onCancel();

    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private slots:
    void on_OKpushButton_clicked();

    void on_CancelpushButton_clicked();

public:
    int moveX,moveY;
    QPoint dragPosition;
    Ui::Dialog *ui;

};

#endif // DIALOG_H
