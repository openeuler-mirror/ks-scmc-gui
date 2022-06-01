#include "activate-page.h"

#include <qrencode.h>
#include <QHBoxLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>

#include <QDateTime>

ActivatePage::ActivatePage(KiranTitlebarWindow *parent) : KiranTitlebarWindow(parent)
{
    this->setWindowModality(Qt::ApplicationModal);
    init();
    initMessageBox();
}

ActivatePage::~ActivatePage()
{
}

void ActivatePage::init()
{
    setResizeable(false);
    KiranTitlebarWindow::setTitle(tr("Software Activation"));
    KiranTitlebarWindow::setButtonHints(KiranTitlebarWindow::TitlebarCloseButtonHint | KiranTitlebarWindow::TitlebarMinimizeButtonHint);
    KiranTitlebarWindow::setIcon(QIcon(":/images/logo.png"));
    //    KiranTitlebarWindow::setTitleBarHeight(30);
    KiranTitlebarWindow::setFixedSize(500, 340);
    KiranTitlebarWindow::setWindowModality(Qt::ApplicationModal);

    QWidget *windowContentWidget = getWindowContentWidget();

    QVBoxLayout *vlayout = new QVBoxLayout(windowContentWidget);
    vlayout->setContentsMargins(6, 6, 0, 20);

    date_code_label = new QLabel(windowContentWidget);
    date_code_edit = new QLineEdit(windowContentWidget);
    date_code_label->setText(tr("Warranty period"));
    date_code_edit->setTextMargins(0, 0, 0, 0);
    date_code_edit->setReadOnly(true);

    machine_code_label = new QLabel(windowContentWidget);
    machine_code_edit = new QLineEdit(windowContentWidget);
    machine_code_label->setText(tr("Machine Code"));

    QHBoxLayout *hlayout = new QHBoxLayout(machine_code_edit);
    QPushButton *button = new QPushButton(machine_code_edit);
    button->setFlat(true);
    // KS::WidgetPropertyHelper::setButtonType(button, KS::ButtonType::BUTTON_Default);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(showQR()));

    button->setStyleSheet("QPushButton{border-image: url(:/images/qrcode.png);}");
    button->setFixedSize(16, 16);
    machine_code_edit->setTextMargins(0, 0, button->width(), 0);
    hlayout->setContentsMargins(6, 6, 12, 6);
    hlayout->addStretch(1);
    hlayout->addWidget(button);
    machine_code_edit->setLayout(hlayout);
    machine_code_edit->setReadOnly(true);

    license_code_label = new QLabel(windowContentWidget);
    license_code_edit = new QLineEdit(windowContentWidget);
    license_code_label->setText(tr("Activation Code"));

    QWidget *widget = new QWidget();
    widget->setStyleSheet("QWidget{"
                          "font:NotoSansCJKsc-Regular;"
                          "font-size:14px;}");
    QHBoxLayout *buttonLayout = new QHBoxLayout(widget);
    QPushButton *activeButton = new QPushButton(tr("Active"));
    QPushButton *canCelButton = new QPushButton(tr("Cancel"));

    Kiran::WidgetPropertyHelper::setButtonType(activeButton, Kiran::ButtonType::BUTTON_Default);
    Kiran::WidgetPropertyHelper::setButtonType(canCelButton, Kiran::ButtonType::BUTTON_Normal);

    activeButton->setFixedHeight(36);
    canCelButton->setFixedHeight(40);

    buttonLayout->addWidget(new QLabel(""));
    buttonLayout->addWidget(activeButton);
    buttonLayout->addWidget(new QLabel(""));
    buttonLayout->addWidget(canCelButton);
    buttonLayout->addWidget(new QLabel(""));
    widget->setLayout(buttonLayout);

    vlayout->addWidget(date_code_label);
    vlayout->addWidget(date_code_edit);
    vlayout->addWidget(machine_code_label);
    vlayout->addWidget(machine_code_edit);
    vlayout->addWidget(license_code_label);
    vlayout->addWidget(license_code_edit);
    vlayout->addWidget(widget);

    connect(activeButton, SIGNAL(clicked(bool)), this, SLOT(activation()));
    connect(canCelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));
}

