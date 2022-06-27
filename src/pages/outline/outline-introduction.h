/**
 * @file          /ks-scmc-gui/src/pages/outline/outline-introduction.h
 * @brief         
 * @author        chendingjian <chendingjian@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef OUTLINEINTRODUCTION_H
#define OUTLINEINTRODUCTION_H

#include <QWidget>

namespace Ui {
class OutlineIntroduction;
}

class OutlineIntroduction : public QWidget
{
    Q_OBJECT

public:
    explicit OutlineIntroduction(QWidget *parent = nullptr);
    ~OutlineIntroduction();

private:
    Ui::OutlineIntroduction *ui;
};

#endif // OUTLINEINTRODUCTION_H
