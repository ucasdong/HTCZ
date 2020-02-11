#include "themewidget.h"
#include "ui_themewidget.h"

#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QLegend>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtCore/QRandomGenerator>
#include <QtCharts/QBarCategoryAxis>
#include <QtWidgets/QApplication>
#include <QtCharts/QValueAxis>
#include <QLegendMarker>
#include <QDebug>
ThemeWidget::ThemeWidget(QWidget *parent) :
    QWidget(parent),
    m_listCount(3),
    m_valueMax(10),
    m_valueCount(7),
    m_tooltip(nullptr),
    m_maxX(0),
    m_minX(0),
    m_maxY(0),
    m_minY(0),
    moveX(0),
    moveY(0),
    m_ui(new Ui_ThemeWidgetForm)
{
    m_ui->setupUi(this);
    populateThemeBox();
    populateAnimationBox();
    populateLegendBox();

    //create charts

    //QChartView *chartView;

    //    chartView = new QChartView(createAreaChart());
    //    m_ui->gridLayout->addWidget(chartView, 1, 0);
    //    m_charts << chartView;

    //    chartView = new QChartView(createPieChart());
    //    // Funny things happen if the pie slice labels do not fit the screen, so we ignore size policy
    //    chartView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //    m_ui->gridLayout->addWidget(chartView, 1, 1);
    //    m_charts << chartView;

    //![5]
    m_pChartView = new QChartView(createLineChart());
    m_ui->gridLayout->addWidget(m_pChartView, 1, 0);
	m_pChartView->setContentsMargins(0, 0, 0, 0);
	m_pChartView->chart()->layout()->setContentsMargins(0, 0, 0, 0);
	m_pChartView->chart()->setBackgroundRoundness(0);

    //![5]
    //!
    m_pChartView->installEventFilter(this);
    m_pChartView->chart()->installEventFilter(this);

    m_pChartView->setRubberBand(QChartView::RectangleRubberBand);
    m_charts << m_pChartView;


    //    chartView = new QChartView(createBarChart(m_valueCount));
    //    m_ui->gridLayout->addWidget(chartView, 2, 0);
    //    m_charts << chartView;

    //    chartView = new QChartView(createSplineChart());
    //    m_ui->gridLayout->addWidget(chartView, 2, 1);
    //    m_charts << chartView;

    //    chartView = new QChartView(createScatterChart());
    //    m_ui->gridLayout->addWidget(chartView, 2, 2);
    //    m_charts << chartView;

    // Set defaults
    m_ui->antialiasCheckBox->setChecked(true);

    // Set the colors from the light theme as default ones
    //QPalette pal = qApp->palette();
    //pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
    //pal.setColor(QPalette::WindowText, QRgb(0x404044));
    //qApp->setPalette(pal);

    m_ui->animatedLabel->setVisible(false);
    m_ui->animatedComboBox->setVisible(false);
    m_ui->themeComboBox->setCurrentIndex(1);
    m_ui->legendComboBox->setCurrentIndex(4);
    updateUI();
}

ThemeWidget::~ThemeWidget()
{
    delete m_ui;
}

void ThemeWidget::setTitle(QString title)
{
    m_pChartView->chart()->setTitle(title);
}

void ThemeWidget::removeAllLines()
{
    //    foreach(QLineSeries * pLine,  m_pChartView->chart()->series())
    //    {
    m_pChartView->chart()->removeAllSeries();
    m_maxX =0;
    m_minX =0;
    m_maxY =0;
    m_minY =0;
    //    }

}

void ThemeWidget::savePic(QString path)
{
    QScreen * screen = QGuiApplication::primaryScreen();
    QPixmap p = screen->grabWindow(m_pChartView->winId());
    QImage image = p.toImage();
    image.save(path+"chart.png");
}

void ThemeWidget::setTopToolsVisible(bool bVisible)
{
 //   m_ui->animatedLabel->setVisible(bVisible);
 //   m_ui->animatedComboBox->setVisible(bVisible);
 //   m_ui->themeComboBox->setVisible(bVisible);
 //   m_ui->legendComboBox->setVisible(bVisible);
 //   m_ui->animatedLabel->setVisible(bVisible);
 //   m_ui->legendLabel->setVisible(bVisible);
 //   m_ui->themeLabel->setVisible(bVisible);
 //   m_ui->antialiasCheckBox->setVisible(bVisible);
 //   m_ui->btn_selectAll->setVisible(bVisible);
 //   m_ui->btn_unSelectAll->setVisible(bVisible);
	//m_ui->btn_save->setVisible(bVisible);
	m_ui->widget->setVisible(bVisible);
}

