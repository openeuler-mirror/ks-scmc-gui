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
    ui->label_2->setText(tr("A single neutral console manages heterogeneous infrastructure, interfaces with existing business process platforms, and logically Pools Resources on the cloud,\n\
So as to implement resource allocation based on enterprise organizational structure and provide standard processes and standardized cloud services."));
}

OutlineIntroduction::~OutlineIntroduction()
{
    delete ui;
}
