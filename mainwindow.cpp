#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "castscreenmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    air_server_ = new AirServer(this);
    CastScreenManager::GetInstance()->setMainWindow(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::StartAirplay()
{
    air_server_->Start();
}

void MainWindow::StopAirplay()
{
    air_server_->Stop();
}

