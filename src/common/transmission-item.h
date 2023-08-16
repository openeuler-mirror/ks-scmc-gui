#ifndef TRANSMISSIONITEM_H
#define TRANSMISSIONITEM_H

#include <QWidget>

namespace Ui
{
class TransmissionItem;
}

class TransmissionItem : public QWidget
{
    Q_OBJECT

public:
    explicit TransmissionItem(QWidget *parent = nullptr);
    ~TransmissionItem();
    void setName(QString name);
    void setVersion(QString version);
    void setStatus(QString status);
    void setRate(int rate);

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void sigClose();

private:
    Ui::TransmissionItem *ui;
};

#endif  // TRANSMISSIONITEM_H
