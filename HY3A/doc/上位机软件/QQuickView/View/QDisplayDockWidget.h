#pragma once

#include "ui_QDisplayDockWidget.h"
#include "QtDockTitleBar.h"
#include <QDockWidget>
#include <QCloseEvent>
#include <QJsonObject>
#include <QMenu>
#include "QImageView.h"
#include "QDisplayDataModel.h"

typedef unsigned char BYTE;
enum DisplayMode
{
	Image = 0,
	Table = 1,
	Line = 2,
};
class QDisplayDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	QDisplayDockWidget(QJsonObject obj, QWidget *parent = Q_NULLPTR);
	~QDisplayDockWidget();

signals:
	void saveFileTrigger(QString sFileName);
	void cal_MTF_Trigger();

public slots:
	void recvGraphicData(BYTE* data);
	void recvPic(QPixmap pic);
	void updateTable(unsigned char *pData);
	void updateLine(QList<QPointF>);

public:
	void setTableModel(QDisplayDataModel * pModel);
	void setDisplayModel(DisplayMode mode);
	QJsonObject getConfigure();
	void setConfigure(QJsonObject obj);
	DisplayMode getCurrentDisplayMode();

protected:
	void closeEvent(QCloseEvent *event) override;
	void mousePressEvent(QMouseEvent *event);

private:
	void createMenu();              //创建菜单

private:
	Ui::QDisplayDockWidget ui;
	QImageView * m_pImageView;
	QDisplayDataModel *m_pTableModel;
	QMenu* m_pPop_menu;                //右键菜单
	QJsonObject m_configureObj;
	bool m_bFirstDrawLine = true;

	QAction* m_pAction_graphic;
	QAction* m_pAction_table;
	QAction* m_pAction_line;
	QAction* m_pAction_saveData; 
	QAction* m_pAction_rotateLeft;
	QAction* m_pAction_rotateRight;

	QAction* m_pAction_MTF;
};
