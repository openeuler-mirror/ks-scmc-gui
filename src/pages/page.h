/**
 * @file          /ks-scmc-gui/src/pages/page.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
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
    ~Page();
    void setMaskParent(QWidget *parent);
    void setBusy(bool);
    bool isBusy();
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
