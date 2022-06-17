#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include "ui_mainwindow.h"
#include "qcustomplot.h"

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
    QString randomColor();

    void setGraphWindow();



private slots:
    void on_btn_addFunc_clicked();

    void addNumberLabelX();
    void addNumberLabelY();

    void timerSlot();
    void tryGraph();

private:
    Ui::MainWindow *ui;

    QHash<QToolButton*, QHBoxLayout*> buttonToLayoutMap;

    QPointer<QCPGraph> graph1;
    QTimer mDataTimer;
};
#endif // MAINWINDOW_H
