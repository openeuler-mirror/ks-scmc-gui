/**
 * @file          /ks-scmc-gui/src/pages/container/security-configuration/security-list-item.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "security-list-item.h"
#include <kiran-log/qt5-log-i.h>
#include <QFile>
#include <QTimer>
#include "ui_security-list-item.h"

#define TIMEOUT 1000
SecurityListItem::SecurityListItem(QString text, QWidget *parent) : QWidget(parent),
                                                                    ui(new Ui::SecurityListItem),
                                                                    m_isPathCorrect(true)
{
    ui->setupUi(this);
    ui->label_name->setText(text);
    ui->lab_error_tips->setText(tr("the path format is wrong"));
    ui->lab_error_tips->setStyleSheet("color:#F56C6C;font-size:12px;");
    ui->lab_error_tips->setVisible(false);
    ui->btn_add->setIcon(QIcon(":/images/icon-add.svg"));
    ui->btn_delete->setIcon(QIcon(":/images/icon-delete.svg"));
    ui->btn_add->setCursor(Qt::PointingHandCursor);
    ui->btn_delete->setCursor(Qt::PointingHandCursor);
    ui->lineEdit->setTextMargins(10, 0, 0, 0);

    connect(ui->btn_add, &QToolButton::clicked,
            [this]
            {
                emit sigAdd();
            });
    connect(ui->btn_delete, &QToolButton::clicked,
            [this]
            {
                emit sigDelete();
            });
    connect(ui->lineEdit, &QLineEdit::textChanged,
            [this](QString text)
            {
                ui->lineEdit->setToolTip(tooptipWordWrap(text));
                checkPath();
            });
}

SecurityListItem::~SecurityListItem()
{
    delete ui;
}

void SecurityListItem::setDeleteBtnVisible(bool visible)
{
    ui->btn_delete->setVisible(visible);
}

void SecurityListItem::updateItemName(QString text)
{
    ui->label_name->setText(text);
}

void SecurityListItem::setInfo(QString text)
{
    ui->lineEdit->setText(text);
}

QString SecurityListItem::getInfo()
{
    return ui->lineEdit->text();
}

bool SecurityListItem::getPathCorrect()
{
    return m_isPathCorrect;
}

void SecurityListItem::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange)
    {
        ui->lineEdit->setCursorPosition(0);
    }
}

void SecurityListItem::checkPath()
{
    QString path = ui->lineEdit->text();
    m_isPathCorrect = true;
    if (!path.isEmpty())
    {
        QRegExp regExp("^/.+$");
        if (!regExp.exactMatch(path))
            m_isPathCorrect = false;
    }

    ui->lab_error_tips->setVisible(!m_isPathCorrect);
}

QString SecurityListItem::tooptipWordWrap(const QString &org)
{
    QString result;
    QFontMetrics fm(fontMetrics());
    int textWidthInPxs = fm.width(org);
    const int rear = org.length();
    int pre = 0, vernier = 1;
    unsigned int pickUpWidthPxs = 0;
    QString pickUp;

    if (textWidthInPxs <= width())
    {
        result = org;
        return result;
    }

    while (vernier <= rear)
    {
        pickUp = org.mid(pre, vernier - pre);
        pickUpWidthPxs = fm.width(pickUp);
        if (pickUpWidthPxs >= width())
        {
            result += pickUp + "\n";  // 插入换行符，或者使用<br/>标签
            pre = vernier;
            pickUpWidthPxs = 0;
        }
        ++vernier;
    }

    if (pickUpWidthPxs < width() && !pickUp.isEmpty())
    {
        result += pickUp;
    }

    return result;
}
