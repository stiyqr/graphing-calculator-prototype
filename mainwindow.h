#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void addWidget();
    void removeWidget();

    void setGraphWindow();

private slots:
    void on_btn_addFunc_clicked();

    void addNumberLabelX();
    void addNumberLabelY();

private:
    Ui::MainWindow *ui;

    QHash<QToolButton*, QHBoxLayout*> buttonToLayoutMap;
};
#endif // MAINWINDOW_H
