/**
 * @file          /ks-scmc-gui/src/pages/page.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "page.h"
#include <kiran-log/qt5-log-i.h>
#include <QPalette>
#include "mask-widget.h"
Page::Page(QWidget *parent) : QWidget(parent), m_maskWidget(nullptr), m_data(QVariant())
{
    m_maskWidget = new MaskWidget();
    m_maskWidget->setFixedSize(this->size());  //设置窗口大小
    //this->stackUnder(qobject_cast<QWidget *>(m_maskWidget));
}

Page::~Page()
{
    if (m_maskWidget)
    {
        delete m_maskWidget;
        m_maskWidget = nullptr;
    }
}

//调用setbusy之前设置加载动画类父控件，只有指定了父控件，动画才能显示在对应的控件上
void Page::setMaskParent(QWidget *parent)
{
    m_maskWidget->setParent(parent);
}

void Page::setBusy(bool status)
{
    m_maskWidget->setMaskVisible(status);
}

void Page::setData(QVariant data)
{
    m_data = data;
}

QVariant Page::getData()
{
    return m_data;
}

void Page::resizeEvent(QResizeEvent *event)
{
    if (event)
    {
    }  //消除警告提示

    if (m_maskWidget != nullptr)
    {
        m_maskWidget->setAutoFillBackground(true);
        QPalette pal = m_maskWidget->palette();
        pal.setColor(QPalette::Background, QColor(186, 186, 186, 50));
        m_maskWidget->setPalette(pal);
        m_maskWidget->setFixedSize(this->size());
    }
}
