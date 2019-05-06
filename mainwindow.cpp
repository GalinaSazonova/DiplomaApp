#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QStandardItemModel"
#include "QStandardItem"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton->resize(150, 40);
    ui->fileNameLabel->resize(150, 40);
    setWindowTitle("Выберите файл для анализа");
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::openFileButtonClick);

    fillComboBox();
    fillTableHeader();
    ui->b_Decomp->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fillComboBox()
{
    QStringList list;
    list.append("Стандартная");
    list.append("Целочисленная");
    list.append("Только адапт. сетка");

    ui->comboBox->addItems(list);
}

void MainWindow::fillTableHeader()
{
    ui->tableWidget->setColumnCount(9);
    //ui->tableWidget->setRowCount(2);
    QStringList list;
    list.append("Имя файла");
    list.append("Р-р сетки");
    list.append("t сетки");
    list.append("t вейвл. потока");
    list.append("%");
    list.append("Время реконстр.");
    list.append("eps");
    list.append("Р-р исх. потока");
    list.append("Тип");
    ui->tableWidget->setHorizontalHeaderLabels(list);
}

void MainWindow::addRowToTable(int mode, QString time1, QString time2)
{
    int row = ui->tableWidget->rowCount();
    if (mode == 0)
    {
        ui->tableWidget->insertRow(row + 1);
        ui->tableWidget->setRowCount(row + 1);

        //Заполение данными
        addItemToTab(row, 0, ui->fileNameLabel->text());
        addItemToTab(row, 1, QString::number(analyse->getAdNetSize()));
        addItemToTab(row, 2, time1);
        addItemToTab(row, 3, time2);
        addItemToTab(row, 6, QString::number(analyse->getCurrentEps()));
        addItemToTab(row, 7, QString::number(analyse->getInitFlowSize()));
        addItemToTab(row, 8, ui->comboBox->currentText());
    }
    if (mode == 1)
    {
        addItemToTab(row - 1, 4, time2);
        addItemToTab(row - 1, 5, time1);
    }
}

void MainWindow::addItemToTab(int i, int j, QString txt)
{
    QTableWidgetItem *item = ui->tableWidget->item(i, j);
    if(!item) {
        item = new QTableWidgetItem();
        ui->tableWidget->setItem(i, j, item);
    }
    item->setText(txt);
}

void MainWindow::openFileButtonClick()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open file"), "",
            tr("Wav Files (*.wav)"));  //;;Text files (*.txt)"));

    if (fileName.isNull() || fileName.isEmpty())
    {
       curFilePath = nullptr;
       ui->fileNameLabel->setText("Файл не выбран");

       ui->b_Decomp->setEnabled(false);
       ui->b_Reconst->setEnabled(false);
       ui->b_SaveWav->setEnabled(false);
       //ui->b_graph->setEnabled(false);
       ui->b_SaveFileTxt->setEnabled(false);

       return;
    }

    int num = fileName.length() - fileName.lastIndexOf("/") - 1;
    QString name = fileName.right(num);

    fileName = fileName.replace("/", "\\\\");

    std::string str = fileName.toStdString();
    curFilePath = str.c_str();

    ui->fileNameLabel->setText(name);

    setDataAccordingToWav();

    ui->b_Decomp->setEnabled(true);
    ui->b_Reconst->setEnabled(false);
    ui->b_SaveWav->setEnabled(false);

    //tempFuncToDebug();
}

void MainWindow::setDataAccordingToWav()
{
    wav.readWav(curFilePath);

    QString winTitle = "Частота: ";
    winTitle.append(QString::number(wav.SamplesPerS()));
    winTitle.append("Hz. ");
    winTitle.append(wav.numOfchan() == 1 ? "Mono":"Stereo");
    setWindowTitle(winTitle);
    analyse = new SplineWaveletAnalysis(wav);
}

