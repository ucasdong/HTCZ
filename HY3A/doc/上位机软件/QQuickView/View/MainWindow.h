#pragma once
#include <QMainWindow>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QJsonObject>
#include "QDisplayDockWidget.h"
#include <QSettings>
#include "ykyc/CDlgComm.h"
#include "QSaveDataConfigDialog.h"
#include "assistdialog.h"
#include "ReplayListFrame.h"
#include "BottomWidget.h"
#include "HealthyForm.h"
namespace Ui {
	class MainWindow;
}

class MainWindow :
	public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

signals :
	void updateTitle(QString sTitle);

public slots:
	void logSlot(const QString &message, int level);
	void upDateViewVisible(QAction* action);
	void onOpenPrjtFile(QString sFileName);
	void onClosePrjt();

	void slot_showListWidget();
	void slot_playNext();
	void slot_playPre();
	void slot_playPause();
	void slot_playModeChange();
	void slot_setPosition(int position);
	void slot_openRawFile();


public :
	QJsonObject configJson();

private slots:
	void btn_capture_clicked(bool checked);
	void btn_stop_clicked(bool checked);
	void btn_ykyc_clicked();

private:
	void loadViews(QJsonObject objConfig);
	void addToolWidget();
	void initAction(QToolButton * pButton, QString sIconText, QString iconPath);
	void initLogger();
	void initConnections();
	Ui::MainWindow *ui;
	QWidget *toolWidget;
	QVBoxLayout *verticalLayout;
	QHBoxLayout *horizontalLayout_2;
	QToolButton *btn_capture;
	QToolButton *btn_stop;
	QToolButton *btn_replay;
	QToolButton *btn_pic;
	QToolButton *btn_table;
	QToolButton *btn_line;
	QToolButton* btn_ykyc;
	QToolButton* btn_auxData;
	QToolButton* btn_report;

	assistDialog* m_pAssistDialog;
	HealthyForm * m_pHealthyDialog;
	QDockWidget *m_pLogDock;
	QTextEdit *m_pLogTextEdit;
	QJsonObject configRoot;
	QList<QDisplayDockWidget *> m_pDisplayWidgetList;
	QSettings * m_pPlatFormIniConfig;
	QString m_sLastPrjtFileName;
	CDlgComm* ykyc_window = nullptr;
	QSaveDataConfigDialog *m_pSaveRawConfigWidget;
	ReplayListFrame * m_ReplayListFrame;
	int m_ReplayListFrameX, m_ReplayListFrameY;
	BottomWidget *m_BottomWidget;

};

