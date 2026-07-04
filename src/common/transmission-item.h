#ifndef TRANSMISSIONITEM_H
#define TRANSMISSIONITEM_H

#include <QWidget>
#include "def.h"
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
    void setStatus(ImageTransmissionStatus status);
    void setRate(int rate);
    QString name();
    QString version();
    ImageTransmissionStatus status();

protected:
    void paintEvent(QPaintEvent *event) override;

public slots:
    void onItemClose();

signals:
    void sigClose();

private:
    Ui::TransmissionItem *ui;
    ImageTransmissionStatus m_status;
};

#endif  // TRANSMISSIONITEM_H
