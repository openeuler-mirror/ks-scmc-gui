#include "user-table.h"
#include <kiran-log/qt5-log-i.h>
#include <QApplication>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QStandardItemModel>
#include <QTableView>
#include <QToolTip>
#include "message-dialog.h"

enum UserTableField
{
    USER_TABLE_FIELD_CHECKBOX,
    USER_TABLE_FIELD_USER_NAME,
    USER_TABLE_FIELD_USER_ID,
    USER_TABLE_FIELD_ROLE,
    USER_TABLE_FIELD_CREATE_TIME,
    USER_TABLE_FIELD_OPERATE,
    USER_TABLE_FIELD_LAST
};

UserDelegate::UserDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

UserDelegate::~UserDelegate()
{
}

void UserDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 获取角色信息
    QModelIndex roleIndex = index.sibling(index.row(), UserTableField::USER_TABLE_FIELD_ROLE);
    auto role = roleIndex.data().toString();
    bool isAdmin = isSystemUser(role);

    const QWidget *widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();

    if (index.column() == UserTableField::USER_TABLE_FIELD_CHECKBOX)
    {
        // 如果是管理员，不显示复选框
        if (isAdmin)
        {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        auto checkboxOption = option;
        initStyleOption(&checkboxOption, index);

        QStyleOptionButton checkboxStyle;
        QPixmap pixmap;
        auto value = index.model()->data(index, Qt::EditRole).toBool();
        pixmap.load(value ? ":images/checked.svg" : ":images/check-default.svg");
        checkboxStyle.state = value ? QStyle::State_On : QStyle::State_Off;
        checkboxStyle.state |= QStyle::State_Enabled;
        checkboxStyle.iconSize = QSize(16, 16);
        checkboxStyle.rect = option.rect;
        checkboxStyle.rect.setX(option.rect.x() + 13);
        checkboxStyle.rect.setY(option.rect.x() + 12);

        const_cast<UserDelegate *>(this)->m_checkboxRects[index] = checkboxStyle.rect;

        style->drawItemPixmap(painter, option.rect, Qt::AlignCenter, pixmap);
        style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, widget);
    }
    else if (index.column() == UserTableField::USER_TABLE_FIELD_OPERATE)
    {
        // 如果是管理员，不显示按钮
        if (isAdmin)
        {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        // 绘制编辑和删除按钮
        QStyleOptionButton editButton, deleteButton;
        editButton.text = tr("Edit");
        deleteButton.text = tr("Delete");

        QPalette palette = editButton.palette;
        palette.setColor(QPalette::ButtonText, QColor(46, 179, 255));
        editButton.palette = palette;
        deleteButton.palette = palette;

        // 禁用边框绘制标志
        editButton.features = QStyleOptionButton::Flat;                    // 扁平化样式
        editButton.palette.setBrush(QPalette::Button, Qt::transparent);    // 透明背景
        deleteButton.features = QStyleOptionButton::Flat;                  // 扁平化样式
        deleteButton.palette.setBrush(QPalette::Button, Qt::transparent);  // 透明背景

        int buttonWidth = 50;
        int buttonHeight = option.rect.height() - 4;
        int buttonSpacing = 5;

        editButton.rect = QRect(option.rect.left() + buttonSpacing,
                                option.rect.top() + 2,
                                buttonWidth,
                                buttonHeight);

        deleteButton.rect = QRect(option.rect.left() + buttonWidth + buttonSpacing * 2,
                                  option.rect.top() + 2,
                                  buttonWidth,
                                  buttonHeight);

        // 保存按钮位置，用于点击检测
        const_cast<UserDelegate *>(this)->m_buttonRects[index] = qMakePair(editButton.rect, deleteButton.rect);

        style->drawControl(QStyle::CE_PushButton, &editButton, painter);
        style->drawControl(QStyle::CE_PushButton, &deleteButton, painter);
        style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, widget);
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

bool UserDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    auto docorationRect = option.rect;
    auto mouseEvent = static_cast<QMouseEvent *>(event);

    if (event->type() == QEvent::MouseButtonPress &&
        docorationRect.contains(mouseEvent->pos()) &&
        index.column() == UserTableField::USER_TABLE_FIELD_CHECKBOX &&
        m_checkboxRects.contains(index))
    {
        auto value = model->data(index, Qt::EditRole).toBool();
        model->setData(index, !value, Qt::EditRole);
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        if (m_buttonRects.contains(index))
        {
            QPair<QRect, QRect> buttonRects = m_buttonRects[index];

            if (buttonRects.first.contains(mouseEvent->pos()))
            {
                emit editClicked(index);
            }
            else if (buttonRects.second.contains(mouseEvent->pos()))
            {
                emit deleteClicked(index);
            }
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

bool UserDelegate::isSystemUser(const QString &role) const
{
    if (0 == QString::compare(role, USER_ROLE_SYSADM) ||
        0 == QString::compare(role, USER_ROLE_SECADM) ||
        0 == QString::compare(role, USER_ROLE_AUDADM))
    {
        return true;
    }
    return false;
}

UserFilterModel::UserFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

bool UserFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QString textComb;
    for (auto i = 0; i < USER_TABLE_FIELD_LAST; ++i)
    {
        auto index = sourceModel()->index(sourceRow, i, sourceParent);
        auto text = sourceModel()->data(index).toString();
        if (text.contains(filterRegExp()))
            return true;
    }

    return false;
}

UserModel::UserModel(QObject *parent)
{
}

int UserModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_usersInfo.size();
}

int UserModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return UserTableField::USER_TABLE_FIELD_LAST;
}