void ThemeWidget::setYRange(int min, int max)
{
    m_pChartView->chart()->axisY()->setRange(min, max);
}

QChartView *ThemeWidget::chart()
{
    return m_pChartView;
}

void ThemeWidget::drawALine(QString name, vector<float> params)
{
    if(m_maxX < params.size())
        m_maxX = params.size();
    for(int i = 0; i < params.size(); i++)
    {
        if(m_maxY < params.at(i))
            m_maxY =  params.at(i);
    }


    DataList aLineData(name, params);
    m_dataTable.append(aLineData);
    QChartView *chart = m_pChartView;

    QLineSeries *series = new QLineSeries(chart);
    series->setUseOpenGL(true);
    series->setPointLabelsFormat("(@xPoint,@yPoint)");
    series->setPointsVisible(true);
    series->setPointLabelsVisible(true);
    QScatterSeries *series1 = new QScatterSeries();
    //series1->setMarkerShape(QScatterSeries::MarkerShapeCircle);//圆形的点
    series1->setBorderColor(series->color()); //离散点边框颜色
    series1->setMarkerSize(series->pen().width()); //离散点大小
    series1->setPointLabelsVisible(false);

    for (int i = 0; i < params.size(); i++)
    {
        series->append(i, params.at(i));
        series1->append(i, params.at(i));
    }
    series->setName(name);
    chart->chart()->addSeries(series);
    chart->chart()->addSeries(series1);
    series1->setColor(series->color());

    foreach (QLegendMarker* poLegendMarker, chart->chart()->legend()->markers(series1))
    {
        poLegendMarker->setVisible(false);
        poLegendMarker->setProperty("type", 1);//散点图
    }
    connect(series1, SIGNAL(hovered(QPointF,bool)), this, SLOT(toolTip(QPointF,bool)));

    chart->chart()->createDefaultAxes();
    chart->chart()->axisX()->setRange(m_minX, m_maxX);
    chart->chart()->axisY()->setRange(m_minY, m_maxY);
    static_cast<QValueAxis *>(chart->chart()->axisX())->setLabelFormat("%i");

    foreach (QLegendMarker* poLegendMarker, m_charts[0]->chart()->legend()->markers(series))
    {
        QVariant v = QVariant::fromValue((void *) series1);
        poLegendMarker->setProperty("PointLine", v);

        disconnect(poLegendMarker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
        connect(poLegendMarker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
}

void ThemeWidget::drawALine(QString name, vector<double> params)
{
    if(m_maxX < params.size())
        m_maxX = params.size();
    for(int i = 0; i < params.size(); i++)
    {
        if(m_maxY < params.at(i))
            m_maxY =  params.at(i);
    }


    //    DataList aLineData(name, params);
    //    m_dataTable.append(aLineData);
    QChartView *chart = m_pChartView;

    QLineSeries *series = new QLineSeries(chart);
    series->setUseOpenGL(true);
    series->setPointLabelsFormat("(@xPoint,@yPoint)");
    series->setPointsVisible(true);
    series->setPointLabelsVisible(true);
    QScatterSeries *series1 = new QScatterSeries();
    //series1->setMarkerShape(QScatterSeries::MarkerShapeCircle);//圆形的点
    series1->setBorderColor(series->color()); //离散点边框颜色
    series1->setMarkerSize(series->pen().width()); //离散点大小
    series1->setPointLabelsVisible(false);

    for (int i = 0; i < params.size(); i++)
    {
        series->append(i, params.at(i));
        series1->append(i, params.at(i));
    }
    series->setName(name);
    chart->chart()->addSeries(series);
    chart->chart()->addSeries(series1);
    series1->setColor(series->color());

    foreach (QLegendMarker* poLegendMarker, chart->chart()->legend()->markers(series1))
    {
        poLegendMarker->setVisible(false);
        poLegendMarker->setProperty("type", 1);//散点图
    }
    connect(series1, SIGNAL(hovered(QPointF,bool)), this, SLOT(toolTip(QPointF,bool)));

    chart->chart()->createDefaultAxes();
    chart->chart()->axisX()->setRange(m_minX, m_maxX);
    chart->chart()->axisY()->setRange(m_minY, m_maxY);
    static_cast<QValueAxis *>(chart->chart()->axisX())->setLabelFormat("%i");

    foreach (QLegendMarker* poLegendMarker, m_charts[0]->chart()->legend()->markers(series))
    {
        QVariant v = QVariant::fromValue((void *) series1);
        poLegendMarker->setProperty("PointLine", v);

        disconnect(poLegendMarker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
        connect(poLegendMarker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }

}

void ThemeWidget::drawALine(QString name, QList<QPointF> params)
{
	m_maxX = params.size();
	m_minX = 0;
	for (int i = 0; i < params.size(); i++)
	{
		if (m_maxY < params.at(i).y())
			m_maxY = params.at(i).y();
	}
	m_maxY = m_maxY * 1.1;
	QChartView *chart = m_pChartView;

	m_series = new QLineSeries(chart);
	m_series->setUseOpenGL(true);
	m_series->setPointLabelsFormat("(@xPoint,@yPoint)");
	m_series->setPointsVisible(true);
	m_series->setPointLabelsVisible(true);
	QScatterSeries *series1 = new QScatterSeries();
	series1->setMarkerShape(QScatterSeries::MarkerShapeCircle);//圆形的点
	series1->setBorderColor(m_series->color()); //离散点边框颜色
	series1->setMarkerSize(m_series->pen().width()); //离散点大小
	series1->setPointLabelsVisible(false);

	m_series->append(params);
	series1->append(params);

	m_series->setName(name);
	chart->chart()->addSeries(m_series);
	chart->chart()->addSeries(series1);
	series1->setColor(m_series->color());

	foreach(QLegendMarker* poLegendMarker, chart->chart()->legend()->markers(series1))
	{
		poLegendMarker->setVisible(false);
		poLegendMarker->setProperty("type", 1);//散点图
	}
	connect(series1, SIGNAL(hovered(QPointF, bool)), this, SLOT(toolTip(QPointF, bool)));

	chart->chart()->createDefaultAxes();
	chart->chart()->axisX()->setRange(m_minX, m_maxX);
	chart->chart()->axisY()->setRange(m_minY, m_maxY);
	static_cast<QValueAxis *>(chart->chart()->axisX())->setLabelFormat("%i");

	foreach(QLegendMarker* poLegendMarker, m_charts[0]->chart()->legend()->markers(m_series))
	{
		QVariant v = QVariant::fromValue((void *)series1);
		poLegendMarker->setProperty("PointLine", v);

		disconnect(poLegendMarker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
		connect(poLegendMarker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
	}
}

void ThemeWidget::updateLine(QList<QPointF> params)
{
	for (int i = 0; i < params.size(); i++)
	{
		if (m_maxY < params.at(i).y())
			m_maxY = params.at(i).y();
	}
	m_series->replace(params);
}

void ThemeWidget::handleMarkerClicked()
{
    QLegendMarker* poLegendMarker = qobject_cast<QLegendMarker*> (sender());
    Q_ASSERT(poLegendMarker);

    switch (poLegendMarker->type())
    {
    case QLegendMarker::LegendMarkerTypeXY:
    {
        /* Toggle visibility of Series */
        poLegendMarker->series()->setVisible(!poLegendMarker->series()->isVisible());

        /* Turn legend marker back to visible,
             * since hiding poSeries also hides the marker
             * and we don't want it to happen now. */
        poLegendMarker->setVisible(true);
        QVariant v = poLegendMarker->property("PointLine");
        QScatterSeries* line = (QScatterSeries*)v.value<void *>();
        line->setVisible(poLegendMarker->series()->isVisible());
        foreach (QLegendMarker* poLegendMarker, m_pChartView->chart()->legend()->markers(line))
        {
            poLegendMarker->setVisible(false);
            //poLegendMarker->setProperty("type", 1);//散点图
        }


        /* Dim the marker, if series is not visible */
        qreal alpha = 1.0;

        if (!poLegendMarker->series()->isVisible())
        {
            alpha = 0.5;
        }

        QColor oColor;
        QBrush oBrush = poLegendMarker->labelBrush();
        oColor = oBrush.color();
        oColor.setAlphaF(alpha);
        oBrush.setColor(oColor);
        poLegendMarker->setLabelBrush(oBrush);

        oBrush = poLegendMarker->brush();
        oColor = oBrush.color();
        oColor.setAlphaF(alpha);
        oBrush.setColor(oColor);
        poLegendMarker->setBrush(oBrush);

        QPen oPen = poLegendMarker->pen();
        oColor = oPen.color();
        oColor.setAlphaF(alpha);
        oPen.setColor(oColor);
        poLegendMarker->setPen(oPen);

        break;
    }
    default:
    {
        qDebug() << "Unknown poLegendMarker type";
        break;
    }
    }
}

void ThemeWidget::toolTip(QPointF point, bool state)
{
    if (m_tooltip == nullptr)
        m_tooltip = new Callout(m_pChartView->chart());
    if (state)
    {
        m_pChartView->chart()->setZValue(2);
        m_tooltip->setText(QString("X:%1 \nY:%2 ").arg(point.x()).arg(point.y()));
        m_tooltip->setAnchor(point);
        m_tooltip->setZValue(0);
        m_tooltip->updateGeometry();
        m_tooltip->show();
        m_tooltip->setActive(true);
    }
    else
    {
        m_tooltip->hide();
    }
}

void ThemeWidget::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0) {
        //m_pChartView->chart()->zoom(1.1);
    } else {
        //m_pChartView->chart()->zoom(10.0/11);
    }

    QWidget::wheelEvent(event);
}

void ThemeWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        m_pChartView->releaseMouse();
    }
    if((event->button() == Qt::LeftButton))                         //点击左边鼠标
    {
        m_pChartView->setRubberBand(QChartView::RectangleRubberBand);
    }
    QWidget::mousePressEvent(event);
}

void ThemeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        //m_pChartView->setRubberBand(QChartView::RectangleRubberBand);
        //m_pChartView->chart()->zoomReset();
    }
    QWidget::mouseReleaseEvent(event);
}

//void ThemeWidget::mouseMoveEvent(QMouseEvent *event)
//{
//    if ((event->buttons() == Qt::LeftButton)) //当满足鼠标左键被点击时。
//    {
////        this->move(pointPox);
//    }
//    QWidget::mouseMoveEvent(event);

//}

bool ThemeWidget::eventFilter(QObject *object, QEvent *event)
{
    if ((object == m_pChartView || object == m_pChartView->chart()))
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent * et = (QMouseEvent *)event;
            if(et->button() == Qt::RightButton && m_pChartView->chart() != nullptr
				&& m_pChartView->chart()->axisX() != nullptr && m_pChartView->chart()->axisY() != nullptr)
            {
                m_pChartView->chart()->zoomReset();
                m_pChartView->chart()->axisX()->setRange(m_minX, m_maxX);
                m_pChartView->chart()->axisY()->setRange(m_minY, m_maxY);
                m_pChartView->setRubberBand(QChartView::NoRubberBand);

                //return true;
            }
        }

        if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent * et = (QMouseEvent *)event;
            if(et->button() == Qt::RightButton)
            {
                return true;
            }
        }

        if(event->type() == QEvent::Wheel)
        {

        }
    }
    return QWidget::eventFilter(object, event);
}

