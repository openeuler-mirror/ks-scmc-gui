#ifndef NETWORKACCESSITEM_H
#define NETWORKACCESSITEM_H

#include <QWidget>

namespace Ui
{
class NetworkAccessItem;
}

class NetworkAccessItem : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkAccessItem(QWidget *parent = nullptr);
    ~NetworkAccessItem();
    void setDeleteBtnVisible(bool visible);
    void setNetworkInfo(int index);

private slots:
    void ondelete();
    void onAdd();

signals:
    void sigDelete();
    void sigAdd();

private:
    void initUI();

private:
    Ui::NetworkAccessItem *ui;
};

#endif  // NETWORKACCESSITEM_H
