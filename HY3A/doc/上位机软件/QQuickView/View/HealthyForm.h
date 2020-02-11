#ifndef HEALTHYFORM_H
#define HEALTHYFORM_H

#include <QWidget>
#include <QToolButton>
#include <QList>
#include "StaticCheckWidget.h"
#include "WordEngine.h"
#include <QDialog>

namespace Ui {
class HealthyForm;
}
using namespace std;
class HealthyForm : public QDialog
{
    Q_OBJECT

public:
    explicit HealthyForm(QWidget *parent = nullptr);
    ~HealthyForm();

private slots:
    void onToolButtonClicked(int id);
    void on_btn_chooseDir_clicked();

    void on_btn_badPoint_clicked();

    void on_btn_jumpCal_clicked();

    void on_btn_stableCal_clicked();

    void on_btn_LightLine_clicked();

    void on_btn_erroLine_clicked();

    void on_btn_staticReport_clicked();

private:
    void initTable();
    void initControl(QString re, QStringList dataList);


private:
    Ui::HealthyForm *ui;
    QList<QToolButton*> m_tButtonList;
    int m_checkButtonId;

    QStringList m_LampList;
    QStringList m_SpectralList;
    QStringList m_CCDList;
    QStringList m_TDIList;
    QStringList m_GainList;
    QStringList m_TimeList;
    QStringList m_AlgorithmList;
    QString fileName;
    StaticChecktDialog * m_pStaticCheckDlg;
    WordEngine *m_pWordEngine;
	QStringList fileList;
	QFileInfoList fileInfoList;

};

#endif // HEALTHYFORM_H
