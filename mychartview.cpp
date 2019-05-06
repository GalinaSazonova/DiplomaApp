#include "mychartview.h"
#include "ui_mychartview.h"

using namespace QtCharts;

MyChartView::MyChartView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MyChartView)
{
    ui->setupUi(this);
}

MyChartView::~MyChartView()
{
    delete ui;
}