QVariant UserModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_usersInfo.size() || index.column() >= UserTableField::USER_TABLE_FIELD_LAST)
    {
        KLOG_DEBUG() << "The index exceeds range limit.";
        return QVariant();
    }

    auto userInfo = m_usersInfo[index.row()];

    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (index.column())
        {
        case UserTableField::USER_TABLE_FIELD_USER_NAME:
            return userInfo.userName;
        case UserTableField::USER_TABLE_FIELD_USER_ID:
            return userInfo.userID;
        case UserTableField::USER_TABLE_FIELD_ROLE:
            return userInfo.role;
        case UserTableField::USER_TABLE_FIELD_CREATE_TIME:
            return userInfo.createTime;
        default:
            break;
        }
    }
    case Qt::EditRole:
    {
        switch (index.column())
        {
        case UserTableField::USER_TABLE_FIELD_OPERATE:
            return QVariant();
        case UserTableField::USER_TABLE_FIELD_CHECKBOX:
        {
            if (isSystemUser(userInfo.role))
            {
                return QVariant();
            }
            return userInfo.selected;
        }
        default:
            break;
        }
    }
    default:
        break;
    }

    return QVariant();
}

QVariant UserModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Vertical)
    {
        return QVariant();
    }
    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (section)
        {
        case UserTableField::USER_TABLE_FIELD_USER_NAME:
            return tr("User Name");
        case UserTableField::USER_TABLE_FIELD_USER_ID:
            return tr("User ID");
        case UserTableField::USER_TABLE_FIELD_ROLE:
            return tr("Role");
        case UserTableField::USER_TABLE_FIELD_CREATE_TIME:
            return tr("Create Time");
        case UserTableField::USER_TABLE_FIELD_OPERATE:
            return tr("Quick Operate");
        default:
            break;
        }
    }
    case Qt::EditRole:
    {
        switch (section)
        {
        case UserTableField::USER_TABLE_FIELD_CHECKBOX:
            return QVariant();
        }
    }
    default:
        break;
    }
    return QVariant();
}

bool UserModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.column() != UserTableField::USER_TABLE_FIELD_CHECKBOX)
    {
        return false;
    }

    if (isSystemUser(m_usersInfo[index.row()].role))
    {
        // 系统管理员不能选中
        return false;
    }

    m_usersInfo[index.row()].selected = value.toBool();

    emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags UserModel::flags(const QModelIndex &index) const
{
    if (index.column() == UserTableField::USER_TABLE_FIELD_CHECKBOX)
    {
        if (!isSystemUser(m_usersInfo[index.row()].role))
            return Qt::ItemFlag::ItemIsEnabled;
    }
    return Qt::ItemFlag::NoItemFlags;
}

QList<UserInfo> UserModel::getUserInfos()
{
    return m_usersInfo;
}

