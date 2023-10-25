/**
 * @file          /ks-scmc-gui/src/common/tool-tip-button.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef TOOLTIPBUTTON_H
#define TOOLTIPBUTTON_H

#include <QToolButton>

class ToolTipButton : public QToolButton
{
    Q_OBJECT
public:
    explicit ToolTipButton(QWidget *parent = nullptr);
};

#endif  // TOOLTIPBUTTON_H
