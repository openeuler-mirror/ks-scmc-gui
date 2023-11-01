/**
 * @file          /ks-scmc-gui/src/common/guide-item.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "guide-item.h"
#include <kiran-log/qt5-log-i.h>
#include <QPainter>
#include <QStyleOption>
#include "ui_guide-item.h"
GuideItem::GuideItem(QString text, QString icon, int type, QWidget *parent) : QWidget(parent),
                                                                              ui(new Ui::GuideItem),
                                                                              m_type(type),
                                                                              m_delBtn(nullptr),
                                                                              m_position(TIP_LINE_POSITION_LEFT),
                                                                              m_text(text)
{
    ui->setupUi(this);
    initUI(text, icon, type);
}

GuideItem::~GuideItem()
{
    delete ui;
}

void GuideItem::setArrow(bool isHide)
{
    if (isHide)  //down
    {
        ui->label_right_icon->setStyleSheet("#label_right_icon{image: url(:/images/down-arrow.svg);}");
    }
    else  //up
    {
        ui->label_right_icon->setStyleSheet("#label_right_icon{image: url(:/images/up-arrow.svg);}");
    }
}

void GuideItem::setTipLinePosition(TipLinePosition position)
{
    m_position = position;
}

void GuideItem::setSelected(bool isSelected)
{
    if (m_isSelected != isSelected)
    {
        m_isSelected = isSelected;
        update();
    }
}

int GuideItem::getItemType()
{
    return m_type;
}

QString GuideItem::getItemText()
{
    return m_text;
}

void GuideItem::setDeleteBtn()
{
    if (ui->label_right_icon)
    {
        delete ui->label_right_icon;
        ui->label_right_icon = nullptr;
    }
    QLayout *layout = this->layout();
    m_delBtn = new QToolButton(this);
    m_delBtn->setObjectName("delBtn");
    m_delBtn->setFixedSize(15, 15);
    m_delBtn->setIcon(QIcon(":/images/delete.svg"));
    m_delBtn->hide();
    layout->addWidget(m_delBtn);
    connect(m_delBtn, &QToolButton::clicked, this, &GuideItem::onDelete);
}

void GuideItem::setName(QString name)
{
    ui->label_text->setText(name);
}

void GuideItem::removeDeleteBtn()
{
    if (m_delBtn)
    {
        delete m_delBtn;
        m_delBtn = nullptr;
    }
}

void GuideItem::setDeleteBtnVisible(bool visible)
{
    m_delBtnVisible = visible;
}
void GuideItem::initUI(QString text, QString lIcon, int type)
{
    ui->label_text->setText(text);

    if (!lIcon.isEmpty())
    {
        ui->label_left_icon->setStyleSheet(QString("#label_left_icon{image:url(%1)}").arg(lIcon));
    }

    if (type == GUIDE_ITEM_TYPE_GROUP)
        ui->label_right_icon->setPixmap(QPixmap());  //arrow
}

void GuideItem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QRect rect;

    if (m_position == TIP_LINE_POSITION_LEFT)
    {
        rect.setX(this->geometry().x());
        rect.setY(this->geometry().x());
        rect.setWidth(2);
        rect.setHeight(this->geometry().height());
    }
    else
    {
        rect.setX(this->geometry().topRight().x() - 2);
        rect.setY(this->geometry().x());
        rect.setWidth(2);
        rect.setHeight(this->geometry().height());
    }
    if (m_isSelected && m_type != GUIDE_ITEM_TYPE_GROUP)
    {
        p.fillRect(rect, QColor(46, 179, 255));
    }
}

void GuideItem::enterEvent(QEvent *event)
{
    if (m_delBtn && m_delBtnVisible)
        m_delBtn->setVisible(true);
}

void GuideItem::leaveEvent(QEvent *event)
{
    if (m_delBtn)
        m_delBtn->setVisible(false);
}

void GuideItem::onDelete()
{
    emit sigDeleteItem();
}
