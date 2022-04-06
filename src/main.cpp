#include <kiran-log/qt5-log-i.h>
#include <kiran-single-application.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QMessageBox>
#include <QStyle>
#include <QTranslator>
#include <iostream>
#include "config/config.h"
#include "main-window.h"
#include "pages/user/login.h"

#define TRANSLATION_DIR TRANSLATIONS_FILE_DIR
int main(int argc, char *argv[])
{
    //设置日志输出
    if (klog_qt5_init("", "kylinsec-session", "ks-scmc-gui", "ks-scmc-gui") < 0)
    {
        std::cout << "init klog error" << std::endl;
    }
    KLOG_INFO("******New Output*********\n");

    KiranSingleApplication a(argc, argv);

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
        QMessageBox::warning(NULL, "warning", "Open failed", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    //加载翻译文件
    QTranslator *qtTranslator = new QTranslator(qApp);
    if (qtTranslator->load(QLocale(), "ks-scmc", ".", TRANSLATION_DIR, ".qm"))
    {
        a.installTranslator(qtTranslator);
    }
    else
    {
        qDebug("Load Translator File failed : %s\n", TRANSLATION_DIR);
    }

    Login w;

    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    w.move(screenGeometry.x() + (screenGeometry.width() - w.width()) / 2,
           screenGeometry.y() + (screenGeometry.height() - w.height()) / 2);

    w.show();
    return a.exec();
}
