/**
 * @file          src/common/ip-mask-lineedit.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "ip-mask-lineedit.h"
#include <kiran-log/qt5-log-i.h>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QStyleOption>

IpMaskLineEdit::IpMaskLineEdit(QWidget *parent, bool withMask)
    : QLineEdit(parent),
      m_withMask(withMask),
      m_maskEditor(nullptr)
{
    setupUi();
    setReadOnly(true);
}

void IpMaskLineEdit::setIpMask(QString ipMask)
{
    // 判断是否包含掩码
    if (ipMask.contains("/"))
    {
        auto parts = ipMask.split("/");
        if (parts.size() != 2)
        {
            KLOG_DEBUG() << "Invalid ip mask:" << ipMask;
            return;
        }

        setIp(parts[0]);
        if (m_maskEditor)
            m_maskEditor->setText(parts[1]);
    }
    else
    {
        setIp(ipMask);
    }
}

void IpMaskLineEdit::setIp(QString ip)
{
    if (ip.isEmpty())
    {
        return;
    }

    auto parts = ip.split(".");
    if (parts.size() != m_ipEditors.size())
    {
        KLOG_INFO() << "The ip:" << ip << "is not complated!";
        return;
    }

    for (int i = 0; i < m_ipEditors.size(); i++)
    {
        m_ipEditors[i]->setText(parts[i]);
    }
}

void IpMaskLineEdit::setupUi()
{
    m_layout = new QHBoxLayout(this);
    m_layout->setSpacing(2);
    m_layout->setContentsMargins(0, 0, 0, 0);

    // 创建ip输入框
    for (int i = 0; i < 4; ++i)
    {
        auto editor = new QLineEdit(this);
        editor->setFixedWidth(40);
        editor->setValidator(new QRegExpValidator(QRegExp("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"), this));
        editor->setAlignment(Qt::AlignCenter);
        editor->installEventFilter(this);
        editor->setStyleSheet("border: none; background: transparent;");
        m_ipEditors.append(editor);
        m_layout->addWidget(editor);

        if (i < 3)
        {
            auto dotLabel = new QLabel(".", this);
            m_layout->addWidget(dotLabel);
        }
    }

    // 创建掩码输入框
    if (m_withMask)
    {
        auto slashLabel = new QLabel("/", this);
        m_layout->addWidget(slashLabel);

        m_maskEditor = new QLineEdit(this);
        m_maskEditor->setFixedWidth(100);
        m_maskEditor->setValidator(new QRegExpValidator(QRegExp("^([0-9]|[1-2][0-9]|3[0-2])$"), this));
        m_maskEditor->setAlignment(Qt::AlignLeft);
        m_maskEditor->installEventFilter(this);
        m_maskEditor->setStyleSheet("border: none; background: transparent;");
        m_layout->addWidget(m_maskEditor);
    }

    m_layout->addStretch();
}

void IpMaskLineEdit::moveCursorToNext(QLineEdit *editor)
{
    int index = m_ipEditors.indexOf(editor);
    // editor为掩码输入框时返回
    if (index < 0)
        return;

    QLineEdit *newEditor = nullptr;

    if (index == m_ipEditors.size() - 1 && m_maskEditor)
    {
        newEditor = m_maskEditor;
    }
    else if (index < m_ipEditors.size() - 1)
    {
        newEditor = m_ipEditors[index + 1];
    }

    if (newEditor)
    {
        newEditor->setFocus();
        newEditor->setCursorPosition(0);
    }
}

void IpMaskLineEdit::jumpToNext(QLineEdit *editor)
{
    int index = m_ipEditors.indexOf(editor);
    // editor为掩码输入框时返回
    if (index < 0)
        return;

    QLineEdit *newEditor = nullptr;

    if (index == m_ipEditors.size() - 1 && m_maskEditor)
    {
        newEditor = m_maskEditor;
    }
    else if (index < m_ipEditors.size() - 1)
    {
        newEditor = m_ipEditors[index + 1];
    }

    if (newEditor)
    {
        newEditor->setFocus();
        newEditor->setCursorPosition(newEditor->text().length());
    }
}

void IpMaskLineEdit::jumpToPre(QLineEdit *editor)
{
    QLineEdit *newEditor = nullptr;
    if (m_maskEditor && editor == m_maskEditor)
    {
        newEditor = m_ipEditors[m_ipEditors.size() - 1];
    }
    else
    {
        int index = m_ipEditors.indexOf(editor);
        if (index - 1 >= 0)
        {
            newEditor = m_ipEditors[index - 1];
        }
    }
    if (newEditor)
    {
        newEditor->setFocus();
        newEditor->setCursorPosition(newEditor->text().length());
    }
}

QString IpMaskLineEdit::getIpAddress() const
{
    QStringList strParts;
    for (QLineEdit *editor : m_ipEditors)
    {
        if (editor->text().isEmpty())
            strParts << "-";
        else
            strParts << QString::number(editor->text().toInt());  // 将前置0如001转化为1
    }

    return strParts.join(".");
}

QString IpMaskLineEdit::getMask() const
{
    if (m_maskEditor)
    {
        return m_maskEditor->text();
    }
    return "";
}

QString IpMaskLineEdit::getIpMask() const
{
    if (m_maskEditor)
    {
        return getIpAddress() + "/" + getMask();
    }
    else
        return "";
}

bool IpMaskLineEdit::isValidIpMask(QString ipMask)
{
    if (ipMask.isEmpty() || ipMask.contains("-") || !ipMask.contains("/"))
        return false;

    QStringList parts = ipMask.split("/");
    if (parts.size() != 2)
    {
        return false;
    }

    QString ipStr = parts[0];
    int maskLength = parts[1].toInt();

    if (maskLength < 0 || maskLength > 32)
        return false;

    quint32 ipInt = ip_to_int(ipStr);
    quint32 mask = mask_length_to_mask(maskLength);

    // 掩码外部分（主机位）
    quint32 hostPart = ipInt & ~mask;

    // /31 特殊处理：允许主机位为 0 或 1
    if (maskLength == 31)
    {
        return true;  // 允许两个地址都作为主机地址使用
    }

    // 主机位不能全为0（网络地址）或全为1（广播地址）
    if (hostPart == 0 || hostPart == ~mask)
    {
        return false;
    }

    return true;
}

bool IpMaskLineEdit::isValidIp(QString ip)
{
    if (ip.isEmpty())
        return false;

    if (ip.contains("-"))
        return false;

    return true;
}

// 将IP字符串转换为32位整数
quint32 IpMaskLineEdit::ip_to_int(const QString &ip)
{
    QStringList parts = ip.split(".");
    if (parts.size() != 4)
        return 0;

    quint32 ipInt = 0;
    for (int i = 0; i < 4; ++i)
    {
        bool ok;
        int part = parts[i].toInt(&ok);
        ipInt = (ipInt << 8) + static_cast<quint8>(part);
    }
    return ipInt;
}

// 根据掩码长度生成32位掩码
quint32 IpMaskLineEdit::mask_length_to_mask(int maskLength)
{
    if (maskLength == 0)
        return 0;

    return (~((1 << (32 - maskLength)) - 1));
}

void IpMaskLineEdit::clear()
{
    for (auto editor : m_ipEditors)
    {
        editor->clear();
    }
    if (m_maskEditor)
    {
        m_maskEditor->clear();
    }
}

void IpMaskLineEdit::setMaksPlaceholderText(const QString &text)
{
    if (m_maskEditor)
    {
        m_maskEditor->setPlaceholderText(text);
    }
}

bool IpMaskLineEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() != QEvent::KeyPress)
        return QObject::eventFilter(obj, event);

    auto editor = qobject_cast<QLineEdit *>(obj);
    if (!editor)
        return QObject::eventFilter(obj, event);

    if (!(m_ipEditors.contains(editor) || (m_maskEditor && m_maskEditor == obj)))
        return QObject::eventFilter(obj, event);

    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    // 输入数字或者“.”,跳转到下一个
    if ((keyEvent->text().size() == 1 && keyEvent->text().at(0).isDigit()) || keyEvent->text() == ".")
    {
        int pos = 0;

        auto newText = editor->text().insert(editor->cursorPosition(), keyEvent->text());
        if (editor->text().length() >= 3 || (editor->text().length() < 3 && keyEvent->text() == ".") || editor->validator()->validate(newText, pos) == QValidator::Invalid)
        {
            jumpToNext(editor);
            return true;
        }
    }
    // 在最后一个ip段输入“/”,跳转到下一个
    else if (keyEvent->text() == "/" && m_ipEditors.last() == editor)
    {
        jumpToNext(editor);
        return true;
    }

    // 输入删除按钮跳转至上一个
    if (keyEvent->key() == Qt::Key_Backspace)
    {
        if (editor->text().length() == 0 || editor->cursorPosition() == 0)
        {
            jumpToPre(editor);
            return true;
        }
    }
    else if (keyEvent->key() == Qt::Key_Left)
    {
        if (editor->cursorPosition() == 0)
        {
            jumpToPre(editor);
            return true;
        }
    }
    else if (keyEvent->key() == Qt::Key_Right)
    {
        if (editor->cursorPosition() == editor->text().length())
        {
            moveCursorToNext(editor);
            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}

void IpMaskLineEdit::focusInEvent(QFocusEvent *event)
{
    m_ipEditors[0]->setFocus();
    QLineEdit::focusInEvent(event);
}
