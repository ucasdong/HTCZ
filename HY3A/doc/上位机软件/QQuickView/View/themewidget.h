#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>
#include "Callout.h"
#include <vector>
using namespace std;

QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
class Ui_ThemeWidgetForm;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

typedef QPair<QString, vector<float>>DataList;
typedef QList<DataList> DataTable;

QT_CHARTS_USE_NAMESPACE

class ThemeWidget: public QWidget
{
    Q_OBJECT
public:
    explicit ThemeWidget(QWidget *parent = 0);
    ~ThemeWidget();

    void setTitle(QString title);
    void removeAllLines();
    void savePic(QString path);
    void setTopToolsVisible(bool bVisible);
    void setYRange(int min, int max);
    QChartView * chart();

public slots:
    void drawALine(QString name, vector<float> params);
    void drawALine(QString name, vector<double> params);
	void drawALine(QString name, QList<QPointF> params);
	void updateLine(QList<QPointF> params);

    void handleMarkerClicked();
    void toolTip(QPointF point, bool state);
    void updateUI();


protected:
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    //virtual void mouseMoveEvent(QMouseEvent * event);
    bool eventFilter(QObject *object, QEvent *event);

private Q_SLOTS:
    void selectAll();
    void unselectAll();
    void savePic();

private:
    void populateThemeBox();
    void populateAnimationBox();
    void populateLegendBox();
    void connectSignals();
    QChart *createLineChart() const;

private:
    int m_listCount;
    int m_valueMax;
    int m_valueCount;
    int moveX,moveY;
    QChartView * m_pChartView;
    float m_maxX;
    float m_maxY;
    float m_minX;
    float m_minY;
    QList<QChartView *> m_charts;
    DataTable m_dataTable;
    QPoint dragPosition;
    Callout *m_tooltip;
    QList<Callout*> m_callouts;
    QList<QScatterSeries*> m_pointLines;
	QLineSeries *m_series;
    Ui_ThemeWidgetForm *m_ui;
};

#endif /* THEMEWIDGET_H */
