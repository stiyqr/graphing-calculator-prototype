#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <map>
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

    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(addNumberLabelX(QCPRange)));
    connect(ui->plot->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(addNumberLabelY(QCPRange)));
    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(updateGraphY(QCPRange)));
    connect(ui->plot->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(updateGraphX(QCPRange)));
    //connect(ui->plot->yAxis, SIGNAL(rangeChanged(const QCPRange&)), this, SLOT(replotGraphs()));


    /////////////////////////////////////////////// try area ///////////////////////////////////////////////

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

    qDebug() << "about to add widget";
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui->vLayout);
    //QHBoxLayout* subLayout = new QHBoxLayout();

    ////qDebug() << "new widget";
    Widget* widget = new Widget{ui->plot};

    ////qDebug() << "insert layout";
    layout->insertLayout(0, widget->subLayout);

    //vWidget.emplace_back(widget);

    ////qDebug() << "insert buttons";
    buttonToWidget.insert(widget->buttonRm, widget);
    buttonToWidget.insert(widget->buttonHd, widget);
    buttonToWidget.insert(widget->buttonEn, widget);
    hiddenList.insert(widget->buttonHd, false);

    QObject::connect(widget->buttonRm, &QToolButton::clicked, this, &MainWindow::removeWidget);
    QObject::connect(widget->buttonHd, &QToolButton::clicked, this, &MainWindow::hideWidget);
    QObject::connect(widget->buttonEn, &QToolButton::clicked, this, &MainWindow::readInput);

    QObject::connect(widget->buttonEn, &QToolButton::clicked, this, &MainWindow::displayVarList);
    QObject::connect(widget->buttonEn, &QToolButton::clicked, this, &MainWindow::displayOutput);

    QObject::connect(widget->buttonRm, &QToolButton::clicked, this, &MainWindow::displayVarList);
}

void MainWindow::removeWidget() {
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    qDebug() << "obtain remove sender";
    //auto it = std::find_if(vWidget.begin(), vWidget.end(), [&](Widget* widget) {return widget->buttonRm == button;});
    //auto widget = *it;

    /*if (it != vWidget.end()) {
        vWidget.erase(it);
        //delete *it;
    }*/

    //std::remove_if(vWidget.begin(), vWidget.end(), [&](Widget* widget) {return widget->buttonRm == button;});

    Widget* widget = buttonToWidget.take(button);
    QHBoxLayout* layout = widget->subLayout;
    ////qDebug() << "assign widget and layout";

    // delete variable
    if (widget->parser.inputType == Parser::IType::VAR) {
        auto it = widget->parser.var.variables.find(widget->parser.inputVar);
        if ( it != widget->parser.var.variables.end()) {
            ////qDebug() << "found var to be deleted, var: " << it->first;
            widget->parser.var.variables.erase(it);
        }
    }
    ////qDebug() << "deleted variable";

    // delete widget
    while (layout->count() != 0) {
        QLayoutItem* item = layout->takeAt(0);

        delete item->widget();
        delete item;
    }
    delete layout;
    ////qDebug() << "deleted layout";

    // delete graph
    if (widget->parser.inputType == Parser::IType::Y || widget->parser.inputType == Parser::IType::X) {
        widget->graph->setVisible(false);
        widget->graph->data().data()->clear();
        //widget->graph->parentPlot()->removeGraph(widget->graph);
        ////qDebug() << "deleted a graph";
    }
    ui->plot->replot();
    ////qDebug() << "deleted graph";

    //std::remove_if(buttonToWidget.begin(), buttonToWidget.end(), [&](Widget* _widget) {return _widget == _widget;});
}

void MainWindow::hideWidget() {
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    Widget* widget = buttonToWidget.value(button);
    qDebug() << "obtain hide sender";

    //auto it = std::find_if(vWidget.begin(), vWidget.end(), [&](Widget* widget) {return widget->buttonRm == button;});
    //auto widget = *it;

    auto&& hidden = hiddenList[button];
    hidden = !hidden;

    if (hidden) {
        button->setIcon(QIcon("../assets/icon-hidden.png"));
        widget->graph->setVisible(false);
        ui->plot->replot();
        ////qDebug() << "hide widget";
    }
    else {
        button->setIcon(QIcon("../assets/icon-hide.png"));
        widget->graph->setVisible(true);
        ui->plot->replot();
        ////qDebug() << "unhide widget";
    }
}

