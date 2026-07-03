#include "login.h"
#include <kiran-log/qt5-log-i.h>
#include <QApplication>
#include <QDesktopWidget>
#include "common/message-dialog.h"
#include "main-window.h"
#include "ui_login.h"

//std::string g_server_addr = "10.200.12.181:10060";
std::string g_server_addr;
Login::Login(QWidget *parent) : KiranTitlebarWindow(parent),
                                ui(new Ui::Login),
                                m_mainWindow(nullptr)
{
    ui->setupUi(getWindowContentWidget());
    setTitle(tr("KylinSec security Container magic Cube"));
    QStringList args = qApp->arguments();
    if (args.size() > 1)
    {
        g_server_addr = args[1].toStdString();
    }

    ui->lineEdit_user_name->setText("test");
    ui->lineEdit_passwd->setText("12345678");
    ui->lineEdit_server->setText("10.200.12.181:10060");

    ui->lineEdit_passwd->setEchoMode(QLineEdit::Password);
    connect(ui->btn_login, &QPushButton::clicked, this, &Login::onLogin);
}

Login::~Login()
{
    KLOG_INFO() << "*************Deconstruction Login";
    delete ui;
    if (m_mainWindow)
    {
        delete m_mainWindow;
        m_mainWindow = nullptr;
    }
}

bool Login::inspectLoginParam()
{
    if (ui->lineEdit_user_name->text().isEmpty())
    {
        ui->lab_tips->setText(tr("Please input user name!"));
        return false;
    }
    if (ui->lineEdit_passwd->text().isEmpty())
    {
        ui->lab_tips->setText(tr("Please input password!"));
        return false;
    }
    else if (ui->lineEdit_passwd->text().size() < 8)
    {
        ui->lab_tips->setText(tr("Please enter a password with at least 8 digits!"));
        return false;
    }

    if (ui->lineEdit_server->text().isEmpty())
    {
        ui->lab_tips->setText(tr("Please enter a server address!"));
        return false;
    }
    return true;
}

void Login::onLogin()
{
    if (!inspectLoginParam())
        return;
    connect(&InfoWorker::getInstance(), &InfoWorker::loginFinished, this, &Login::getLoginResult);
    g_server_addr = ui->lineEdit_server->text().toStdString();
    InfoWorker::getInstance().login(ui->lineEdit_user_name->text().toStdString(), ui->lineEdit_passwd->text().toStdString());
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));  //等待旋转
}

void Login::onLogout()
{
    InfoWorker::getInstance().logout();
    connect(&InfoWorker::getInstance(), &InfoWorker::logoutFinished, this, &Login::getLogoutResult);
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));  //等待旋转
}

void Login::getLoginResult(const QPair<grpc::Status, user::LoginReply> &reply)
{
    KLOG_INFO() << "getLoginResult";
    QApplication::restoreOverrideCursor();

    if (reply.first.ok())
    {
        if (!m_mainWindow)
        {
            m_mainWindow = new MainWindow();
            m_mainWindow->setUserName(ui->lineEdit_user_name->text());
            m_mainWindow->show();
            connect(m_mainWindow, &MainWindow::sigLogout, this, &Login::onLogout);
            int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
            QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
            int iTitleBarHeight = m_mainWindow->style()->pixelMetric(QStyle::PM_TitleBarHeight);                               // 获取标题栏高度
            m_mainWindow->setGeometry(0, iTitleBarHeight, screenGeometry.width(), screenGeometry.height() - iTitleBarHeight);  // 设置窗体充满桌面客户区
            hide();
        }
    }
    else
    {
        ui->lab_tips->setText(tr("Login failed! %1").arg(reply.first.error_message().data()));
    }
}

void Login::getLogoutResult(const QPair<grpc::Status, user::LogoutReply> &reply)
{
    KLOG_INFO() << "getLogoutResult";
    QApplication::restoreOverrideCursor();

    if (reply.first.ok())
    {
        if (m_mainWindow)
        {
            delete m_mainWindow;
            m_mainWindow = nullptr;
        }
        show();
    }
    else
    {
        MessageDialog::message(tr("Logout"),
                               tr("Logout failed!"),
                               tr("Error: ") + reply.first.error_message().data(),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
    }
}
