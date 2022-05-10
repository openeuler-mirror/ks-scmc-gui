#include "network-access-ctl-tab.h"
#include <kiran-log/qt5-log-i.h>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QScrollArea>
#include <QToolButton>
#include "network-access-item.h"
NetworkAccessCtlTab::NetworkAccessCtlTab(QWidget *parent) : QWidget(parent),
                                                            m_itemLayout(nullptr),
                                                            m_listWidget(nullptr),
                                                            m_funcDesc(nullptr)
{
    initUI();
}

NetworkAccessCtlTab::~NetworkAccessCtlTab()
{
    if (m_funcDesc)
    {
        delete m_funcDesc;
        m_funcDesc = nullptr;
    }
}

void NetworkAccessCtlTab::setNetworkAccessInfo()
{
}

void NetworkAccessCtlTab::getNetworkAccessInfo()
{
}

NetworkAccessItem *NetworkAccessCtlTab::createItem(int index)
{
    QListWidgetItem *newItem = nullptr;
    NetworkAccessItem *customItem = nullptr;

    newItem = new QListWidgetItem(m_listWidget);

    customItem = new NetworkAccessItem(m_listWidget);
    customItem->setNetworkInfo(index);
    connect(customItem, &NetworkAccessItem::sigAdd, this, &NetworkAccessCtlTab::addItem);
    connect(customItem, &NetworkAccessItem::sigDelete, this, &NetworkAccessCtlTab::deleteItem);

    m_listWidget->insertItem(index, newItem);
    m_listWidget->setItemWidget(newItem, customItem);

    newItem->setSizeHint(QSize(500, 68));
    m_listWidget->setGridSize(QSize(500, 78));

    return customItem;
}

void NetworkAccessCtlTab::initUI()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setContentsMargins(20, 20, 10, 0);
    layout->setSpacing(0);

    m_listWidget = new QListWidget(this);
    m_listWidget->setFocusPolicy(Qt::NoFocus);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    createItem(0);

    QVBoxLayout *btnLayout = new QVBoxLayout();
    btnLayout->setMargin(0);
    QToolButton *funcDescBtn = new QToolButton(this);
    funcDescBtn->setText("Function\ndescription");
    funcDescBtn->setFixedSize(20, 20);
    funcDescBtn->setStyleSheet("font-size:10px;");
    btnLayout->addWidget(funcDescBtn);
    btnLayout->addStretch();

    layout->addWidget(m_listWidget);
    layout->addLayout(btnLayout);

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
    //    textBrowser->setHtml("<div id='u1489_text' class='text '>"
    //                         "<p style='font-size:20px;'><span style='font-weight:650;color:#555555;'>进程白名单-功能说明</span></p>"
    //                         "<p style='font-size:14px;'><span style='font-weight:400;'>[规则]</span></p>"
    //                         "<p style='font-size:14px;'><span style='font-weight:400;'>1、白名单状态：开启与关闭；</span></p>"
    //                         "<p style='font-size:14px;'><span style='font-weight:400;'>组件为单选项，默认为开启状态；</span></p>"
    //                         "<p style='font-size:14px;'><span style='font-weight:400;'>当白名单状态为开启时，才可对进程白名单进行编辑、修改、删除；</span></p>"
    //                         "<p style='font-size:14px;'><span style='font-weight:400;'>当白名单为关闭状态时，进程白名称组件都为禁用状态。</span></p>"
    //                         "<p style='font-size:14px;'><span style='font-weight:400;'>2、进程白名单：非必须项；</span></p>"
    //                         "<p style='font-size:14px;'><span style='font-weight:400;'>填写内容为路径，精确到进程名称；</span></p>"
    //                         "<p style='font-size:14px;'><span style='font-weight:400;'>当失去光标时，进行判断路径格式是否正确；windows ： 可以使用单斜杠和双斜杠 \\ ，都没有问题、Linux： 认准 /；</span></p>"
    //                         "<p style='font-size:14px;'><span style='font-weight:400;'>错误信息提示在组件下，提示格式为“进程路径有误，请重新填写”</span></p>"
    //                         "</div>");

    textBrowser->setHtml("<div id='u1490_text' class='text '>"
                         "<p style='font-size:20px;'><span style='font-size: 14px;font-weight:650;color:#555555;'>网络访问控制-功能说明</span></p>"
                         "<p style='font-size:20px;'><span style='font-size: 14px;font-weight:400;'>[规则]</span></p>"
                         "<p style='font-size:20px;'><span style='font-size: 14px;font-weight:400;'>1、网络协议：分类为TCP、UDP、ICMP；</span></p>"
                         "<p style='font-size:20px;'><span style='font-size: 14px;font-weight:400;'>支持多选，至少选择一个协议；</span></p>"
                         "<p style='font-size:20px;'><span style='font-size: 14px;font-weight:400;'>当白名单状态为开启时，才可对进程白名单进行编辑、修改、删除；</span></p>"
                         "<p style='font-size:20px;'><span style='font-size: 14px;font-weight:400;'>当白名单为关闭状态时，进程白名称组件都为禁用状态。</span></p>"
                         "<p style='font-size:20px;'><span style='font-size: 14px;font-weight:400;'>2、网络地址：非必填项；</span></p>"
                         "<p style='font-size:20px;'><span style='font-size: 14px;font-weight:400;'>网络地址可以填写单个 IP 地址，单个 IP 地址采用点分十进制表示，例如</span></p>"
                         "<p style='font-size:20px;'><span style='font-size: 14px;font-weight:400;'>192.168.1.1；也可以填写一个网段，由点分十进制的 IP 地址与掩码组成，例如192.168.1.1/24。</span></p>"
                         "<p style='font-size:20px;'><span style='font-size: 14px;font-weight:400;'>3、端口：非必填项，默认为0；</span></p>"
                         "<p style='font-size:20px;'><span style='font-size: 14px;font-weight:400;'>端口合法范围为 1~65535。也可以填写 0 表示允许所有端口。</span></p>"
                         "<p style='font-size:20px;'><span style='font-size: 14px;font-weight:400;'>4、添加：添加数据不限制；</span></p>"
                         "<p style='font-size:20px;'><span style='font-size: 14px;font-weight:400;'>5、删除：当只有一条数据时，无删除按钮</span></p>"
                         "</div>");
    funcLayout->addWidget(textBrowser);

    connect(funcDescBtn, &QToolButton::clicked, this, &NetworkAccessCtlTab::popuoFuncDesc);
}

void NetworkAccessCtlTab::addItem()
{
    NetworkAccessItem *item = qobject_cast<NetworkAccessItem *>(sender());

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

void NetworkAccessCtlTab::deleteItem()
{
    NetworkAccessItem *item = qobject_cast<NetworkAccessItem *>(sender());

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
}

void NetworkAccessCtlTab::popuoFuncDesc()
{
    QToolButton *btn = qobject_cast<QToolButton *>(sender());
    QPoint point = btn->mapToGlobal(QPoint(0, 0));
    KLOG_INFO() << point;

    m_funcDesc->move(QPoint(point.x() - 600, point.y() - 200));
    m_funcDesc->show();
}
