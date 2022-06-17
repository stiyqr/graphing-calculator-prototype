#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets>
#include "qcustomplot.h"

class Widget {
public:
    QToolButton* buttonCol;
    QLineEdit*   txt_inputBar;
    QToolButton* buttonEn;
    QToolButton* buttonHd;
    QToolButton* buttonRm;
    QCPGraph*    graph;

    QColor color;
    bool   hidden = false;


    Widget(){}
    Widget(QHBoxLayout*, QCustomPlot*);

    QString randomColor();
};

#endif // WIDGET_H
