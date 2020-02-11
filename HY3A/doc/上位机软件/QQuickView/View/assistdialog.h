#ifndef ASSISTDIALOG_H
#define ASSISTDIALOG_H

#include <QDialog>
#include "CommonHeader.h"
namespace Ui {
class assistDialog;
}

class assistDialog : public QDialog
{
    Q_OBJECT

public:
    explicit assistDialog(QWidget *parent = nullptr);
    ~assistDialog();

public:
    void getData(int id, BYTE* data, int length);

public:
    Ui::assistDialog *ui;
};

#endif // ASSISTDIALOG_H
