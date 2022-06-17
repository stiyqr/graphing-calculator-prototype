#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets>

#include "qcustomplot.h"
#include "Parser.h"

class Widget {
public:
    QToolButton* buttonCol;
    QLineEdit*   txt_inputBar;
    QToolButton* buttonEn;
    QToolButton* buttonHd;
    QToolButton* buttonRm;
    QCPGraph*    graph;

    QHBoxLayout* subLayout;

    QString color;
    bool    hidden = false;

    Parser parser;

public:
    Widget(){}
    Widget(QCustomPlot*);

    QString randomColor();
};

#endif // WIDGET_H
