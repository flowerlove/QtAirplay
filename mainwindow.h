#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "airserver.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void StartAirplay();
    void StopAirplay();

private:
    Ui::MainWindow *ui;
    AirServer* air_server_ = Q_NULLPTR;
};
#endif // MAINWINDOW_H
