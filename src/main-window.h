#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QWidget>
#include "common/def.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class GuideItem;
class CommonPage;
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

private:
    void initUI();
    CommonPage *createSubPage(GUIDE_ITEM itemEnum);
    QListWidgetItem *createGuideItem(QString text, int type = GUIDE_ITEM_TYPE_NORMAL, QString icon = "");
    void setPageName(QString name);

private slots:
    void onLogoutAction(QAction *action);

signals:
    void sigLogout();

private:
    Ui::MainWindow *ui;
    QStackedWidget *m_stackedWidget;
    QMap<int, CommonPage *> m_pageMap;
    QMap<QListWidgetItem *, QList<QListWidgetItem *>> m_groupMap;  //key group ,value subs
    QMap<QListWidgetItem *, bool> m_isShowMap;
    QList<GuideItem *> m_pageItems;
};
#endif  // MAINWINDOW_H
