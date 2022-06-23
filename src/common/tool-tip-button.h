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
