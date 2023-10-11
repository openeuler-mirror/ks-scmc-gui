#ifndef ERROR_MESSAGE_BOX_H
#define ERROR_MESSAGE_BOX_H

#include <QFont>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>

//#include "baseutils.h"

class ErrorMessageBox : public QWidget
{
    Q_OBJECT

public:
    explicit ErrorMessageBox(QWidget *parent = 0);
    ~ErrorMessageBox();

    void setLabelText(QString str);
    double getLabelTextLength();

protected:
    void paintEvent(QPaintEvent *);
    void leaveEvent(QEvent *event);

private:
    void calcTextSize();

private:
    qint32 text_max_width;  //文字的最大宽度
    qint32 text_min_width;  //文字的最小宽度
    qint32 text_height;     //文字的实际高度

    QString label_str;
};

#endif  // KSGERRORBOX_H
