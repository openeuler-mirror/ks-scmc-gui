#ifndef TRANSMISSIONLIST_H
#define TRANSMISSIONLIST_H
#include <QListWidget>
#include <QStackedWidget>
#include <QWidget>
class TransmissionList : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int transfersNum READ getTransfersNum WRITE setTransfersNum NOTIFY transfersNumChanged)
public:
    explicit TransmissionList(QWidget* parent = nullptr);
    ~TransmissionList();
    void addItem(QString name, QString version, QString status, int rate);
    int getTransfersNum();

private:
    void initUI();

public slots:
    void setTransfersNum(int num);

signals:
    void transfersNumChanged(int num);

private:
    QListWidget* m_listWidget;
    QStackedWidget* m_stackedWidget;
    int m_transfersNum;
};

#endif  // TRANSMISSIONLIST_H
