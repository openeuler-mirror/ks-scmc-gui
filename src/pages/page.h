#ifndef PAGE_H
#define PAGE_H

#include <QVariant>
#include <QWidget>

class MaskWidget;
class Page : public QWidget
{
    Q_OBJECT
public:
    explicit Page(QWidget *parent = nullptr);
    ~Page() = default;
    void setBusy(bool);
    void setData(QVariant data);
    QVariant getData();
    virtual void updateInfo(QString keyword = "") = 0;

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    MaskWidget *m_maskWidget;
    QVariant m_data;
};

#endif  // PAGE_H
