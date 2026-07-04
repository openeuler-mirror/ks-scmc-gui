#ifndef ACTIVATE_PAGE
#define ACTIVATE_PAGE

#include <QWidget>
#include <QLabel>
#include <QLineEdit>

#include <qrencode.h>
#include <kiranwidgets-qt5/kiran-style-public-define.h>
#include <kiranwidgets-qt5/kiran-switch-button.h>
#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <kiranwidgets-qt5/widget-property-helper.h>
#include <kiranwidgets-qt5/kiran-message-box.h>

class ActivatePage : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit ActivatePage(KiranTitlebarWindow *parent = 0);
    ~ActivatePage();
    void setText(QString machine_code, QString activation_code, int activation_time, int expired_time);
    void showAcitvedBox();
    void showErrorBox();

signals:
    void activate_app(QString activation_code);

private slots:
    void activation();
    void cancel();
    void showQR();
    void hideQR();

private:
    void init();
    void createQRcode(QString machine_code);
    void initMessageBox();

private:
    QLabel *date_code_label;
    QLabel *machine_code_label;
    QLabel *license_code_label;
    QLineEdit *date_code_edit;
    QLineEdit *machine_code_edit;
    QLineEdit *license_code_edit;

    KiranTitlebarWindow *qr;
    KiranMessageBox *m_errorBox;
    KiranMessageBox *m_messageBox;
    KiranMessageBox *m_acitvedMessageBox;
    QRcode *m_qrcode;
};

#endif // KSGAUTH_H
