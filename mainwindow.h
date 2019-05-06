#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTime>
#include "wavClass.h"
#include "splinewaveletanalysis.h"
#include "adaptiveClass.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void openFileButtonClick();

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_b_Decomp_clicked();

    void on_b_Reconst_clicked();

    void on_b_SaveWav_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_b_SaveFileTxt_clicked();

private:
    Ui::MainWindow *ui;

    void setDataAccordingToWav();
    void tempFuncToDebug();
    void fillComboBox();
    void fillTableHeader();
    void addRowToTable(int mode, QString time1, QString time2);
    void addItemToTab(int i, int j, QString txt);

    const char* curFilePath;
    wavFileParse wav;
    SplineWaveletAnalysis *analyse;

    short* reconstrRes;
};

#endif // MAINWINDOW_H
