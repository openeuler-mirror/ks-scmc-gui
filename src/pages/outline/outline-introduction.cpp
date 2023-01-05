/**
 * @file          /ks-scmc-gui/src/pages/outline/outline-introduction.cpp
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "outline-introduction.h"
#include "ui_outline-introduction.h"

OutlineIntroduction::OutlineIntroduction(QWidget *parent) : QWidget(parent),
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
    ui->label_1->setText(tr("Security container platform"));
    ui->label_2->setStyleSheet("QLabel {"
                               "background: transparent;"
                               "border:none;"
                               "font: NotoSansCJKsc-Regular;"
                               "font-size: 12px;"
                               "border-radius:0px;"
                               "color: #ffffff;}");
    ui->label_2->setText(tr("KylinSec container management system is built on the domestic Linux system to realize the self-developed security container platform. It provides \n"
                            "a security container operation platform with fine-grained permission control for the new generation of power systems, and provides users with simple and \n"
                            "easy-to-use experience through a friendly human-machine interface. The KylinSec container management system has the main functions of transparent deployment \n"
                            "of applications, isolated operation of applications, centralized monitoring of application operation status, and minimum permission control strategy.\n"));
}

OutlineIntroduction::~OutlineIntroduction()
{
    delete ui;
}