void MainWindow::readInput() {
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    Widget* widget = buttonToWidget.value(button);
    qDebug() << "obtain read sender";

    //auto it = std::find_if(vWidget.begin(), vWidget.end(), [&](Widget* widget) {return widget->buttonEn == button;});
    //if (it == vWidget.end()) return;
    //auto widget = *it;

    inputStr = widget->txt_inputBar->text();
    widget->parser.var.variables["x"] = "0";
    widget->parser.var.variables["y"] = "0";
    widget->parser.init(inputStr);

    outputStr = widget->parser.getOutputStr();
    ////qDebug() << "   init done";
    ////qDebug() << "   input: " << inputStr;
    ////qDebug() << "   output: " << outputStr;
    ////qDebug() << "   resultToken: " << widget->parser.resultToken.getNum();

    if (outputStr == "Error: input invalid" || widget->parser.inputValid == false) {
        ////qDebug() << "input invalid";

        widget->buttonCol->setStyleSheet({"color: red"});
        widget->buttonCol->setText("X");
        widget->graph->setVisible(false);
        ui->plot->replot();

        ////qDebug() << "displayed invalid icon";
    }
    else {
        widget->buttonCol->setText("");
        widget->buttonCol->setStyleSheet(widget->color);

        ////qDebug() << "about to update graph";
        // update graph
        if (widget->parser.inputType == Parser::IType::Y) {
            widget->graph->setVisible(true);
            ui->plot->replot();
            ////qDebug() << "updating graph Y";
            updateGraphY(ui->plot->xAxis->range());
        }
        else if (widget->parser.inputType == Parser::IType::X) {
            widget->graph->setVisible(true);
            ui->plot->replot();
            ////qDebug() << "updating graph X";
            updateGraphX(ui->plot->yAxis->range());
        }
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

void MainWindow::displayVarList() {
    ////qDebug() << "display varlist";
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    Widget* widget = buttonToWidget.value(button);

    QString finalStr = "";
    for (auto&& [key, value] : widget->parser.var.variables) {
        QString outputStr = key + " = " + value + "\n";
        finalStr += outputStr;
    }
    ui->txt_varlist->setPlainText(finalStr);
}

void MainWindow::displayOutput() {
    ////qDebug() << "display output";
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    Widget* widget = buttonToWidget.value(button);

    QString outputStr = widget->parser.outputStr;
    QString outputNum = QString::number(widget->parser.resultToken.getNum());

    ui->txt_output->setPlainText("outputstr: " + outputStr + ", num: " + outputNum);
}

void MainWindow::addNumberLabelX(const QCPRange& range) {
    static QVector<QCPItemText*> numLabelx;

    while (!numLabelx.isEmpty()) {
        ui->plot->removeItem(numLabelx.back());
        //delete numLabelx.back();
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
    ui->plot->replot();
}

void MainWindow::addNumberLabelY(const QCPRange& range) {
    //ui->plot->yAxis->setScaleRatio(ui->plot->xAxis, 1.0);

    static QVector<QCPItemText*> numLabely;

    while (!numLabely.isEmpty()) {
        ui->plot->removeItem(numLabely.back());
        //delete numLabely.back();
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
    ui->plot->replot();
}

void MainWindow::updateGraphY(const QCPRange& range) {
    qDebug() << "ENTER update graph y";

    auto&& axis = ui->plot->xAxis;

    auto&& tickVector =axis->tickVector();
    auto&& tickVectorIterator = tickVector.begin();

    constexpr auto precision = 100;
    const auto tick = ( *( tickVectorIterator + 1 ) - *tickVectorIterator ) / precision;
    const auto max = ( range.upper - range.lower ) / tick;

    //std::map<QString, QString> dummyX;

    ////qDebug() << "about to clear graph Y data";
    foreach(Widget* widget, buttonToWidget) {
        if (widget->parser.inputType == Parser::IType::Y) {
            widget->graph->data().data()->clear();
        }
    }

    ////qDebug() << "about to draw graph Y data";
    foreach (Widget* widget, buttonToWidget) {
        if (widget->graph->dataCount() != 0 || widget->parser.inputType != Parser::IType::Y) continue;

        auto value = range.lower;

        for ( auto i = 0; i < max; ++i, value += tick ) {
            //dummyX["x"] = QString::number(value);
            //dummyX["X"] = QString::number(value);

            widget->parser.var.variables["x"] = QString::number(value);
            //qDebug() << "variable[x]" << widget->parser.var.variables["x"];
            //qDebug() << "input: " << inputStr;
            //qDebug() << "output: " << outputStr;
            //qDebug() << "resultToken: " << widget->parser.resultToken.getNum();

            //widget->parser.init(widget->parser.inputStr);
            widget->parser.readMainStack();

            widget->graph->addData( value, widget->parser.resultToken.getNum() );

        }
        ui->plot->replot();
    }
}

void MainWindow::updateGraphX(const QCPRange& range) {
    qDebug() << "ENTER update graph x";

    auto&& axis = ui->plot->yAxis;

    auto&& tickVector =axis->tickVector();
    auto&& tickVectorIterator = tickVector.begin();

    constexpr auto precision = 100;
    const auto tick = ( *( tickVectorIterator + 1 ) - *tickVectorIterator ) / precision;
    const auto max = ( range.upper - range.lower ) / tick;

    //std::map<QString, QString> dummyX;

    ////qDebug() << "about to clear graph X data";
    foreach(Widget* widget, buttonToWidget) {
        if (widget->parser.inputType == Parser::IType::X) {
            widget->graph->data().data()->clear();
        }
    }

    ////qDebug() << "about to draw graph X data";
    foreach (Widget* widget, buttonToWidget) {
        if (widget->graph->dataCount() != 0 || widget->parser.inputType != Parser::IType::X) continue;

        auto value = range.lower;

        for ( auto i = 0; i < max; ++i, value += tick ) {

            widget->parser.var.variables["y"] = QString::number(value);
            //widget->parser.init(widget->parser.inputStr);
            widget->parser.readMainStack();

            widget->graph->addData( widget->parser.resultToken.getNum(), value  );

        }
        ui->plot->replot();

        //widget->parser.mainStackToStr();
    }
}

void MainWindow::replotGraphs() {
    ui->plot->replot();
}

void MainWindow::on_btn_addFunc_clicked() {
    addWidget();
}

