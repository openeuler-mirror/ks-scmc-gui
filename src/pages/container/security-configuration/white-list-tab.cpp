#include "white-list-tab.h"
#include <kiran-log/qt5-log-i.h>
#include <QLabel>
#include <QRadioButton>
#include <QTextBrowser>
#include <QToolButton>
#include <QVBoxLayout>
#include "white-list-item.h"
WhiteListTab::WhiteListTab(PROC_PROTECTION type, QWidget *parent) : QWidget(parent),
                                                                    m_listWidget(nullptr),
                                                                    m_funcDesc(nullptr),
                                                                    m_type(type)
{
    initUI();
    createItem(0);
}

WhiteListTab::~WhiteListTab()
{
    if (m_funcDesc)
    {
        delete m_funcDesc;
        m_funcDesc = nullptr;
    }
}

void WhiteListTab::setWhiteListInfo()
{
}

void WhiteListTab::getWhiteListInfo()
{
}

WhiteListItem *WhiteListTab::createItem(int index)
{
    QListWidgetItem *newItem = nullptr;
    WhiteListItem *customItem = nullptr;

    newItem = new QListWidgetItem(m_listWidget);

    customItem = new WhiteListItem(index, m_listWidget);
    if (index == 0)
        customItem->setDeleteBtnVisible(false);
    connect(customItem, &WhiteListItem::sigAdd, this, &WhiteListTab::addItem);
    connect(customItem, &WhiteListItem::sigDelete, this, &WhiteListTab::deleteItem);

    m_listWidget->insertItem(index, newItem);
    m_listWidget->setItemWidget(newItem, customItem);

    newItem->setSizeHint(QSize(500, 30));
    m_listWidget->setGridSize(QSize(500, 40));

    return customItem;
}

void WhiteListTab::addItem()
{
    WhiteListItem *item = qobject_cast<WhiteListItem *>(sender());

    int row = 0;
    while (row < m_listWidget->count())
    {
        QListWidgetItem *listItem = m_listWidget->item(row);
        if (m_listWidget->itemWidget(listItem) == item)
        {
            KLOG_INFO() << "NetworkAccessItem index:" << row;
            createItem(row + 1);

            break;
        }
        row++;
    }
}

void WhiteListTab::deleteItem()
{
    WhiteListItem *item = qobject_cast<WhiteListItem *>(sender());

    int row = 0;
    while (row < m_listWidget->count() && m_listWidget->count() > 1)
    {
        QListWidgetItem *listItem = m_listWidget->item(row);
        if (m_listWidget->itemWidget(listItem) == item)
        {
            KLOG_INFO() << "NetworkAccessItem index:" << row;
            QListWidgetItem *delItem = m_listWidget->takeItem(row);
            delete delItem;
            delItem = nullptr;
            break;
        }
        row++;
    }
    if (m_listWidget->count() == 1)
    {
        WhiteListItem *item = qobject_cast<WhiteListItem *>(m_listWidget->itemWidget(m_listWidget->item(0)));
        item->setDeleteBtnVisible(false);
    }
}

void WhiteListTab::popuoFuncDesc()
{
    QToolButton *btn = qobject_cast<QToolButton *>(sender());
    QPoint point = btn->mapToGlobal(QPoint(0, 0));
    KLOG_INFO() << point;

    m_funcDesc->move(QPoint(point.x() - 600, point.y() - 200));
    m_funcDesc->show();
}

void WhiteListTab::initUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 10, 0);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setMargin(0);
    topLayout->setSpacing(20);
    QLabel *labStatus = new QLabel(this);
    labStatus->setText(tr("White list status"));

    QRadioButton *btnOpen = new QRadioButton(tr("Open"), this);
    QRadioButton *btnClose = new QRadioButton(tr("Close"), this);

    topLayout->addWidget(labStatus);
    topLayout->addWidget(btnOpen);
    topLayout->addWidget(btnClose);
    topLayout->addStretch();

    m_listWidget = new QListWidget(this);
    m_listWidget->setFocusPolicy(Qt::NoFocus);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    layout->addLayout(topLayout);
    layout->addWidget(m_listWidget);
    layout->addStretch();

    QVBoxLayout *descBtnLayout = new QVBoxLayout();
    descBtnLayout->setMargin(0);
    descBtnLayout->setSpacing(0);

    QToolButton *funcDescBtn = new QToolButton(this);
    funcDescBtn->setText("Instruction");
    funcDescBtn->setFixedSize(50, 20);
    funcDescBtn->setStyleSheet("border:none;border-radius:4px;font-size:10px;background-color:#2eb3ff;color:#000000;");
    connect(funcDescBtn, &QToolButton::clicked, this, &WhiteListTab::popuoFuncDesc);

    descBtnLayout->addWidget(funcDescBtn);
    descBtnLayout->addStretch();

    mainLayout->addLayout(layout);
    mainLayout->addLayout(descBtnLayout);

    //创建功能描述弹出控件
    m_funcDesc = new QWidget();
    m_funcDesc->setWindowFlags(Qt::Widget | Qt::Popup | Qt::FramelessWindowHint);
    m_funcDesc->setAttribute(Qt::WA_TranslucentBackground, true);
    m_funcDesc->setStyleSheet("background-color:#F7FCFF;border-radius:4px;");

    QVBoxLayout *funcLayout = new QVBoxLayout();
    funcLayout->setMargin(0);
    funcLayout->setSpacing(0);
    m_funcDesc->setLayout(funcLayout);

    QTextBrowser *textBrowser = new QTextBrowser(m_funcDesc);
    textBrowser->setFixedSize(495, 500);
    textBrowser->setHtml("<div id='u1489_text' class='text '>"
                         "<p style='font-size:20px;'><span style='font-weight:650;color:#555555;'>进程白名单-功能说明</span></p>"
                         "<p style='font-size:14px;'><span style='font-weight:400;'>[规则]</span></p>"
                         "<p style='font-size:14px;'><span style='font-weight:400;'>1、白名单状态：开启与关闭；</span></p>"
                         "<p style='font-size:14px;'><span style='font-weight:400;'>组件为单选项，默认为开启状态；</span></p>"
                         "<p style='font-size:14px;'><span style='font-weight:400;'>当白名单状态为开启时，才可对进程白名单进行编辑、修改、删除；</span></p>"
                         "<p style='font-size:14px;'><span style='font-weight:400;'>当白名单为关闭状态时，进程白名称组件都为禁用状态。</span></p>"
                         "<p style='font-size:14px;'><span style='font-weight:400;'>2、进程白名单：非必须项；</span></p>"
                         "<p style='font-size:14px;'><span style='font-weight:400;'>填写内容为路径，精确到进程名称；</span></p>"
                         "<p style='font-size:14px;'><span style='font-weight:400;'>当失去光标时，进行判断路径格式是否正确；windows ： 可以使用单斜杠和双斜杠 \\ ，都没有问题、Linux： 认准 /；</span></p>"
                         "<p style='font-size:14px;'><span style='font-weight:400;'>错误信息提示在组件下，提示格式为“进程路径有误，请重新填写”</span></p>"
                         "</div>");
    funcLayout->addWidget(textBrowser);

    connect(btnClose, &QRadioButton::clicked,
            [=] {
                m_listWidget->setDisabled(true);
            });
    connect(btnOpen, &QRadioButton::clicked,
            [=] {
                m_listWidget->setDisabled(false);
            });
}
