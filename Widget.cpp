#include "Widget.h"


Widget::Widget(QCustomPlot* _plot) {
    qDebug() << "widget created";

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

    plot = _plot;
    graph = plot->addGraph();

    // set graph color
    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor(rgb1, rgb2, rgb3));
    graph->setPen(pen);
}

Widget::~Widget() {
    qDebug() << "widget destroyed";

    // delete variable
    /*QString str = txt_inputBar->text(), varCheck = "";
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '=') break;
        varCheck += str[i];
    }

    parser.var.variables.erase(varCheck);

    // buttons and input bar
    //delete buttonCol;
    //delete buttonEn;
    //delete buttonHd;
    //delete buttonRm;
    //delete txt_inputBar;

    // delete sublayout
    /*QLayoutItem* item;;
    while ((item = subLayout->takeAt(0))!= 0) {
        if (item->layout() != 0) {
            QLayout* layout = item->layout();
            QLayoutItem* layoutItem;

            while (layout->count() != 0) {
                layoutItem = layout->takeAt(0);
                delete layoutItem->widget();
                delete layoutItem;
            }
        }
        delete item->widget();
        delete item;
    }
    delete subLayout->widget();
    delete subLayout;*/

    // delete widget
    /*while (subLayout->count() != 0) {
        QLayoutItem* item = subLayout->takeAt(0);

        delete item->widget();
        delete item;
    }
    delete subLayout;

    // delete graph
    if (parser.inputType == Parser::IType::Y || parser.inputType == Parser::IType::X) {
        graph->setVisible(false);
        graph->data().data()->clear();
        plot->replot();
        plot->removeGraph(graph);
        plot->clearPlottables();
    }
    delete graph;*/
}

QString Widget::randomColor() {
    rgb1 = rand() % 256;
    rgb2 = rand() % 256;
    rgb3 = rand() % 256;

    QString colorStr = "background-color: rgb(" + QString::number(rgb1) + ", " + QString::number(rgb2)
            + ", " + QString::number(rgb3) + ")";

    color = colorStr;

    //qDebug() << colorStr;

    return colorStr;
}
