#include "QSaveDataConfigDialog.h"
#include <QMessageBox>
#include <QFileDialog>

QSaveDataConfigDialog::QSaveDataConfigDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);


	connect(ui.comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
		ui.stackedWidget->setCurrentIndex(index);
	});
}

QSaveDataConfigDialog::~QSaveDataConfigDialog()
{
}

void QSaveDataConfigDialog::on_pushButton_ChoosePath_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QApplication::applicationDirPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir.isEmpty())
	{
		QMessageBox::warning(NULL, QStringLiteral("Warning"), QStringLiteral(""), QMessageBox::Ok);
		return;
	}
	ui.lineEdit_savePath->setText(dir);
}
