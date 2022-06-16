#include "security-list-item.h"
#include <kiran-log/qt5-log-i.h>
#include <QFile>
#include <QTimer>
#include "ui_security-list-item.h"

#define TIMEOUT 1000
SecurityListItem::SecurityListItem(QString text, QWidget *parent) : QWidget(parent),
                                                                    ui(new Ui::SecurityListItem),
                                                                    m_isPathCorrect(false)
{
    ui->setupUi(this);
    ui->label_name->setText(text);
    ui->lab_error_tips->setText(tr("the path format is wrong"));
    ui->lab_error_tips->setStyleSheet("color:#F56C6C;font-size:14px;");
    ui->lab_error_tips->setVisible(false);
    ui->btn_add->setCursor(Qt::PointingHandCursor);
    ui->btn_delete->setCursor(Qt::PointingHandCursor);
    ui->lineEdit->setTextMargins(10, 0, 0, 0);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout,
            [this] {
                checkPath();
                m_timer->stop();
            });

    connect(ui->btn_add, &QToolButton::clicked,
            [this] {
                emit sigAdd();
            });
    connect(ui->btn_delete, &QToolButton::clicked,
            [this] {
                emit sigDelete();
            });
    connect(ui->lineEdit, &QLineEdit::textChanged,
            [this](QString text) {
                if (!text.isEmpty())
                    m_timer->start();
                else
                    ui->lab_error_tips->hide();
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
    if (m_isPathCorrect)
        return ui->lineEdit->text();
    else
        return "";
}

bool SecurityListItem::getPathCorrect()
{
    return m_isPathCorrect;
}

void SecurityListItem::checkPath()
{
    QString path = ui->lineEdit->text();
    if (!path.isEmpty())
    {
        QRegExp regExp("^/(\\w+/?)+$");
        if (regExp.exactMatch(path))
            m_isPathCorrect = true;
        else
            m_isPathCorrect = false;

        ui->lab_error_tips->setVisible(!m_isPathCorrect);
    }
}
