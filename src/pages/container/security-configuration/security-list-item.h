/**
 * @file          /ks-scmc-gui/src/pages/container/security-configuration/security-list-item.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef SECURITYLISTITEM_H
#define SECURITYLISTITEM_H

#include <QWidget>

namespace Ui
{
class SecurityListItem;
}

class SecurityListItem : public QWidget
{
    Q_OBJECT

public:
    explicit SecurityListItem(QString text, QWidget *parent = nullptr);
    ~SecurityListItem();
    void setDeleteBtnVisible(bool visible);
    void updateItemName(QString text);
    void setInfo(QString text);
    QString getInfo();
    bool getPathCorrect();

protected:
    void changeEvent(QEvent *event);

private:
    void checkPath();
    QString tooptipWordWrap(const QString &org);

signals:
    void sigAdd();
    void sigDelete();

private:
    Ui::SecurityListItem *ui;
    bool m_isPathCorrect;
    QTimer *m_timer;
};

#endif  // SECURITYLISTITEM_H
