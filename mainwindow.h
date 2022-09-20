#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


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

    void on_ApplyButton_clicked();

    void on_filterSize_valueChanged(int value);

    void on_useResult_clicked();

    void on_openButton_clicked();

    void on_BinarizationMethod_currentIndexChanged(int index);


    void on_morphSize_valueChanged(int value);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
