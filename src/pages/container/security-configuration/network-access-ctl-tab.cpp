#include "network-access-ctl-tab.h"
#include <kiran-log/qt5-log-i.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QScrollArea>
#include <QToolButton>
#include "network-access-item.h"
NetworkAccessCtlTab::NetworkAccessCtlTab(QWidget *parent) : QWidget(parent),
                                                            m_listWidget(nullptr),
                                                            m_funcDesc(nullptr),
                                                            m_btnOpen(nullptr),
                                                            m_btnClose(nullptr),
                                                            m_isEnable(true)
{
    initUI();
    m_btnClose->setChecked(true);
    createItem(0);
}

NetworkAccessCtlTab::~NetworkAccessCtlTab()
{
    if (m_funcDesc)
    {
        delete m_funcDesc;
        m_funcDesc = nullptr;
    }
}

void NetworkAccessCtlTab::setNetworkAccessInfo(container::SecurityConfig *securityCfg)
{
    if (!securityCfg || !securityCfg->has_network_rule())
        return;

    auto networkRuleList = securityCfg->network_rule();
    bool isOn = networkRuleList.is_on();
    if (isOn)
        m_btnOpen->setChecked(true);
    else
        m_btnClose->setChecked(true);

    int count = 0;
    QStringList protocols;
    KLOG_INFO() << "*********" << networkRuleList.rules_size();
    for (auto rule : networkRuleList.rules())
    {
        if (count > 0)
            createItem(count);
        for (std::string protocol : rule.protocols())
        {
            QString strProtocol = QString::fromStdString(protocol);
            if (0 == strProtocol.compare("tcp", Qt::CaseInsensitive) ||
                0 == strProtocol.compare("udp", Qt::CaseInsensitive) ||
                0 == strProtocol.compare("icmp", Qt::CaseInsensitive))

            {
                protocols.append(strProtocol);
            }
        }
        QString addr = QString::fromStdString(rule.addr().data());
        int port = rule.port();

        auto listItem = m_listWidget->item(count);
        NetworkAccessItem *item = qobject_cast<NetworkAccessItem *>(m_listWidget->itemWidget(listItem));
        item->setInfo(protocols, addr, port);
        count++;
    }
}

void NetworkAccessCtlTab::getNetworkAccessInfo(container::SecurityConfig *securityCfg)
{
    auto networkRuleList = securityCfg->mutable_network_rule();
    networkRuleList->set_is_on(m_isEnable);
    for (int i = 0; i < m_listWidget->count(); i++)
    {
        auto listItem = m_listWidget->item(i);
        auto item = qobject_cast<NetworkAccessItem *>(m_listWidget->itemWidget(listItem));
        auto rules = networkRuleList->add_rules();

        QStringList protocols;
        QString addr;
        int port;
        item->getInfo(protocols, addr, port);
        KLOG_INFO() << protocols << addr << port;
        foreach (QString protocol, protocols)
        {
            rules->add_protocols(protocol.toStdString());
        }
        rules->set_addr(addr.toStdString());
        rules->set_port(port);
    }
}

NetworkAccessItem *NetworkAccessCtlTab::createItem(int index)
{
    QListWidgetItem *newItem = nullptr;
    NetworkAccessItem *customItem = nullptr;

    newItem = new QListWidgetItem();

    customItem = new NetworkAccessItem(m_listWidget);
    connect(customItem, &NetworkAccessItem::sigAdd, this, &NetworkAccessCtlTab::addItem);
    connect(customItem, &NetworkAccessItem::sigDelete, this, &NetworkAccessCtlTab::deleteItem);

    if (index == 0)
        customItem->setDeleteBtnVisible(false);

    m_listWidget->insertItem(index, newItem);
    m_listWidget->setItemWidget(newItem, customItem);

    newItem->setSizeHint(QSize(500, 68));
    m_listWidget->setGridSize(QSize(500, 78));

    return customItem;
}

void NetworkAccessCtlTab::initUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(20, 20, 20, 0);
    mainLayout->setSpacing(0);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(20);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setMargin(0);
    topLayout->setSpacing(20);
    QLabel *labStatus = new QLabel(this);
    labStatus->setText(tr("Access status"));

    m_btnOpen = new QRadioButton(tr("Open"), this);
    m_btnClose = new QRadioButton(tr("Close"), this);

    topLayout->addWidget(labStatus);
    topLayout->addWidget(m_btnOpen);
    topLayout->addWidget(m_btnClose);
    topLayout->addStretch();

    m_listWidget = new QListWidget(this);
    m_listWidget->setFocusPolicy(Qt::NoFocus);
    m_listWidget->setFrameShape(QFrame::NoFrame);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    layout->addLayout(topLayout);
    layout->addWidget(m_listWidget);

    QVBoxLayout *btnLayout = new QVBoxLayout();
    btnLayout->setMargin(0);
    QToolButton *funcDescBtn = new QToolButton(this);
    funcDescBtn->setObjectName("funcDescBtn");
    funcDescBtn->setText("Function\nInstruction");
    funcDescBtn->setCursor(Qt::PointingHandCursor);
    funcDescBtn->setFixedSize(50, 40);
    btnLayout->addWidget(funcDescBtn);
    btnLayout->addStretch();

    mainLayout->addLayout(layout);
    mainLayout->addLayout(btnLayout);

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
    connect(m_btnClose, &QRadioButton::toggled,
            [=](bool checked) {
                if (checked == true)
                {
                    m_listWidget->setDisabled(true);
                    m_isEnable = false;
                }
            });
    connect(m_btnOpen, &QRadioButton::toggled,
            [=](bool checked) {
                if (checked == true)
                {
                    m_listWidget->setDisabled(false);
                    m_isEnable = true;
                }
            });
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
