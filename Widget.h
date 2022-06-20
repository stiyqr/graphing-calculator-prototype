#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets>

#include "qcustomplot.h"
#include "Parser.h"

class Widget {
public:
    QSharedPointer<QToolButton> buttonCol;
    QSharedPointer<QLineEdit>   txt_inputBar;
    QSharedPointer<QToolButton> buttonEn;
    QSharedPointer<QToolButton> buttonHd;
    QSharedPointer<QToolButton> buttonRm;
    QCPGraph*                   graph;
    QSharedPointer<QHBoxLayout> subLayout;

    QVBoxLayout* layout;

    //QCustomPlot* plot;


    QString color;
    int     rgb1, rgb2, rgb3;
    bool    hidden = false;

    Parser parser;

public:
    Widget(){}
    Widget(QCustomPlot*, QVBoxLayout*);
    ~Widget();

    QString randomColor();
};

#endif // WIDGET_H
