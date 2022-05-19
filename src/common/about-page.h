#ifndef ABOUTPAGE_H
#define ABOUTPAGE_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include "def.h"

class AboutPage : public KiranTitlebarWindow
{
    Q_OBJECT
public:
    AboutPage(QWidget *parent = nullptr);
    ~AboutPage();
private:
    void initUI();
};

#endif // ABOUTPAGE_H
