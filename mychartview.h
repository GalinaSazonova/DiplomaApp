#ifndef MYCHARTVIEW_H
#define MYCHARTVIEW_H

#include <QMainWindow>
#include <QtCharts/QtCharts>

namespace Ui {
class MyChartView;
}

class MyChartView : public QMainWindow
{
    Q_OBJECT

public:
    explicit MyChartView(QWidget *parent = nullptr);
    ~MyChartView();

private:
    Ui::MyChartView *ui;
};

#endif // MYCHARTVIEW_H
