/**
 * @file          /ks-scmc-gui/src/common/tool-tip-button.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "tool-tip-button.h"
#include <QProxyStyle>

class ProxyStyle : public QProxyStyle
{
public:
    int styleHint(StyleHint hint,
                  const QStyleOption *option,
                  const QWidget *widget,
                  QStyleHintReturn *returnData) const
    {
        if (hint == QStyle::SH_ToolTip_WakeUpDelay)
            return 10;
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

ToolTipButton::ToolTipButton(QWidget *parent) : QToolButton(parent)
{
    auto proxyStyle = new ProxyStyle();
    proxyStyle->setParent(this);
    this->setStyle(proxyStyle);
}
