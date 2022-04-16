#ifndef TABPAGE_H
#define TABPAGE_H

#include <QMap>
#include <QObject>
#include <QTabWidget>
#include <QWidget>
#include "def.h"
#include "page.h"
#include "table-page.h"
class TabPage : public Page
{
    Q_OBJECT
public:
    explicit TabPage(QWidget *parent = nullptr);
    ~TabPage() = default;
    virtual void updateInfo(QString keyword = "") = 0;
    void addTabPage(QWidget *tabPage, QString key);
    void updateCurrentPage();

private:
    void initUI();

private slots:
    void onCurrentPageChanged(int index);

signals:

private:
    QTabWidget *m_tabWidget;
    QMap<QString, QWidget *> m_pageMap;
};

#endif  // TABPAGE_H