void ActivatePage::createQRcode(QString machine_code)
{
    qr = new KiranTitlebarWindow(this);
    qr->setButtonHints(KiranTitlebarWindow::TitlebarCloseButtonHint);
    qr->setIcon(QIcon(":/images/logo.png"));
    //    qr->setTitleBarHeight(30);
    qr->setFixedSize(350, 300);
    qr->setWindowModality(Qt::ApplicationModal);
    qr->setTitle(tr("QR Code"));

    QWidget *widget = qr->getWindowContentWidget();
    QVBoxLayout *vlayout = new QVBoxLayout(widget);
    QLabel *label = new QLabel(widget);
    label->setAlignment(Qt::AlignCenter);
//    label->setMinimumSize(160,160);
    label->setContentsMargins(0,0,0,0);

    QLabel *textLabel = new QLabel(widget);
    textLabel->setAlignment(Qt::AlignCenter);
    textLabel->setText(tr("Scan QR code to get machine code"));

    vlayout->addWidget(label);
    vlayout->addWidget(textLabel);
    widget->setLayout(vlayout);

    int width = 160;
    int height = 160;

    QPixmap QRPixmap;
    m_qrcode = QRcode_encodeString(machine_code.toStdString().c_str(), 2, QR_ECLEVEL_Q, QR_MODE_8, 1);
    int qrcodeWidth = m_qrcode->width > 0 ? m_qrcode->width : 1;
    double scaledWidgth = (double)width / (double)qrcodeWidth;
    double scaledHeight = (double)height / (double)qrcodeWidth;

    QImage image = QImage(width, height, QImage::Format_ARGB32);

    QPainter painter(&image);
    QColor background(Qt::white);
    painter.setBrush(background);
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, 0, width, height);
    QColor foreground(Qt::black);
    painter.setBrush(foreground);

    for (qint32 y = 0; y < qrcodeWidth; y++)
    {
        for (qint32 x = 0; x < qrcodeWidth; x++)
        {
            unsigned char b = m_qrcode->data[y * qrcodeWidth + x];
            if (b & 0x01)
            {
                QRectF r(x * scaledWidgth, y * scaledHeight, scaledWidgth, scaledHeight);
                painter.drawRects(&r, 1);
            }
        }
    }

    QRPixmap = QPixmap::fromImage(image);
    label->setPixmap(QRPixmap);
}

void ActivatePage::initMessageBox()
{
    m_messageBox = new KiranMessageBox(this);
    QPushButton *button = new QPushButton(tr("OK"));
    button->setStyleSheet("QPushButton{"
                          "color:black;"
                          "font:NotoSansCJKsc-Regular;"
                          "font-size:14px;"
                          "border-radius:4px;"
                          "background:rgba(255,255,255,255);}");
    button->setFocusPolicy(Qt::NoFocus);
    m_messageBox->addButton(button, QDialogButtonBox::AcceptRole);
    m_messageBox->setButtonSize(QSize(80, 30));
    m_messageBox->setText(tr("Please enter the activation code."));

    m_errorBox = new KiranMessageBox(this);
    QPushButton *bt = new QPushButton(tr("OK"));
    bt->setStyleSheet("QPushButton{"
                      "color:black;"
                      "font:NotoSansCJKsc-Regular;"
                      "font-size:14px;"
                      "border-radius:4px;"
                      "background:rgba(255,255,255,255);}");
    bt->setFocusPolicy(Qt::NoFocus);
    m_errorBox->addButton(bt, QDialogButtonBox::AcceptRole);
    m_errorBox->setButtonSize(QSize(80, 30));
    m_errorBox->setText(tr("Activation failed. Please confirm whether the activation code is correct."));

    m_acitvedMessageBox = new KiranMessageBox(this);
    QPushButton *okButton = new QPushButton(tr("OK"));
    okButton->setStyleSheet("QPushButton{"
                            "color:black;"
                            "font:NotoSansCJKsc-Regular;"
                            "font-size:14px;"
                            "border-radius:4px;"
                            "background:rgba(255,255,255,255);}");
    okButton->setFocusPolicy(Qt::NoFocus);
    m_acitvedMessageBox->addButton(okButton, QDialogButtonBox::AcceptRole);
    m_acitvedMessageBox->setButtonSize(QSize(80, 30));
    m_acitvedMessageBox->setText(tr("Activation succeeded!"));

    connect(okButton, SIGNAL(clicked(bool)), this, SLOT(close()));
}

void ActivatePage::activation()
{
    QString activation_code = license_code_edit->text();
    if (activation_code.isEmpty())
    {
        int x = this->x() + this->width() / 2 - m_messageBox->width() / 2;
        int y = this->y() + this->height() / 2 - m_messageBox->height() / 2;
        m_messageBox->move(x, y);
        m_messageBox->show();
    }
    else
    {
        emit this->activate_app(activation_code);
    }
}

void ActivatePage::cancel()
{
    this->close();
}

void ActivatePage::showQR()
{
    int x = this->x() + this->width() / 2 - qr->width() / 2;
    int y = this->y() + this->height() / 2 - qr->height() / 2;
    qr->move(x, y);
    qr->show();
}

void ActivatePage::hideQR()
{
    qr->hide();
}

void ActivatePage::setText(QString machine_code, QString activation_code, int activation_time, uint expired_time)
{
    if (!machine_code.isNull())
        createQRcode(machine_code);
    if (activation_code.isEmpty())
    {
        date_code_edit->hide();
        date_code_label->hide();
        this->setFixedSize(500, 280);
    }
    else
    {
        date_code_edit->show();
        date_code_label->show();
        this->setFixedSize(500, 340);
    }
    date_code_edit->setText(QDateTime::fromTime_t(expired_time).toString("yyyy-MM-dd"));
    machine_code_edit->setText(machine_code);
    license_code_edit->setText(activation_code);
}

void ActivatePage::showAcitvedBox()
{
    int x = this->x() + this->width() / 2 - m_acitvedMessageBox->width() / 2;
    int y = this->y() + this->height() / 2 - m_acitvedMessageBox->height() / 2;
    m_acitvedMessageBox->move(x, y);
    m_acitvedMessageBox->show();
    update();
}

void ActivatePage::showErrorBox()
{
    int x = this->x() + this->width() / 2 - m_errorBox->width() / 2;
    int y = this->y() + this->height() / 2 - m_errorBox->height() / 2;
    m_errorBox->move(x, y);
    m_errorBox->show();
}
