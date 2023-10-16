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

private:
    void checkPath();
signals:
    void sigAdd();
    void sigDelete();

private:
    Ui::SecurityListItem *ui;
    bool m_isPathCorrect;
    QTimer *m_timer;
};

#endif  // SECURITYLISTITEM_H