void MainWindow::tempFuncToDebug()
{
    //analyse = new SplineWaveletAnalysis(wav);
    analyse->buildAdaptiveNet();
    analyse->buildWaveFlow();
    reconstrRes = analyse->restoreByAdNetAndWaveFlow();

    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save file"), "",
            tr("Wav Files (*.wav)"));  //;;Text files (*.txt)"));

    if (fileName.isNull() || fileName.isEmpty())
    {
       return;
    }

    fileName = fileName.replace("/", "\\\\");

    std::string str = fileName.toStdString();
    const char* savePath = str.c_str();

    //analyse->compareTwoNets();

    analyse->saveWavFile(reconstrRes, wav.maxInS(), wav.getWavHeader(), savePath);

    double perc = analyse->compareTwoFlows(wav.wavData2(), reconstrRes, wav.maxInS(), analyse->getInitFlowSize());

    QString title = windowTitle();
    title.append("Perc: ");
    title.append(QString::number(perc));
    setWindowTitle(title);


}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{

}


void MainWindow::on_b_Decomp_clicked()
{
    int indx = ui->comboBox->currentIndex();
    QTime time;
    time.start();
    analyse->buildAdaptiveNet();
    int timeAdNet = time.elapsed();

    int timeWaveFlow = 0;

    switch (indx) {
    case 0:
        time.start();
        analyse->buildWaveFlow();
        timeWaveFlow = time.elapsed();
        break;
    case 1:
        time.start();
        analyse->buildIntWaveFlow();
        timeWaveFlow = time.elapsed();
        break;
    case 2:
        break;
    }

    ui->b_Reconst->setEnabled(true);
    ui->b_SaveFileTxt->setEnabled(true);

    QMessageBox msgBox;
    msgBox.setWindowTitle("");
    msgBox.setText("Декомпозиция выполнена.");
    msgBox.exec();

    addRowToTable(0, QString::number(timeAdNet), QString::number(timeWaveFlow));
}

void MainWindow::on_b_Reconst_clicked()
{
    int indx = ui->comboBox->currentIndex();

    QTime time;
    int rectime = 0;

    switch (indx) {
    case 0:
        time.start();
        reconstrRes = analyse->restoreByAdNetAndWaveFlow();
        rectime = time.elapsed();
        break;
    case 1:
        time.start();
        reconstrRes = analyse->restoreIntByAdNetAndWaveFlow();
        rectime = time.elapsed();
        break;
    case 2:
        time.start();
        reconstrRes = analyse->restoreByAdaptiveNet();
        rectime = time.elapsed();
        break;
    }

    double perc = analyse->compareTwoFlows(wav.wavData2(), reconstrRes, wav.maxInS(), analyse->getInitFlowSize());

   // QString title = windowTitle();
    //title.append("Perc: ");
    //title.append(QString::number(perc));
   // setWindowTitle(title);

    ui->b_SaveWav->setEnabled(true);

    QMessageBox msgBox;
    msgBox.setWindowTitle("");
    msgBox.setText("Реконструкция выполнена.");
    msgBox.exec();

    addRowToTable(1, QString::number(rectime), QString::number(perc));
}

void MainWindow::on_b_SaveWav_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save file"), "",
            tr("Wav Files (*.wav)"));  //;;Text files (*.txt)"));

    if (fileName.isNull() || fileName.isEmpty())
    {
       return;
    }

    fileName = fileName.replace("/", "\\\\");

    std::string str = fileName.toStdString();
    const char* savePath = str.c_str();

    analyse->saveWavFile(reconstrRes, wav.maxInS(), wav.getWavHeader(), savePath);
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    //cout << ui->comboBox->currentText().toStdString() << index << endl;
    ui->b_Reconst->setEnabled(false);
    ui->b_SaveWav->setEnabled(false);
    ui->b_SaveFileTxt->setEnabled(false);
}

void MainWindow::on_b_SaveFileTxt_clicked()
{
    //short *data, int *index, long int size, wav_hdr wavHeader, char *filename
    //analyse->saveTxtFile();
}
