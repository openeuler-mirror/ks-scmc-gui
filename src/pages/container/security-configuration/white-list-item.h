#ifndef WHITELISTITEM_H
#define WHITELISTITEM_H

#include <QWidget>

namespace Ui
{
class WhiteListItem;
}

class WhiteListItem : public QWidget
{
    Q_OBJECT

public:
    explicit WhiteListItem(int index, QWidget *parent = nullptr);
    ~WhiteListItem();
    void setDeleteBtnVisible(bool visible);
    void updateItemName(int index);
    void setProcessInfo();

signals:
    void sigAdd();
    void sigDelete();

private:
    Ui::WhiteListItem *ui;
};

#endif  // WHITELISTITEM_H
