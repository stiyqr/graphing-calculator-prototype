#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setGraphWindow();

    //QObject::connect(ui->btn_addFunc, &QPushButton::clicked, this, &MainWindow::addWidget);



    /////////////////////////////////////////////// try area ///////////////////////////////////////////////
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addWidget() {
    /*QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui->vLayout);

    QHBoxLayout* subLayout = new QHBoxLayout();

    QString buttonText = "Button";
    QPushButton* button = new QPushButton(buttonText, ui->plot);
    subLayout->addWidget(button);

    QCheckBox* checkBox = new QCheckBox("Check me!");
    subLayout->addWidget(checkBox);

    layout->insertLayout(0, subLayout);

    buttonToLayoutMap.insert(button, subLayout);

    QObject::connect(button, &QPushButton::clicked, this, &MainWindow::removeWidget);*/

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui->vLayout);
    QHBoxLayout* subLayout = new QHBoxLayout();

    QString lineStr = "y=";
    QLineEdit* txt_inputBar = new QLineEdit(lineStr);
    subLayout->addWidget(txt_inputBar);

    QToolButton* buttonRm = new QToolButton();
    buttonRm->setStyleSheet("QToolButton {background-color: red}");
    subLayout->addWidget(buttonRm);

    QToolButton* buttonHd = new QToolButton();
    buttonHd->setStyleSheet("QToolButton {background-color: gray}");
    subLayout->addWidget(buttonHd);

    layout->insertLayout(0, subLayout);

    buttonToLayoutMap.insert(buttonRm, subLayout);

    QObject::connect(buttonRm, &QToolButton::clicked, this, &MainWindow::removeWidget);
}

void MainWindow::removeWidget() {
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    QHBoxLayout* layout = buttonToLayoutMap.take(button);

    while (layout->count() != 0) {
        QLayoutItem* item = layout->takeAt(0);
        delete item->widget();
        delete item;
    }
    delete layout;
}

void MainWindow::setGraphWindow() {
    // make XY axis
    ui->plot->xAxis->grid()->setPen(QPen(QColor(220, 220, 220)));
    ui->plot->yAxis->grid()->setPen(QPen(QColor(220, 220, 220)));
    ui->plot->xAxis->grid()->setZeroLinePen(QPen(Qt::black, 2));
    ui->plot->yAxis->grid()->setZeroLinePen(QPen(Qt::black, 2));

    // interactions for zoom and drag
    ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->plot->addGraph();
    connect(ui->plot, SIGNAL(mouseDoubleClick(QMouseEvent*)), SLOT(clickedGraph(QMouseEvent*)));


}

void MainWindow::on_btn_addFunc_clicked()
{
    addWidget();
}
