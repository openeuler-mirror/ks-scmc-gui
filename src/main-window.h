#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <QListWidgetItem>
#include <QMutex>
#include <QStackedWidget>
#include <QWidget>
#include "base/info-worker.h"
#include "def.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class GuideItem;
class CommonPage;
class TransmissionList;
class BubbleTipButton;
class MainWindow : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setUserName(QString name);

protected:
    void onItemClicked(QListWidgetItem *currItem);
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void initUI();
    CommonPage *createSubPage(GUIDE_ITEM itemEnum);
    QListWidgetItem *createGuideItem(QString text, int type = GUIDE_ITEM_TYPE_NORMAL, QString icon = "");
    void setPageName(QString name);

private slots:
    void onLogoutAction(bool checked);
    void popupTransmissionList();
    void getTransferImageStatus(ImageTransmissionStatus status, QString name, QString version, int rate);
    void onTransferItemDeleted(QString name, QString version, ImageTransmissionStatus status);

signals:
    void sigLogout();

private:
    Ui::MainWindow *ui;
    QStackedWidget *m_stackedWidget;
    QMap<int, CommonPage *> m_pageMap;
    QMap<QListWidgetItem *, QList<QListWidgetItem *>> m_groupMap;  //key group ,value subs
    QMap<QListWidgetItem *, bool> m_isShowMap;
    QList<GuideItem *> m_pageItems;
    TransmissionList *m_transmissionList;
    QStringList m_transferImages;
    BubbleTipButton *m_btnTransmission;
    BubbleTipButton *m_btnApproval;
    BubbleTipButton *m_btnWarning;
    QTimer *m_timer;
    QMutex m_mutex;
};
#endif  // MAINWINDOW_H
