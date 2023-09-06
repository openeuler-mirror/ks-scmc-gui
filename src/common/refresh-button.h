#ifndef REFRESHBUTTON_H
#define REFRESHBUTTON_H

#include <QToolButton>

class RefreshButton : public QToolButton
{
    Q_OBJECT
public:
    RefreshButton(QWidget *parent = nullptr);

protected:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);

private:
    void refresh();
    void startTimer();

private:
    QTimer *m_timer;
};

#endif  // REFRESHBUTTON_H
