/**
 * @file          /ks-scmc-gui/src/common/button-delegate.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "button-delegate.h"
#include <kiran-log/qt5-log-i.h>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QToolTip>
#define BUTTON_WIDTH 16
#define BUTTON_HEIGHT 16
#define BUTTON_SPACE 20
#define BUTTON_TOP 22

#define TEXT_WIDTH 50
#define TEXT_HEIGHT 16
#define TEXT_SPACE 10
#define TEXT_TOP 22

ButtonDelegate::ButtonDelegate(QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>> btnInfo, QObject *parent)
    : QStyledItemDelegate(parent),
      m_btnInfo(btnInfo),
      m_menu(nullptr)
{
    m_isSetDelegateDefault = false;
    if (m_btnInfo.contains(ACTION_BUTTON_TYPE_MENU))
    {
        m_menu = new QMenu();
        QFont font = m_menu->font();
        font.setPixelSize(12);
        m_menu->setFont(font);

        m_menu->setObjectName("moreInTableMenu");
        m_menu->addAction(tr("Run"));
        m_menu->addAction(tr("Stop"));
        m_menu->addAction(tr("Restart"));
        m_menu->addAction(tr("Generate template"));
        connect(m_menu, &QMenu::triggered, this, &ButtonDelegate::onActTriggered);
    }
}

ButtonDelegate::~ButtonDelegate()
{
    if (m_menu)
    {
        delete m_menu;
        m_menu = nullptr;
    }
}

void ButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem viewOption(option);
    initStyleOption(&viewOption, index);
    if (option.state.testFlag(QStyle::State_HasFocus))
        viewOption.state = viewOption.state ^ QStyle::State_HasFocus;

    QStyledItemDelegate::paint(painter, viewOption, index);
    int count = 0;
    if (!m_isSetDelegateDefault)
    {
        QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>>::const_iterator i = m_btnInfo.constBegin();
        while (i != m_btnInfo.constEnd())
        {
            // 绘制按钮
            QStyleOptionButton button;
            QRect btnRect = QRect(option.rect.x() + BUTTON_SPACE + count * BUTTON_WIDTH + count * BUTTON_SPACE, option.rect.y() + BUTTON_TOP, BUTTON_WIDTH, BUTTON_HEIGHT);
            QRect btnTextRect = QRect(option.rect.x() + TEXT_SPACE + TEXT_SPACE * count + count * TEXT_WIDTH, option.rect.y() + TEXT_TOP, TEXT_WIDTH, TEXT_HEIGHT);

            button.state |= QStyle::State_Enabled;
            if (btnRect.contains(m_mousePoint))
            {
                if (m_nType == 0)
                {
                    button.state |= QStyle::State_MouseOver;
                }
                else if (m_nType == 1)
                {
                    button.state |= QStyle::State_Sunken;
                }
            }
            if (i.value().second == tr("Pass") ||
                i.value().second == tr("Resume") ||
                i.value().second == tr("Update") ||
                i.value().second == tr("Remove") ||
                i.value().second == tr("Readed") ||
                i.value().second == tr("Export") ||
                i.value().second == tr("Run") ||
                i.value().second == tr("Stop") ||
                i.value().second == tr("Edit") ||
                i.value().second == tr("Delete"))
            {
                painter->setPen(QColor(46, 179, 255));
                QPalette *pal = new QPalette;
                pal->setColor(QPalette::ButtonText, QColor(46, 179, 255));
                QApplication::style()->drawItemText(painter, btnTextRect, Qt::AlignHCenter | Qt::AlignVCenter, *pal, true, i.value().first);
            }
            else if (i.value().second == tr("Refuse") ||
                     i.value().second == tr("Ignore"))
            {
                painter->setPen(QColor(211, 0, 0));
                QPalette *pal = new QPalette;
                pal->setColor(QPalette::ButtonText, QColor(211, 0, 0));
                QApplication::style()->drawItemText(painter, btnTextRect, Qt::AlignHCenter | Qt::AlignVCenter, *pal, true, i.value().first);
            }
            else
            {
                QPixmap pixBtn;
                pixBtn.load(i.value().second);
                QApplication::style()->drawItemPixmap(painter, btnRect, Qt::AlignHCenter | Qt::AlignVCenter, pixBtn);
                //QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter, pWidget);
            }
            ++i;
            count++;
            //            m_isSetDelegateDefault = true;
        }
    }
    else
    {
        //        painter->setPen(QColor(211, 0, 0));
        QPalette *pal = new QPalette;
        pal->setColor(QPalette::ButtonText, QColor(211, 0, 0));
        QRect btnRect = QRect(option.rect.x() + BUTTON_SPACE, option.rect.y() + BUTTON_TOP, BUTTON_WIDTH, BUTTON_HEIGHT);
        QApplication::style()->drawItemText(painter, btnRect, Qt::AlignHCenter | Qt::AlignVCenter, *pal, true, "-");
        //        m_isSetDelegateDefault = false;
    }
}

bool ButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(model);
    Q_UNUSED(index);
    QMouseEvent *pEvent = static_cast<QMouseEvent *>(event);
    m_mousePoint = pEvent->pos();
    QApplication::restoreOverrideCursor();

    bool repaint = false;
    int count = 0;
    if (!m_isSetDelegateDefault)
    {
        QMap<ACTION_BUTTON_TYPE, QPair<QString, QString>>::const_iterator i = m_btnInfo.constBegin();
        while (i != m_btnInfo.constEnd())
        {
            // 绘制按钮
            QStyleOptionButton button;
            QRect btnRect = QRect(option.rect.x() + BUTTON_SPACE + count * BUTTON_WIDTH + count * BUTTON_SPACE, option.rect.y() + BUTTON_TOP, BUTTON_WIDTH, BUTTON_HEIGHT);
            if (i.value().second == tr("Refuse") ||
                i.value().second == tr("Pass") ||
                i.value().second == tr("Resume") ||
                i.value().second == tr("Update") ||
                i.value().second == tr("Remove") ||
                i.value().second == tr("Readed") ||
                i.value().second == tr("Ignore") ||
                i.value().second == tr("Export") ||
                i.value().second == tr("Run") ||
                i.value().second == tr("Stop") ||
                i.value().second == tr("Edit") ||
                i.value().second == tr("Delete"))
                btnRect = QRect(option.rect.x() + TEXT_SPACE + TEXT_SPACE * count + count * TEXT_WIDTH, option.rect.y() + TEXT_TOP, TEXT_WIDTH, TEXT_HEIGHT);
            // 鼠标位于按钮之上
            if (!btnRect.contains(m_mousePoint))
            {
                i++;
                count++;
                continue;
            }
            repaint = true;
            switch (event->type())
            {
            // 鼠标滑过
            case QEvent::MouseMove:
            {
                // 设置鼠标样式为手型
                QApplication::setOverrideCursor(Qt::PointingHandCursor);

                QToolTip::showText(pEvent->globalPos(), i.value().first);
                QPalette palette;
                palette.setColor(QPalette::Inactive, QPalette::ToolTipBase, QColor("#ffffff"));
                palette.setColor(QPalette::Inactive, QPalette::ToolTipText, QColor("#000000"));
                QToolTip::setPalette(palette);
                m_nType = 0;
                break;
            }
            // 鼠标按下
            case QEvent::MouseButtonPress:
            {
                m_nType = 1;
                break;
            }
            // 鼠标释放
            case QEvent::MouseButtonRelease:
            {
                switch (i.key())
                {
                case ACTION_BUTTON_TYPE_APP:
                {
                    emit sigApp(index.row());
                    break;
                }
                case ACTION_BUTTON_TYPE_APP_RUN:
                {
                    emit sigAppRun(index.row());
                    break;
                }
                case ACTION_BUTTON_TYPE_APP_STOP:
                {
                    emit sigAppStop(index.row());
                    break;
                }
                case ACTION_BUTTON_TYPE_MONITOR:
                {
                    emit sigMonitor(index.row());
                    break;
                }
                case ACTION_BUTTON_TYPE_EDIT:
                {
                    emit sigEdit(index.row());
                    break;
                }
                case ACTION_BUTTON_TYPE_TERINAL:
                {
                    emit sigTerminal(index.row());
                    break;
                }
                case ACTION_BUTTON_TYPE_DELETE:
                {
                    emit sigdelete(index.row());
                    break;
                }
                case ACTION_BUTTON_TYPE_IMAGE_REFUSE:
                {
                    emit sigImageRefuse(index.row());
                    break;
                }
                case ACTION_BUTTON_TYPE_IMAGE_PASS:
                {
                    emit sigImagePass(index.row());
                    break;
                }
                case ACTION_BUTTON_TYPE_BACKUP_RESUME:
                {
                    emit sigBackupResume(index.row());
                    break;
                }
                case ACTION_BUTTON_TYPE_BACKUP_UPDATE:
                {
                    emit sigBackupUpdate(index.row());
                    break;
                }
                case ACTION_BUTTON_TYPE_BACKUP_REMOVE:
                {
                    emit sigBackupRemove(index.row());
                    break;
                }
                case ACTION_BUTTON_TYPE_BACKUP_EXPORT:
                {
                    emit sigBackupExport(index.row());
                    break;
                }
                case ACTION_BUTTON_TYPE_WARN_READ:
                {
                    emit sigWarnRead(index.row());
                    break;
                }
                case ACTION_BUTTON_TYPE_WARN_IGNORE:
                {
                    emit sigWarnIgnore(index.row());
                    break;
                }
                case ACTION_BUTTON_TYPE_MENU:
                {
                    if (m_menu)
                    {
                        m_index = index;
                        QPoint point(QCursor::pos().x() + 10, QCursor::pos().y());
                        m_menu->popup(point);
                    }
                    break;
                }
                default:
                    break;
                }
            }
            default:
                break;
            }
            count++;
            i++;
            //            m_isSetDelegateDefault = true;
        }
    }
    //    else
    //        m_isSetDelegateDefault = false;
    return repaint;
}

void ButtonDelegate::onActTriggered(QAction *act)
{
    if (act->text() == tr("Run"))
        emit sigActRun(m_index);
    else if (act->text() == tr("Stop"))
        emit sigActStop(m_index);
    else if (act->text() == tr("Restart"))
        emit sigActRestart(m_index);
    else if (act->text() == tr("Generate template"))
        emit sigActGenerateTemp(m_index);
}

void ButtonDelegate::isSetDelegateDefault(bool key)
{
    m_isSetDelegateDefault = key;
}
