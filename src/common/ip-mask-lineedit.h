/**
 * @file          src/common/ip-mask-lineedit.h
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#ifndef IP_MASK_LINEEDIT_H
#define IP_MASK_LINEEDIT_H

#include <QHBoxLayout>
#include <QIntValidator>
#include <QLineEdit>
#include <QWidget>

class IpMaskLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit IpMaskLineEdit(QWidget *parent = nullptr, bool withMask = false);

    void setHasMask(bool withMask);

    void setIpMask(QString ipMask);
    void setIp(QString ip);
    QString getIpAddress() const;
    QString getMask() const;
    QString getIpMask() const;

    bool isValidIpMask(QString ipMask);
    bool isValidIp(QString ip);

    void clear();

    void setMaksPlaceholderText(const QString &text);

signals:
    void
    textChanged(const QString &text);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

private:
    void setupUi();
    void moveCursorToNext(QLineEdit *editor);
    void jumpToNext(QLineEdit *editor);
    void jumpToPre(QLineEdit *editor);
    quint32 ip_to_int(const QString &ip);
    quint32 mask_length_to_mask(int maskLength);

private:
    bool m_withMask;
    QList<QLineEdit *> m_ipEditors;
    QLineEdit *m_maskEditor;
    QHBoxLayout *m_layout;
    int m_currentIndex = 0;
};

#endif  // IP_MASK_LINEEDIT_H
