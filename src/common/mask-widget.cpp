/**
 * @file          /ks-scmc-gui/src/common/mask-widget.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "mask-widget.h"
#include <kiran-log/qt5-log-i.h>
#include <QGridLayout>
#include <QMouseEvent>
#include <QMovie>
MaskWidget::MaskWidget(QWidget *parent) : QWidget(parent), m_labLoading(nullptr)
{
    initUI();
}

void MaskWidget::setMaskVisible(bool visible)
{
    this->setVisible(visible);
    if (visible)
        m_movie->start();
    else
        m_movie->stop();
}

bool MaskWidget::maskIsVisible()
{
    return this->isVisible();
}

MaskWidget::~MaskWidget()
{
    if (m_movie)
    {
        m_movie->stop();
        delete m_movie;
        m_movie = nullptr;
    }
}

void MaskWidget::initUI()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setMargin(0);
    gridLayout->setSpacing(0);

    m_labLoading = new QLabel(this);
    m_labLoading->setFixedSize(128, 128);
    m_movie = new QMovie(":/images/loading.gif");
    m_labLoading->setMovie(m_movie);
    gridLayout->addWidget(m_labLoading, 0, 0, Qt::AlignCenter);

    hide();
}