void UserModel::addUser(const UserInfo &user)
{
    beginInsertRows(QModelIndex(), m_usersInfo.size(), m_usersInfo.size());
    m_usersInfo.append(user);
    endInsertRows();
}

void UserModel::removeUser(QModelIndex index)
{
    if (index.row() < 0 || index.row() >= m_usersInfo.size())
        return;

    beginRemoveRows(QModelIndex(), index.row(), index.row());
    auto userName = index.data().toString();
    for (auto it = m_usersInfo.begin(); it != m_usersInfo.end();)
    {
        KLOG_INFO() << "index user name:" << userName << "it user name:" << it->userName;
        if (it->userName == userName)
        {
            it = m_usersInfo.erase(it);
        }
        else
        {
            ++it;
        }
    }
    endRemoveRows();
}

void UserModel::checkSelectStatus()
{
    auto state = Qt::Unchecked;
    int selectCount = 0;
    for (int i = 0; i < m_usersInfo.size(); ++i)
    {
        if (m_usersInfo[i].selected)
        {
            ++selectCount;
        }
    }

    if (selectCount >= m_checkableRowNum)
    {
        state = Qt::Checked;
    }
    else if (selectCount > 0)
    {
        state = Qt::PartiallyChecked;
    }

    emit stateChanged(state);
}

void UserModel::updateRecord(QList<UserInfo> userInfos)
{
    m_checkableRowNum = 0;

    beginResetModel();

    m_usersInfo.clear();
    // 刷新时checkbox状态清空
    emit stateChanged(Qt::Unchecked);

    foreach (auto userInfo, userInfos)
    {
        if (!isSystemUser(userInfo.role))
        {
            m_checkableRowNum++;
        }
        m_usersInfo.push_back(userInfo);
    }
    emit usersUpdate(m_usersInfo.size());

    endResetModel();
}

bool UserModel::isSystemUser(const QString &role) const
{
    if (0 == QString::compare(role, USER_ROLE_SYSADM) ||
        0 == QString::compare(role, USER_ROLE_SECADM) ||
        0 == QString::compare(role, USER_ROLE_AUDADM))
    {
        return true;
    }
    return false;
}

UserTable::UserTable(QWidget *parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);

    // 设置Model
    m_model = new UserModel(this);
    m_headerViewProxy = new CheckableHeader(this);
    setHorizontalHeader(m_headerViewProxy);

    connect(m_model, &UserModel::stateChanged, m_headerViewProxy, &CheckableHeader::setCheckState);
    connect(m_model, &UserModel::usersUpdate, this, &UserTable::usersUpdate);
    connect(m_model, &UserModel::stateChanged, this, &UserTable::checkStateChanged);

    m_filterProxy = new UserFilterModel(this);
    m_filterProxy->setSourceModel(qobject_cast<QAbstractItemModel *>(m_model));
    setModel(m_filterProxy);

    // 设置Delegate
    auto userDelegate = new UserDelegate(this);
    setItemDelegate(userDelegate);
    connect(userDelegate, &UserDelegate::editClicked, this, &UserTable::onEditClicked);
    connect(userDelegate, &UserDelegate::deleteClicked, this, &UserTable::onDeleteClicke);

    // 设置水平行表头
    m_headerViewProxy->resizeSection(UserTableField::USER_TABLE_FIELD_CHECKBOX, 50);
    m_headerViewProxy->resizeSection(UserTableField::USER_TABLE_FIELD_USER_NAME, 400);
    m_headerViewProxy->resizeSection(UserTableField::USER_TABLE_FIELD_USER_ID, 200);
    m_headerViewProxy->resizeSection(UserTableField::USER_TABLE_FIELD_ROLE, 200);
    m_headerViewProxy->resizeSection(UserTableField::USER_TABLE_FIELD_CREATE_TIME, 400);
    m_headerViewProxy->resizeSection(UserTableField::USER_TABLE_FIELD_OPERATE, 200);
    m_headerViewProxy->setSectionResizeMode(UserTableField::USER_TABLE_FIELD_CHECKBOX, QHeaderView::Fixed);
    m_headerViewProxy->setSectionResizeMode(UserTableField::USER_TABLE_FIELD_OPERATE, QHeaderView::Fixed);
    m_headerViewProxy->setStretchLastSection(true);
    m_headerViewProxy->setSectionsMovable(false);
    m_headerViewProxy->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_headerViewProxy->setFixedHeight(40);
    connect(m_headerViewProxy, &CheckableHeader::toggled, this, &UserTable::checkedAllItem);

    // 设置表的其他属性
    setSelectionMode(QAbstractItemView::NoSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setFocusPolicy(Qt::NoFocus);
    setShowGrid(false);
    setColumnWidth(0, 50);

    // 隐藏列表头
    this->verticalHeader()->setVisible(false);
    this->verticalHeader()->setDefaultSectionSize(60);

    connect(this, &UserTable::entered, this, &UserTable::mouseEnter);
}

