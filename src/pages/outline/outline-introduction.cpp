/**
 * @file          /ks-scmc-gui/src/pages/outline/outline-introduction.cpp
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "outline-introduction.h"
#include "ui_outline-introduction.h"

OutlineIntroduction::OutlineIntroduction(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutlineIntroduction)
{
    ui->setupUi(this);

    this->setStyleSheet("background-image: url(:/images/banner.png);");
    //                       "border:10px solid #000000;"
    //                         "min-width: 1668px;"
    //                         "min-height: 204px;"

//    ui->label_2->setStyleSheet("QLabel {background-color: transparent;}");
//    ui->label_1->autoFillBackground();
    ui->label_1->setStyleSheet("QLabel {"
                                "background: transparent;"
                                "border:none;"
                                "font: NotoSansCJKsc-Regular;"
                                "font-size: 36px;"
                                "border-radius:0px;"
                                "color: #ffffff;}");
    ui->label_1->setText(tr("Container platform"));
    ui->label_2->setStyleSheet("QLabel {"
                                "background: transparent;"
                                "border:none;"
                                "font: NotoSansCJKsc-Regular;"
                                "font-size: 14px;"
                                "border-radius:0px;"
                                "color: #ffffff;}");
    ui->label_2->setText(tr("The security container management client mainly provides a management interface to realize the management of the server.\n\
It mainly includes four modules: user management, node management, image and image warehouse management, and container management."));
}

OutlineIntroduction::~OutlineIntroduction()
{
    delete ui;
}
