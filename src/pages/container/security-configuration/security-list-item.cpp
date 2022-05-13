#include "security-list-item.h"
#include <QFile>
#include "ui_security-list-item.h"
SecurityListItem::SecurityListItem(QString text, QWidget *parent) : QWidget(parent),
                                                                    ui(new Ui::SecurityListItem),
                                                                    m_isPathCorrect(false)
{
    ui->setupUi(this);
    ui->label_name->setText(text);
    ui->lab_error_tips->setText("the process path is wrong");
    ui->lab_error_tips->setStyleSheet("color:#F56C6C");
    ui->lab_error_tips->setVisible(false);
    ui->btn_add->setCursor(Qt::PointingHandCursor);
    ui->btn_delete->setCursor(Qt::PointingHandCursor);

    connect(ui->btn_add, &QToolButton::clicked,
            [this] {
                emit sigAdd();
            });
    connect(ui->btn_delete, &QToolButton::clicked,
            [this] {
                emit sigDelete();
            });
    connect(ui->lineEdit, &QLineEdit::editingFinished, this, &SecurityListItem::checkPath);
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
        QFile file(path);
        if (!file.exists())
        {
            ui->lab_error_tips->show();
            m_isPathCorrect = false;
        }
        else
        {
            ui->lab_error_tips->hide();
            m_isPathCorrect = true;
        }
    }
}
