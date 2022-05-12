#ifndef STARTSTOPCONTROLTAB_H
#define STARTSTOPCONTROLTAB_H

#include <QCheckBox>
#include <QWidget>
class StartStopControlTab : public QWidget
{
    Q_OBJECT
public:
    explicit StartStopControlTab(QWidget *parent = nullptr);
    void setStartStopInfo(bool isRestrict);
    bool getStartStopInfo();

private:
    void initUI();

signals:

private:
    QCheckBox *m_checkBox;
};

#endif  // STARTSTOPCONTROLTAB_H
