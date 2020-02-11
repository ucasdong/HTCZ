#ifndef STATICCHECKWIDGET_H
#define STATICCHECKWIDGET_H

#include <QWidget>
#include "Dialog.h"

namespace Ui {
class StaticCheckWidget;
}

class StaticCheckWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StaticCheckWidget(QWidget *parent = nullptr);
    ~StaticCheckWidget();

public:
    Ui::StaticCheckWidget *ui;
};
class StaticChecktDialog : public Dialog
{
    Q_OBJECT
public:
    StaticChecktDialog(QWidget *parent = nullptr);
    ~StaticChecktDialog();

    virtual int exec();
    StaticCheckWidget * m_pMainWidget;

};
#endif // STATICCHECKWIDGET_H
