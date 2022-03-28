#ifndef BUBBLETIPBUTTON_H
#define BUBBLETIPBUTTON_H

#include <QObject>
#include <QPushButton>

class BubbleTipButton : public QPushButton
{
    Q_OBJECT
public:
    explicit BubbleTipButton(QWidget *parent = nullptr);
    ~BubbleTipButton();

    void setTipMsg(int num);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    int m_tipMsg;
    int m_bubbleWidth;
};

#endif  // BUBBLETIPBUTTON_H
