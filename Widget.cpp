#include "Widget.h"


Widget::Widget(QCustomPlot* plot) {
    subLayout = new QHBoxLayout();

    buttonCol = new QToolButton();
    buttonCol->setStyleSheet({randomColor()});
    subLayout->addWidget(buttonCol);

    QString lineStr = "y=";
    txt_inputBar = new QLineEdit(lineStr);
    subLayout->addWidget(txt_inputBar);

    buttonEn = new QToolButton();
    buttonEn->setIcon(QIcon("../assets/icon-enter.png"));
    subLayout->addWidget(buttonEn);

    buttonHd = new QToolButton();
    buttonHd->setIcon(QIcon("../assets/icon-hide.png"));
    subLayout->addWidget(buttonHd);

    buttonRm = new QToolButton();
    buttonRm->setIcon(QIcon("../assets/icon-remove.png"));
    subLayout->addWidget(buttonRm);

    graph = plot->addGraph();
}

QString Widget::randomColor() {
    int rgb1 = rand() % 256;
    int rgb2 = rand() % 256;
    int rgb3 = rand() % 256;

    QString colorStr = "background-color: rgb(" + QString::number(rgb1) + ", " + QString::number(rgb2)
            + ", " + QString::number(rgb3) + ")";

    color = colorStr;

    //qDebug() << colorStr;

    return colorStr;
}
