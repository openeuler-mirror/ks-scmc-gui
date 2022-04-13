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
