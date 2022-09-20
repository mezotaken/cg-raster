#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "imgproc.h"

static imgProc process;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ApplyButton_clicked()
{
    ui->processing->setDisabled(true);
    QApplication::processEvents();
    QAbstractButton* act = ui->editAct->checkedButton();
    QString actname = act->objectName();
    if(actname == "grayScale"){
        process.toGray(ui->grayScaleMethod->currentIndex());
    }
    if(actname == "Binarization"){
        switch(ui->BinarizationMethod->currentIndex()) {
        case 0:
            process.ThBinarize(ui->threshold->text().toInt(), ui->isAllChannels->isChecked());
            break;
        case 1:
            process.OtsuBinarize(ui->isAllChannels->isChecked());
            break;
        }
    }
    if(actname == "noiseFilter"){
        switch(ui->noiseFilterMethod->currentIndex()) {
        case 0:
            process.MedianBlur(ui->filterSize->value()*2+1);
            break;
        case 1:
            process.GaussianBlur(ui->filterSize->value()*2+1);
            break;
        case 2:
            process.BoxBlur(ui->filterSize->value()*2+1);
            break;
        }
    }
    if(actname == "findEdges"){
        switch(ui->findEdgesMethod->currentIndex()) {
        case 0:
            process.RobertsEdges();
            break;
        case 1:
            process.PrewittEdges();
            break;
        case 2:
            process.SobelEdges();
            break;
        }

    }
    if(actname == "morphFilter")
    {
        switch(ui->morphFilterMethod->currentIndex()) {
        case 0:
            process.Dilate(process.source ,ui->morphSize->value());
            break;
        case 1:
            process.Erode(process.source ,ui->morphSize->value());
            break;
        case 2:
            process.Opening(process.source ,ui->morphSize->value());
            break;
        case 3:
            process.Closing(process.source ,ui->morphSize->value());
            break;
        case 4:
            process.TopHat(process.source ,ui->morphSize->value());
            break;
        case 5:
            process.BlackHat(process.source ,ui->morphSize->value());
            break;
        case 6:
            process.MorphEdges(process.source ,ui->morphSize->value());
            break;
        }
    }
    if(actname == "filter"){
        switch(ui->filterMethod->currentIndex()) {
        case 0:
            process.SharpenFilter();
            break;
        case 1:
            process.EmbossFilter();
            break;
        case 2:
            process.Invert();
            break;
        }
    }
    ui->res->setPixmap(QPixmap::fromImage(process.result));
    ui->processing->setDisabled(false);
}


void MainWindow::on_filterSize_valueChanged(int value)
{
     ui->filterSizeInfo->setText(QString::number(value*2+1));
}

void MainWindow::on_useResult_clicked()
{
    process.setSource(process.result);
    ui->src->setPixmap(QPixmap::fromImage(process.source));
}

void MainWindow::on_openButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"));
    QImage loaded(fileName);
    QImage img = loaded.scaled(ui->src->width(),ui->src->height(),Qt::KeepAspectRatio);
    process.setSource(img);
    ui->src->setPixmap(QPixmap::fromImage(process.source));
    ui->res->setPixmap(QPixmap::fromImage(process.result));
}

void MainWindow::on_BinarizationMethod_currentIndexChanged(int index)
{
    if(index == 1)
        ui->threshold->setDisabled(true);
    else
        ui->threshold->setDisabled(false);
}

void MainWindow::on_morphSize_valueChanged(int value)
{
    ui->morphSizeInfo->setText(QString::number(value*2+1));
}