void ThemeWidget::populateThemeBox()
{
    // add items to theme combobox
    m_ui->themeComboBox->addItem("Light", QChart::ChartThemeLight);
    m_ui->themeComboBox->addItem("Blue Cerulean", QChart::ChartThemeBlueCerulean);
    m_ui->themeComboBox->addItem("Dark", QChart::ChartThemeDark);
    m_ui->themeComboBox->addItem("Brown Sand", QChart::ChartThemeBrownSand);
    m_ui->themeComboBox->addItem("Blue NCS", QChart::ChartThemeBlueNcs);
    m_ui->themeComboBox->addItem("High Contrast", QChart::ChartThemeHighContrast);
    m_ui->themeComboBox->addItem("Blue Icy", QChart::ChartThemeBlueIcy);
    m_ui->themeComboBox->addItem("Qt", QChart::ChartThemeQt);
}

void ThemeWidget::populateAnimationBox()
{
    // add items to animation combobox
    m_ui->animatedComboBox->addItem("No Animations", QChart::NoAnimation);
    m_ui->animatedComboBox->addItem("GridAxis Animations", QChart::GridAxisAnimations);
    m_ui->animatedComboBox->addItem("Series Animations", QChart::SeriesAnimations);
    m_ui->animatedComboBox->addItem("All Animations", QChart::AllAnimations);
}

