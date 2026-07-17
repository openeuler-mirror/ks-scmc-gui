/**
 * @file          src/common/security-widgets/security-list.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#pragma once

#include <QListWidget>
#include <QVBoxLayout>
#include <QWidget>

class SecurityListItem;
class SecurityList : public QWidget
{
    Q_OBJECT
public:
    explicit SecurityList(QString itemTitle, QWidget *parent = nullptr);
    ~SecurityList();

    void setSecurityInfos(QStringList list);
    QStringList getSecurityInfos();

    void setItemTitle(QString title);
    void clearItems();

private:
    void initUI();
    void createItem(int index);
    void addItem();
    void deleteItem();
    void deleteOneItem(QListWidgetItem *listItem);
    void updateItem();

private:
    QListWidget *m_listWidget;
    QString m_itemTitle;
};