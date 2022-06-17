#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Parser.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setGraphWindow();

    ui->plot->xAxis->setTicks(false);
    ui->plot->yAxis->setTicks(false);
    ui->plot->xAxis2->setTicks(false);
    ui->plot->yAxis2->setTicks(false);
    ui->plot->xAxis2->setVisible(true);
    ui->plot->yAxis2->setVisible(true);

    connect(ui->plot->xAxis, SIGNAL(rangeChanged(const QCPRange&)), this, SLOT(addNumberLabelX()));
    connect(ui->plot->yAxis, SIGNAL(rangeChanged(const QCPRange&)), this, SLOT(addNumberLabelY()));


    /////////////////////////////////////////////// try area ///////////////////////////////////////////////

    graph = ui->plot->addGraph();
    //connect(ui->plot->xAxis, SIGNAL(rangeChanged(const QCPRange&)), this, SLOT(tryGraph(const QCPRange&)));
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
    //QHBoxLayout* subLayout = new QHBoxLayout();

    Widget* widget = new Widget{ui->plot};

    layout->insertLayout(0, widget->subLayout);

    buttonToWidget.insert(widget->buttonRm, widget);
    buttonToWidget.insert(widget->buttonHd, widget);
    buttonToWidget.insert(widget->buttonEn, widget);
    hiddenList.insert(widget->buttonHd, false);

    QObject::connect(widget->buttonRm, &QToolButton::clicked, this, &MainWindow::removeWidget);
    QObject::connect(widget->buttonHd, &QToolButton::clicked, this, &MainWindow::hideWidget);
    QObject::connect(widget->buttonEn, &QToolButton::clicked, this, &MainWindow::readInput);
}

void MainWindow::removeWidget() {
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    QHBoxLayout* layout = buttonToWidget.take(button)->subLayout;

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

void MainWindow::readInput() {
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    Widget* widget = buttonToWidget.value(button);

    inputStr = widget->txt_inputBar->text();
    Parser parser(inputStr);

    outputStr = parser.getOutputStr();
    qDebug() << outputStr;

    if (outputStr == "Error: input invalid") {
        widget->buttonCol->setStyleSheet({"color: red"});
        widget->buttonCol->setText("X");
    }
    else {
        widget->buttonCol->setText("");
        widget->buttonCol->setStyleSheet(widget->color);

        // update graph
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
    //ui->plot->addGraph();
    //connect(ui->plot, SIGNAL(mouseDoubleClick(QMouseEvent*)), SLOT(clickedGraph(QMouseEvent*)));

}

void MainWindow::tryGraph(const QCPRange& range) {
    //QCPGraph* graph = ui->plot->addGraph();
    /*auto interval = ui->plot->xAxis->range().upper - ui->plot->xAxis->range().lower;

    for (double i = 0; ui->plot->xAxis->range().upper; i++) {
        double x = i/1000;
        double y = x;

        graph->addData(x, y);
    }

    for (double i = 0; -i > ui->plot->xAxis->range().lower * 1000; i++) {
        double x = -i/1000;
        double y = x;

        graph->addData(x, y);
    }*/

    auto&& axis = ui->plot->xAxis;

    auto&& tickVector =axis->tickVector();
    //auto&& tickVectorLabels = axis->tickVectorLabels();

    auto&& tickVectorIterator = tickVector.begin();
    //auto&& tickVectorLabelsIterator = tickVectorLabels.begin();

    constexpr auto precision = 1000;
    const auto tick = ( *( tickVectorIterator + 1 ) - *tickVectorIterator ) / precision;
    const auto max = ( range.upper - range.lower ) / tick;
    auto value = range.lower;

    graph->data().data()->clear();

    for ( auto i = 0; i < max; ++i, value += tick )
        graph->addData( value, qSin( value ) );
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

void MainWindow::updateGraphs(const QCPRange& range) {
    auto&& axis = ui->plot->xAxis;

    auto&& tickVector =axis->tickVector();
    auto&& tickVectorIterator = tickVector.begin();

    constexpr auto precision = 1000;
    const auto tick = ( *( tickVectorIterator + 1 ) - *tickVectorIterator ) / precision;
    const auto max = ( range.upper - range.lower ) / tick;

    foreach (Widget* widget, buttonToWidget) {
        auto value = range.lower;

        widget->graph->data().data()->clear();

        for ( auto i = 0; i < max; ++i, value += tick )
            widget->graph->addData( value, qSin( value ) );
    }
}

void MainWindow::on_btn_addFunc_clicked() {
    addWidget();
}

