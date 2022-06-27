/**
 * @file          /ks-scmc-gui/src/common/about-page.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
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