void ThemeWidget::populateLegendBox()
{
    // add items to legend combobox
    m_ui->legendComboBox->addItem("No Legend ", 0);
    m_ui->legendComboBox->addItem("Legend Top", Qt::AlignTop);
    m_ui->legendComboBox->addItem("Legend Bottom", Qt::AlignBottom);
    m_ui->legendComboBox->addItem("Legend Left", Qt::AlignLeft);
    m_ui->legendComboBox->addItem("Legend Right", Qt::AlignRight);
}

QChart *ThemeWidget::createLineChart() const
{
    //![1]
    QChart *chart = new QChart();
    //![1]

    //![2]
    //    QString name("Series ");
    //    int nameIndex = 0;
    //    for (const DataList &list : m_dataTable) {
    //        QLineSeries *series = new QLineSeries(chart);
    //        for (const Data &data : list)
    //            series->append(data.first);
    //        series->setName(name + QString::number(nameIndex));
    //        nameIndex++;
    //        chart->addSeries(series);
    //    }
    //![2]

    //![3]
    //chart->axisX()->setRange(0, m_valueMax);
    //chart->axisY()->setRange(0, m_valueCount);
    //![3]
    //![4]
    // Add space to label to add space between labels and axis
    //static_cast<QValueAxis *>(chart->axisY())->setLabelFormat("%.1f  ");
    //![4]

    return chart;
}

