//
// Created by marcin on 16.12.18.
//

#include "Widget.h"

Widget::Widget(QWidget *parent) :
        QWidget(parent),
        m_chart(new QChart()),
        m_series(new QLineSeries())
{
    QChartView *chartView = new QChartView(m_chart);
    chartView->setMinimumSize(800, 600);
    m_chart->addSeries(m_series);
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 1000);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Iters");
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(-1, 1);
    axisY->setTitleText("Mean square error");
    m_chart->setAxisX(axisX, m_series);
    m_chart->setAxisY(axisY, m_series);
    m_chart->legend()->hide();
    m_chart->setTitle("Genetic strategy error in each iter:");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(chartView);
    m_chart->update();

}

Widget::~Widget()
{

}