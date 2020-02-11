#include "assistdialog.h"
#include "ui_assistdialog.h"

assistDialog::assistDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::assistDialog)
{
    ui->setupUi(this);
	setWindowFlags(windowFlags());
	//this->setModal(true);
	ui->widget->setName(QStringLiteral("全色通道"));
	ui->widget_2->setName(QStringLiteral("多光谱通道"));

}

assistDialog::~assistDialog()
{
    delete ui;
}

void assistDialog::getData(int id, BYTE *data, int length)
{
    if((id & 0xF0) == 0xA0)
    {
        ui->widget->setData(data,length);
    }
    else if((id & 0xF0) == 0xC0)
    {
        ui->widget_2->setData(data,length);
    }
}
