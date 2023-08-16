#ifndef TRANSMISSIONLIST_H
#define TRANSMISSIONLIST_H
#include <QListWidget>
#include <QStackedWidget>
#include <QWidget>
#include "def.h"
class TransmissionItem;
class TransmissionList : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int transfersNum READ getTransfersNum WRITE setTransfersNum NOTIFY transfersNumChanged)
public:
    explicit TransmissionList(QWidget* parent = nullptr);
    ~TransmissionList();
    void addItem(QString name, QString version, ImageTransmissionStatus status, int rate);
    void updateItem(QString name, QString version, ImageTransmissionStatus status, int rate);
    int getTransfersNum();

private:
    void initUI();
    void adjustSize();

public slots:
    void setTransfersNum(int num);
    void deleteItem();

signals:
    void transfersNumChanged(int num);
    void transferItemDeleted(QString name, QString version, ImageTransmissionStatus status);

private:
    QListWidget* m_listWidget;
    QStackedWidget* m_stackedWidget;
    int m_transfersNum;
    QList<TransmissionItem*> m_transfersItems;
};

#endif  // TRANSMISSIONLIST_H
