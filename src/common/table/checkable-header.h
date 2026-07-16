#ifndef CHECKABLEHEADER_H
#define CHECKABLEHEADER_H

#include <QCheckBox>
#include <QHeaderView>

class CheckableHeader : public QHeaderView
{
    Q_OBJECT

public:
    explicit CheckableHeader(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void paintSection(QPainter *painter,
                      const QRect &rect,
                      int logicalIndex) const override;

signals:
    void toggled(Qt::CheckState checkState);

public slots:
    void setCheckState(Qt::CheckState checkState);

private:
    bool m_stateChanged;
    Qt::CheckState m_checkState;
    QRect *m_rect;
};

#endif  // CHECKABLEHEADER_H
