/**
 * @file          /ks-scmc-gui/src/main.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include <kiran-log/qt5-log-i.h>
#include <kiran-single-application.h>
#include <kiranwidgets-qt5/kiran-message-box.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QStyle>
#include <QStyleFactory>
#include <QTranslator>
#include <iostream>
#include "config/config.h"
#include "login-dialog.h"
#define TRANSLATION_DIR TRANSLATIONS_FILE_DIR
#define APP_NAME "ks-scmc-gui"
int main(int argc, char *argv[])
{
    //设置日志输出
    if (klog_qt5_init("", "kylinsec-session", APP_NAME, APP_NAME) < 0)
    {
        std::cout << "init klog error" << std::endl;
    }
    KLOG_INFO("******New Output*********\n");

    KiranSingleApplication a(argc, argv);
    //a.setStyle(QStyleFactory::create("fusion"));

    ///加载qss样式表
    QFile file(":/style/theme.qss");
    if (file.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(file.readAll());

        a.setStyleSheet(styleSheet);
        file.close();
    }
    else
    {
        KiranMessageBox::message(NULL, "Warning", "Open failed", KiranMessageBox::Ok);
    }
    //加载翻译文件
    QTranslator *qtTranslator = new QTranslator(qApp);
    if (qtTranslator->load(QLocale(), APP_NAME, ".", TRANSLATION_DIR, ".qm"))
    {
        a.installTranslator(qtTranslator);
    }
    else
    {
        KLOG_INFO() << "Load Translator File failed :" << QLocale().name() << TRANSLATION_DIR;
    }

    LoginDialog w;

    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    w.move(screenGeometry.x() + (screenGeometry.width() - w.width()) / 2,
           screenGeometry.y() + (screenGeometry.height() - w.height()) / 2);

    w.show();
    return a.exec();
}
