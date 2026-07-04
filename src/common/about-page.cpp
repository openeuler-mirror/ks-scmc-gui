/**
 * @file          /ks-scmc-gui/src/common/about-page.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "about-page.h"
#include <QFile>
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>

AboutPage::AboutPage(QWidget *parent) : KiranTitlebarWindow(parent)
{
    initUI();
}

AboutPage::~AboutPage()
{
}

static QString getVersion(QString filepath)
{
    QFile file(filepath);
    QString ret;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray ba = file.readLine();
        ret = QString(ba);

        file.close();
    }

    return ret.simplified();
}

void AboutPage::initUI()
{
    this->setButtonHints(KiranTitlebarWindow::TitlebarCloseButtonHint);
    this->setIcon(QIcon(":/images/logo.png"));
    //    this->setTitleBarHeight(30);
    this->getTitlebarCustomLayout()->setContentsMargins(0, 0, 0, 0);
    this->setFixedSize(430, 270);
    setResizeable(false);
    this->setWindowModality(Qt::ApplicationModal);

    QWidget *aboutWidget = this->getWindowContentWidget();
    aboutWidget->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *vlayout = new QVBoxLayout(aboutWidget);
    vlayout->setContentsMargins(0, 0, 0, 0);

    QSpacerItem *spacer = new QSpacerItem(aboutWidget->width(), 20);
    QSpacerItem *endSpacer = new QSpacerItem(aboutWidget->width(), 40);

    QLabel *logo = new QLabel(aboutWidget);
    logo->setPixmap(QPixmap(":/images/kylin-logo.png"));
    logo->setAlignment(Qt::AlignCenter);
    logo->setMinimumSize(172, 52);

    QLabel *version = new QLabel(aboutWidget);
    version->setStyleSheet("QLabel{"
                           "color:#ffffff;"
                           "font-size:12px;}");
    version->setText(tr("KylinSec Container System V1(Security Version)"));
    version->setAlignment(Qt::AlignCenter);

    QLabel *info = new QLabel(aboutWidget);
    info->setStyleSheet("QLabel{"
                        "color:#ffffff;"
                        "font-size:12px;}");
    QString ksScmcGuiVersion = getVersion(SCMC_GUI_VERSION_FILE_PATH);
    QString ksScmcVersion = getVersion(SCMC_VERSION_FILE_PATH);

    if (ksScmcVersion == "")
        info->setText(QString("ks-scmc-gui: %1,").arg(ksScmcGuiVersion) + QString(" ks-scmc: %1").arg("(none)"));
    else
        info->setText(QString("ks-scmc-gui: %1,").arg(ksScmcGuiVersion) + QString(" ks-scmc: %1").arg(ksScmcVersion));
    info->setAlignment(Qt::AlignCenter);

    QLabel *license = new QLabel(aboutWidget);
    license->setStyleSheet("QLabel{"
                           "color:#919191;"
                           "font-size:12px;}");
    license->setText("Copyright (c) 2022 ~ 2023 KylinSec Co. Ltd. All Rights Reserved.");
    license->setAlignment(Qt::AlignCenter);
    vlayout->addSpacerItem(spacer);
    vlayout->addWidget(logo);
    vlayout->addWidget(version);
    vlayout->addWidget(info);
    vlayout->addWidget(license);
    vlayout->addSpacerItem(endSpacer);

    aboutWidget->setLayout(vlayout);
    this->setTitle(tr("About"));
}