void ThemeWidget::updateUI()
{
    //![6]
    QChart::ChartTheme theme = static_cast<QChart::ChartTheme>(
                m_ui->themeComboBox->itemData(m_ui->themeComboBox->currentIndex()).toInt());
    //![6]
    const auto charts = m_charts;
    if (!m_charts.isEmpty() && m_pChartView->chart()->theme() != theme) {
        for (QChartView *chartView : charts) {
            //![7]
            //chartView->chart()->zoom(1.1);
            //chartView->chart()->zoom(10.0/11);
            chartView->chart()->setTheme(theme);
            //chartView->chart()->legend()->markers().at(0)->set
            //![7]
        }

        // Set palette colors based on selected theme
        //![8]
        QPalette pal = window()->palette();
        if (theme == QChart::ChartThemeLight) {
            pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
            //![8]
        } else if (theme == QChart::ChartThemeDark) {
            pal.setColor(QPalette::Window, QRgb(0x121218));
            pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        } else if (theme == QChart::ChartThemeBlueCerulean) {
            pal.setColor(QPalette::Window, QRgb(0x40434a));
            pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        } else if (theme == QChart::ChartThemeBrownSand) {
            pal.setColor(QPalette::Window, QRgb(0x9e8965));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeBlueNcs) {
            pal.setColor(QPalette::Window, QRgb(0x018bba));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeHighContrast) {
            pal.setColor(QPalette::Window, QRgb(0xffab03));
            pal.setColor(QPalette::WindowText, QRgb(0x181818));
        } else if (theme == QChart::ChartThemeBlueIcy) {
            pal.setColor(QPalette::Window, QRgb(0xcee7f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else {
            pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        }
        window()->setPalette(pal);
    }

    // Update antialiasing
    //![11]
    bool checked = m_ui->antialiasCheckBox->isChecked();
    for (QChartView *chart : charts)
        chart->setRenderHint(QPainter::Antialiasing, checked);
    //![11]

    // Update animation options
    //![9]
    QChart::AnimationOptions options(
                m_ui->animatedComboBox->itemData(m_ui->animatedComboBox->currentIndex()).toInt());
    if (!m_charts.isEmpty() && m_pChartView->chart()->animationOptions() != options) {
        for (QChartView *chartView : charts)
            chartView->chart()->setAnimationOptions(options);
    }
    //![9]

    // Update legend alignment
    //![10]
    Qt::Alignment alignment(
                m_ui->legendComboBox->itemData(m_ui->legendComboBox->currentIndex()).toInt());

    if (!alignment) {
        for (QChartView *chartView : charts)
            chartView->chart()->legend()->hide();
    } else {
        for (QChartView *chartView : charts) {
            chartView->chart()->legend()->setAlignment(alignment);
            chartView->chart()->legend()->update();
            chartView->chart()->legend()->show();
        }
    }
    //![10]
}

void ThemeWidget::selectAll()
{
    foreach (QLegendMarker* poLegendMarker, m_charts[0]->chart()->legend()->markers())
    {
        if(!poLegendMarker->series()->isVisible() && poLegendMarker->property("type").toInt() != 1)
            emit poLegendMarker->clicked();
    }
}

void ThemeWidget::unselectAll()
{
    foreach (QLegendMarker* poLegendMarker, m_charts[0]->chart()->legend()->markers())
    {
        if(poLegendMarker->series()->isVisible() && poLegendMarker->property("type").toInt() != 1)
            emit poLegendMarker->clicked();
    }
}

void ThemeWidget::savePic()
{
    QPixmap pixmap = m_pChartView->grab();
    QImage image2 = pixmap.toImage();

    QString fileName = QFileDialog::getSaveFileName(
                this,
                "Save",
                ".",
                "Images (*.png *.xpm *.jpg)");
    image2.save(fileName);
}

