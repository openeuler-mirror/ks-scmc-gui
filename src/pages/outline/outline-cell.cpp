#include "outline-cell.h"
#include "ui_outline-cell.h"
#include "main-window.h"

OutlineCell::OutlineCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutlineCell)
{
    is_mouse_Press = false;
//    m_outline_view = new OutlineView(this);
    ui->setupUi(this);
    this->setIcon();
    ui->frame->setStyleSheet("background: #2d2d2d;");
//    ui->label_online->setMinimumSize(20,20);
}

OutlineCell::~OutlineCell()
{
    delete ui;
}

void OutlineCell::setOutlineCellName(const QString name)
{
    ui->label->setText(name);
}

void OutlineCell::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    is_mouse_Press = true;
//    emit outlineCellStep(ONUTLINE);
    emit isPress();
}

void OutlineCell::setIcon()
{
    ui->cell_name->setStyleSheet("QLabel{"
                               "background-image:url(:/images/dot.svg);"
                               "background-position:left;"
                               "background-origin:content;"
                               "background-repeat:none;"
                               "}");
}
