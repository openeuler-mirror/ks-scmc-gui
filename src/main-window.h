/**
 * @file          /ks-scmc-gui/src/main-window.h
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <QListWidgetItem>
#include <QMutex>
#include <QStackedWidget>
#include <QWidget>
#include "def.h"
#include "info-worker.h"
#include "pages/outline/outline-view.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class GuideItem;
class TablePage;
class TransmissionList;
class BubbleTipButton;
class Page;
class PasswdUpdateDialog;
class MainWindow : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    MainWindow(QString name, QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void onItemClicked(QListWidgetItem *currItem);
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event);
    //void closeEvent(QCloseEvent *event);
    //void timerEvent(QTimerEvent *event);

private:
    void initUI();
    Page *createSubPage(GUIDE_ITEM itemEnum);
    void loadUserPage();
    void loadUserItem();
    QListWidgetItem *createGuideItem(QString text, int type = GUIDE_ITEM_TYPE_NORMAL, QString icon = "");
    void setPageName(QString name);
    void outlinePageChange(QString str);

private slots:
    void onLogoutAction(bool checked);
    void onChangePwAction(bool checked);
    void onAboutAction(bool checked);
    void onUpdatePwSuccessful();
    void popupTransmissionList();
    void onApprovalPage(bool check);
    void onWarningPage(bool check);
    void setApprovalTipNums(int);
    void setWarningTipNums(int);
    void getTransferImageStatus(ImageTransmissionStatus status, QString name, QString version, int rate);
    void onTransferItemDeleted(QString name, QString version, ImageTransmissionStatus status);
    void outlineJumpPage(OutlineCellType type);
    void changePage(int);
    void onUpdateWarnTipsSums();

signals:
    void sigLogout();

private:
    Ui::MainWindow *ui;
    QString m_userName;
    QStackedWidget *m_stackedWidget;
    QMap<QString, Page *> m_pageMap;
    QMap<QListWidgetItem *, QList<QListWidgetItem *>> m_groupMap;  //key group ,value subs
    QMap<QListWidgetItem *, bool> m_isShowMap;
    QList<GuideItem *> m_pageItems;
    TransmissionList *m_transmissionList;
    QStringList m_transferImages;
    BubbleTipButton *m_btnTransmission;
    BubbleTipButton *m_btnApproval;
    BubbleTipButton *m_btnWarning;
    QMutex m_mutex;
    PasswdUpdateDialog *m_pwUpdateDlg;
    //    bool m_filterInput;
    //    bool m_isClose;
    //    int m_timerId;
};
#endif  // MAINWINDOW_H
