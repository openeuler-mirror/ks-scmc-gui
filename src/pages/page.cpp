#include "page.h"
#include <kiran-log/qt5-log-i.h>
#include <QPalette>
#include "mask-widget.h"
Page::Page(QWidget *parent) : QWidget(parent), m_maskWidget(nullptr), m_data(QVariant())
{
    KLOG_INFO() << "Page";
    m_maskWidget = new MaskWidget(this);
    m_maskWidget->setFixedSize(this->size());  //设置窗口大小
    this->stackUnder(qobject_cast<QWidget *>(m_maskWidget));
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
        pal.setColor(QPalette::Background, QColor(0x00, 0x00, 0x00, 0x20));
        m_maskWidget->setPalette(pal);
        m_maskWidget->setFixedSize(this->size());
    }
}