void UserTable::searchTextChanged(const QString &text)
{
    qDebug() << "The search text is change to " << text;

    m_filterProxy->setFilterFixedString(text);
}

void UserTable::setUserInfos(QList<UserInfo> userInfos)
{
    m_model->updateRecord(userInfos);
}

QList<UserInfo> UserTable::getUserInfos()
{
    return m_model->getUserInfos();
}

QList<UserInfo> UserTable::getSelectedUserInfos()
{
    QList<UserInfo> selectedUserInfos;
    for (int i = 0; i < m_model->rowCount(); i++)
    {
        auto isSelected = m_model->data(m_model->index(i, UserTableField::USER_TABLE_FIELD_CHECKBOX)).toBool();
        if (isSelected)
        {
            selectedUserInfos.append(m_model->getUserInfos().at(i));
        }
    }
    return selectedUserInfos;
}

void UserTable::removeUsers(QList<UserInfo> userInfos)
{
    for (auto userInfo : userInfos)
    {
        for (int i = 0; i < m_model->rowCount(); i++)
        {
            auto index = m_model->index(i, UserTableField::USER_TABLE_FIELD_USER_ID);
            if (m_model->data(index).toLongLong() == userInfo.userID)
            {
                m_model->removeUser(index);
                break;
            }
        }
    }
}

void UserTable::clearTable()
{
    m_model->removeRows(0, m_model->rowCount());
}

void UserTable::mouseEnter(const QModelIndex &index)
{
    if (index.column() != UserTableField::USER_TABLE_FIELD_USER_NAME)
    {
        return;
    }
    auto mod = selectionModel()->model()->data(index);
    QToolTip::showText(QCursor::pos(), mod.toString(), this, rect(), 2000);
}

void UserTable::checkedAllItem(Qt::CheckState checkState)
{
    for (int i = 0; i < selectionModel()->model()->rowCount(); i++)
    {
        auto index = m_model->index(i, UserTableField::USER_TABLE_FIELD_CHECKBOX);
        if (index.data().toString() == USER_ROLE_SYSADM ||
            index.data().toString() == USER_ROLE_SECADM ||
            index.data().toString() == USER_ROLE_AUDADM)
        {
            // 系统管理员不能选中
            continue;
        }

        m_model->setData(index, checkState == Qt::Checked, Qt::EditRole);
    }
}

void UserTable::onEditClicked(const QModelIndex &index)
{
    auto userInfo = m_model->getUserInfos()[index.row()];
    emit userEdited(userInfo.userID, userInfo.userName, userInfo.roleID);
}

void UserTable::onDeleteClicke(const QModelIndex &index)
{
    int row = index.row();
    auto username = m_model->data(m_model->index(row, UserTableField::USER_TABLE_FIELD_USER_NAME)).toString();
    auto userID = m_model->data(m_model->index(row, UserTableField::USER_TABLE_FIELD_USER_ID)).toLongLong();

    MessageDialog::StandardButton ret = MessageDialog::message(tr("Remove User"),
                                                               tr("Are you sure you want to remove \n%1 ?").arg(username),
                                                               tr("It can't be recovered after deletion.Are you sure you want to continue?"),
                                                               ":/images/warning.svg",
                                                               MessageDialog::StandardButton::Yes | MessageDialog::StandardButton::Cancel);
    if (ret == MessageDialog::StandardButton::Yes)
    {
        emit userDeleted(userID);
    }
}
