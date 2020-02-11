#pragma once

#include <QDialog>
#include "ui_QSaveDataConfigDialog.h"

class QSaveDataConfigDialog : public QDialog
{
	Q_OBJECT

public:
	QSaveDataConfigDialog(QWidget *parent = Q_NULLPTR);
	~QSaveDataConfigDialog();

public slots:
	void on_pushButton_ChoosePath_clicked();

private:
	Ui::QSaveDataConfigDialog ui;
};
