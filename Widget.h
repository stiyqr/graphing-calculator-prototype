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
    QCustomPlot* plot;

    QHBoxLayout* subLayout;

    QString color;
    int     rgb1, rgb2, rgb3;
    bool    hidden = false;

    Parser parser;

public:
    Widget(){}
    Widget(QCustomPlot*);
    ~Widget();

    QString randomColor();
};

#endif // WIDGET_H
