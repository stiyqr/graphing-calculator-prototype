#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include "Widget.h"

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
    void hideWidget();
    void readInput();

    void setGraphWindow();



private slots:
    void on_btn_addFunc_clicked();

    void addNumberLabelX(const QCPRange&);
    void addNumberLabelY(const QCPRange&);
    void updateGraphY(const QCPRange&);
    void updateGraphX(const QCPRange&);
    void replotGraphs();

    void tryGraph(const QCPRange&);
    void displayVarList();
    void displayOutput();

private:
    Ui::MainWindow *ui;

    QVector<Widget*>             vWidget;
    QHash<QToolButton*, Widget*> buttonToWidget;
    QHash<QToolButton*, bool>    hiddenList;

    QString   inputStr, outputStr;
    QCPGraph* graph;

};
#endif // MAINWINDOW_H
