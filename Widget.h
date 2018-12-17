//
// Created by marcin on 16.12.18.
//
#include <QtWidgets>
#include <QtCharts>

#ifndef PORR_WIDGET_H
#define PORR_WIDGET_H

QT_CHARTS_BEGIN_NAMESPACE
    class QLineSeries;
    class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE


class Widget : public QWidget
{
Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    QChart *m_chart;
    QLineSeries *m_series ;
};


#endif //PORR_WIDGET_H
