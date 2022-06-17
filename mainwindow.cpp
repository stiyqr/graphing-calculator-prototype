#include "mainwindow.h"
#include "ui_mainwindow.h"

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



    /////////////////////////////////////////////// try area ///////////////////////////////////////////////


    QCPItemLine *line = new QCPItemLine(ui->plot);
    line->setPen(QPen(Qt::green));
    //line->start->setCoords(ui->plot->xAxis->range().lower);

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

    QToolButton* buttonCol = new QToolButton();
    buttonCol->setStyleSheet({randomColor()});
    subLayout->addWidget(buttonCol);

    QString lineStr = "y=";
    QLineEdit* txt_inputBar = new QLineEdit(lineStr);
    subLayout->addWidget(txt_inputBar);

    QToolButton* buttonHd = new QToolButton();
    auto hdAct = new QAction();
    hdAct->setIcon(QIcon("../assets/icon-hide.png"));
    buttonHd->setDefaultAction(hdAct);
    subLayout->addWidget(buttonHd);

    QToolButton* buttonRm = new QToolButton();
    auto rmAct = new QAction();
    rmAct->setIcon(QIcon("../assets/icon-remove.png"));
    buttonRm->setDefaultAction(rmAct);
    subLayout->addWidget(buttonRm);

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

QString MainWindow::randomColor() {
    int rgb1 = rand() % 256;
    int rgb2 = rand() % 256;
    int rgb3 = rand() % 256;

    QString colorStr = "background-color: rgb(" + QString::number(rgb1) + ", " + QString::number(rgb2)
            + ", " + QString::number(rgb3) + ")";

    qDebug() << colorStr;

    return colorStr;
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
