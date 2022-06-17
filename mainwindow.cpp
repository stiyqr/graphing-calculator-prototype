#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Widget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setGraphWindow();
    addNumberLabelX();
    addNumberLabelY();

    ui->plot->xAxis->setTicks(false);
    ui->plot->yAxis->setTicks(false);
    ui->plot->xAxis2->setTicks(false);
    ui->plot->yAxis2->setTicks(false);
    ui->plot->xAxis2->setVisible(true);
    ui->plot->yAxis2->setVisible(true);

    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(addNumberLabelX()));
    connect(ui->plot->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(addNumberLabelY()));


    //connect(&mDataTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));
    //mDataTimer.start(40);

    /////////////////////////////////////////////// try area ///////////////////////////////////////////////

    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(tryGraph()));
    /*QCPGraph* graph = ui->plot->addGraph();

    for (int i = 0; i < 999; i++) {
        int x = i / 50;
        int y = qSin(graph->dataCount()/50.0);

        //graph->addData(x, y);
        //ui->plot->replot();
    }

    QVector<double> x(251), y0(251), y1(251);
    for (int i=0; i<251; ++i)
    {
      x[i] = i;
      y0[i] = qExp(-i/150.0)*qCos(i/10.0); // exponentially decaying cosine
    }
    graph->setData(x,y0);
    graph->rescaleAxes();*/

    //graph1 = ui->plot->addGraph(ui->plot->xAxis, ui->plot->axisRect()->axis(QCPAxis::atRight, 0));
    //graph1->setPen(QPen(QColor(250, 120, 0)));

    /*QCPItemRect* r =  new QCPItemRect(ui->plot);
    r->topLeft->setPixelPosition(ui->plot->xAxis->range().lower);
    r->topLeft->setCoords(ui->plot->yAxis->range().upper);
    r->bottomRight->setCoords(ui->plot->yAxis->range().lower);*/

    //connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), line, SLOT(changeLineLen(line)));

}

MainWindow::~MainWindow() {
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

    Widget* widget = new Widget{subLayout, ui->plot};

    layout->insertLayout(0, subLayout);

    buttonToLayoutMap.insert(widget->buttonRm, subLayout);
    hiddenList.insert(widget->buttonHd, false);

    QObject::connect(widget->buttonRm, &QToolButton::clicked, this, &MainWindow::removeWidget);
    QObject::connect(widget->buttonHd, &QToolButton::clicked, this, &MainWindow::hideWidget);
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

void MainWindow::hideWidget() {
    QToolButton* button = qobject_cast<QToolButton*>(sender());

    auto&& hidden = hiddenList[button];
    hidden = !hidden;

    if (hidden) {
        button->setIcon(QIcon("../assets/icon-hidden.png"));
    }
    else {
        button->setIcon(QIcon("../assets/icon-hide.png"));
    }
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

void MainWindow::tryGraph() {
    //double interval = ui->plot->xAxis->range().upper - ui->plot->xAxis->range().lower;
    QCPGraph* graph = ui->plot->addGraph();

    for (double i = 0; i < ui->plot->xAxis->range().upper * 1000; i++) {
        double x = i/1000;
        double y = x;

        graph->addData(x, y);
    }

    for (double i = 0; -i > ui->plot->xAxis->range().lower * 1000; i++) {
        double x = -i/1000;
        double y = x;

        graph->addData(x, y);
    }
}

void MainWindow::addNumberLabelX() {
    static QVector<QCPItemText*> numLabelx;

    while (!numLabelx.empty()) {
        delete numLabelx[numLabelx.size()-1];
        numLabelx.pop_back();
    }

    auto&& tickVector =ui->plot->xAxis->tickVector();
    auto&& tickVectorLabels = ui->plot->xAxis->tickVectorLabels();

    auto&& tickVectorIterator = tickVector.begin();
    auto&& tickVectorLabelsIterator = tickVectorLabels.begin();

    for ( ; tickVectorIterator != tickVector.end();
              ++tickVectorIterator,
              ++tickVectorLabelsIterator ) {
        numLabelx.emplaceBack(new QCPItemText(ui->plot));
        numLabelx.back()->setPositionAlignment(Qt::AlignBottom|Qt::AlignRight);
        numLabelx.back()->position->setType(QCPItemPosition::ptPlotCoords);

        numLabelx.back()->position->setCoords(*tickVectorIterator, 0);
        numLabelx.back()->setText(*tickVectorLabelsIterator);
    }
}

void MainWindow::addNumberLabelY() {
    //ui->plot->yAxis->setScaleRatio(ui->plot->xAxis, 1.0);

    static QVector<QCPItemText*> numLabely;

    while (!numLabely.empty()) {
        delete numLabely[numLabely.size()-1];
        numLabely.pop_back();
    }

    /*for (int i = 0; i < ui->plot->yAxis->range().upper; i++) {
        numLabely.emplaceBack(new QCPItemText(ui->plot));
        numLabely[i]->setPositionAlignment(Qt::AlignBottom|Qt::AlignRight);
        numLabely[i]->position->setType(QCPItemPosition::ptPlotCoords);

        numLabely[i]->position->setCoords(0, i);
        numLabely[i]->setText(QString::fromStdString(std::to_string(i)));
    }

    for (int i = 0; -i > ui->plot->yAxis->range().lower; i++) {
        numLabely2.emplaceBack(new QCPItemText(ui->plot));
        numLabely2[i]->setPositionAlignment(Qt::AlignBottom|Qt::AlignRight);
        numLabely2[i]->position->setType(QCPItemPosition::ptPlotCoords);

        numLabely2[i]->position->setCoords(0, -i);
        numLabely2[i]->setText(QString::fromStdString(std::to_string(-i)));
    }*/

    auto&& tickVector =ui->plot->yAxis->tickVector();
    auto&& tickVectorLabels = ui->plot->yAxis->tickVectorLabels();

    auto&& tickVectorIterator = tickVector.begin();
    auto&& tickVectorLabelsIterator = tickVectorLabels.begin();

    for ( ; tickVectorIterator != tickVector.end();
              ++tickVectorIterator,
              ++tickVectorLabelsIterator ) {
        numLabely.emplaceBack(new QCPItemText(ui->plot));
        numLabely.back()->setPositionAlignment(Qt::AlignBottom|Qt::AlignRight);
        numLabely.back()->position->setType(QCPItemPosition::ptPlotCoords);

        numLabely.back()->position->setCoords(0, *tickVectorIterator);
        numLabely.back()->setText(*tickVectorLabelsIterator);
    }
}

void MainWindow::on_btn_addFunc_clicked() {
    addWidget();
}


void MainWindow::timerSlot()
{
  // calculate and add a new data point to each graph:
  graph1->addData(graph1->dataCount(), qSin(graph1->dataCount()/50.0)+qSin(graph1->dataCount()/50.0/0.3843)*0.25);


  // make key axis range scroll with the data:
  graph1->rescaleValueAxis(false, true);

  // update the vertical axis tag positions and texts to match the rightmost data point of the graphs:
  double graph1Value = graph1->dataMainValue(graph1->dataCount()-1);

  ui->plot->replot();
}

