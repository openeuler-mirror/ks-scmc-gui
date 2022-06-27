/**
 * @file          /ks-scmc-gui/src/common/mask-widget.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef MASKWIDGET_H
#define MASKWIDGET_H

#include <QLabel>
#include <QObject>
class MaskWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MaskWidget(QWidget *parent = nullptr);
    void setMaskVisible(bool visible);
    bool maskIsVisible();
    ~MaskWidget();

private:
    void initUI();

signals:

public slots:

private:
    QLabel *m_labLoading;
};

#endif  // MASKWIDGET_H
