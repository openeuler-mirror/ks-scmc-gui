#include "security-list-tab.h"
#include <kiran-log/qt5-log-i.h>
#include <QRadioButton>
#include <QToolButton>
#include <QVBoxLayout>
#include "security-list-item.h"
SecurityListTab::SecurityListTab(ProtectionType type, QWidget *parent) : QWidget(parent),
                                                                         m_listWidget(nullptr),
                                                                         m_funcDesc(nullptr),
                                                                         m_btnOpen(nullptr),
                                                                         m_btnClose(nullptr),
                                                                         m_type(type),
                                                                         m_isEnable(true)
{
    initUI();
    m_btnClose->setChecked(true);
    createItem(0);
}

SecurityListTab::~SecurityListTab()
{
    if (m_funcDesc)
    {
        delete m_funcDesc;
        m_funcDesc = nullptr;
    }
}

void SecurityListTab::setSecurityListInfo(container::SecurityConfig *securityCfg)
{
    if (!securityCfg)
        return;

    switch (m_type)
    {
    case PROTECT_FILE_LIST:
    {
        if (!securityCfg->has_file_protection())
            return;

        auto fileProtect = securityCfg->file_protection();
        if (fileProtect.is_on())
            m_btnOpen->setChecked(true);
        else
            m_btnClose->setChecked(true);

        int count = 0;
        for (auto file : fileProtect.file_list())
        {
            if (count > 0)
                createItem(count);

            auto listItem = m_listWidget->item(count);
            SecurityListItem *item = qobject_cast<SecurityListItem *>(m_listWidget->itemWidget(listItem));
            item->setInfo(QString::fromStdString(file));
            count++;
        }
        break;
    }
    case EXEC_WHITELIST:
    {
        if (!securityCfg->has_proc_protection())
            return;

        auto processProtect = securityCfg->proc_protection();
        if (processProtect.is_on())
            m_btnOpen->setChecked(true);
        else
            m_btnClose->setChecked(true);

        int count = 0;
        for (auto file : processProtect.exe_list())
        {
            if (count > 0)
                createItem(count);

            auto listItem = m_listWidget->item(count);
            SecurityListItem *item = qobject_cast<SecurityListItem *>(m_listWidget->itemWidget(listItem));
            item->setInfo(QString::fromStdString(file));
            count++;
        }
        break;
    }
    case NET_EXEC_WHITELIST:
    {
        if (!securityCfg->has_nproc_protection())
            return;

        auto processProtect = securityCfg->nproc_protection();
        if (processProtect.is_on())
            m_btnOpen->setChecked(true);
        else
            m_btnClose->setChecked(true);

        int count = 0;
        for (auto file : processProtect.exe_list())
        {
            if (count > 0)
                createItem(count);

            auto listItem = m_listWidget->item(count);
            SecurityListItem *item = qobject_cast<SecurityListItem *>(m_listWidget->itemWidget(listItem));
            item->setInfo(QString::fromStdString(file));
            count++;
        }
        break;
    }
    default:
        break;
    }
}

bool SecurityListTab::getSecurityListInfo(container::SecurityConfig *securityCfg)
{
    bool pathCorrect = true;
    switch (m_type)
    {
    case PROTECT_FILE_LIST:
    {
        auto fileProtect = securityCfg->mutable_file_protection();
        fileProtect->set_is_on(m_isEnable);
        for (int i = 0; i < m_listWidget->count(); ++i)
        {
            auto listItem = m_listWidget->item(i);
            auto item = qobject_cast<SecurityListItem *>(m_listWidget->itemWidget(listItem));
            if (item->getPathCorrect())
            {
                QString filePath = item->getInfo();
                KLOG_INFO() << "filePath:" << filePath;
                if (!filePath.isEmpty())
                    fileProtect->add_file_list(filePath.toStdString());
            }
            else
                pathCorrect = false;
        }
        break;
    }
    case EXEC_WHITELIST:
    {
        auto processProtect = securityCfg->mutable_proc_protection();
        processProtect->set_protection_type(security::EXEC_WHITELIST);
        processProtect->set_is_on(m_isEnable);
        for (int i = 0; i < m_listWidget->count(); ++i)
        {
            auto listItem = m_listWidget->item(i);
            auto item = qobject_cast<SecurityListItem *>(m_listWidget->itemWidget(listItem));
            if (item->getPathCorrect())
            {
                QString filePath = item->getInfo();
                KLOG_INFO() << "process path:" << filePath;
                if (!filePath.isEmpty())
                    processProtect->add_exe_list(filePath.toStdString());
            }
            else
                pathCorrect = false;
        }
        break;
    }
    case NET_EXEC_WHITELIST:
    {
        auto processProtect = securityCfg->mutable_nproc_protection();
        processProtect->set_protection_type(security::NET_WHITELIST);
        processProtect->set_is_on(m_isEnable);
        for (int i = 0; i < m_listWidget->count(); ++i)
        {
            auto listItem = m_listWidget->item(i);
            auto item = qobject_cast<SecurityListItem *>(m_listWidget->itemWidget(listItem));
            if (item->getPathCorrect())
            {
                QString filePath = item->getInfo();
                KLOG_INFO() << "net process path:" << filePath;
                if (!filePath.isEmpty())
                    processProtect->add_exe_list(filePath.toStdString());
            }
            else
                pathCorrect = false;
        }
        break;
    }

    default:
        break;
    }
    return pathCorrect;
}

SecurityListItem *SecurityListTab::createItem(int index)
{
    QListWidgetItem *newItem = nullptr;
    SecurityListItem *customItem = nullptr;
    QString itemText;

    newItem = new QListWidgetItem();

    int itemIndex = index + 1;
    if (m_type == PROTECT_FILE_LIST)
        itemText = tr("Key file %1").arg(itemIndex);
    else if (m_type == EXEC_WHITELIST)
        itemText = tr("Process %1").arg(itemIndex);
    else if (m_type == NET_EXEC_WHITELIST)
        itemText = tr("Network process %1").arg(itemIndex);

    customItem = new SecurityListItem(itemText, m_listWidget);
    if (itemIndex == 1)
        customItem->setDeleteBtnVisible(false);

    connect(customItem, &SecurityListItem::sigAdd, this, &SecurityListTab::addItem);
    connect(customItem, &SecurityListItem::sigDelete, this, &SecurityListTab::deleteItem);

    m_listWidget->insertItem(index, newItem);
    m_listWidget->setItemWidget(newItem, customItem);

    newItem->setSizeHint(QSize(340, 62));
    m_listWidget->setGridSize(QSize(340, 68));

    return customItem;
}

void SecurityListTab::addItem()
{
    KLOG_INFO() << "addItem";
    SecurityListItem *item = qobject_cast<SecurityListItem *>(sender());

    int row = 0;
    while (row < m_listWidget->count())
    {
        QListWidgetItem *listItem = m_listWidget->item(row);
        if (m_listWidget->itemWidget(listItem) == item)
        {
            KLOG_INFO() << "NetworkAccessItem index:" << row;
            createItem(row + 1);
            updateItemName();
            break;
        }
        row++;
    }
}

void SecurityListTab::deleteItem()
{
    KLOG_INFO() << "deleteItem";
    SecurityListItem *item = qobject_cast<SecurityListItem *>(sender());

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

            updateItemName();
            break;
        }
        row++;
    }

    if (m_listWidget->count() == 1)
    {
        SecurityListItem *item = qobject_cast<SecurityListItem *>(m_listWidget->itemWidget(m_listWidget->item(0)));
        item->setDeleteBtnVisible(false);
    }
}

void SecurityListTab::updateItemName()
{
    int row = 0;
    while (row < m_listWidget->count())
    {
        QListWidgetItem *listItem = m_listWidget->item(row);
        SecurityListItem *item = qobject_cast<SecurityListItem *>(m_listWidget->itemWidget(listItem));

        QString itemName;
        if (m_type == PROTECT_FILE_LIST)
            itemName = tr("Key file %1").arg(row + 1);
        else if (m_type == EXEC_WHITELIST)
            itemName = tr("Process %1").arg(row + 1);
        else if (m_type == NET_EXEC_WHITELIST)
            itemName = tr("Network process %1").arg(row + 1);

        item->updateItemName(itemName);
        row++;
    }
}

void SecurityListTab::popuoFuncDesc()
{
    QToolButton *btn = qobject_cast<QToolButton *>(sender());
    QPoint point = btn->mapToGlobal(QPoint(0, 0));
    KLOG_INFO() << point;

    m_funcDesc->move(QPoint(point.x() - 600, point.y() - 200));
    m_funcDesc->show();
}

void SecurityListTab::initUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 0);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setMargin(0);
    topLayout->setSpacing(20);
    QLabel *labStatus = new QLabel(this);
    if (m_type == PROTECT_FILE_LIST)
        labStatus->setText(tr("Open status"));
    else
        labStatus->setText(tr("White list status"));

    m_btnOpen = new QRadioButton(tr("Open"), this);
    m_btnClose = new QRadioButton(tr("Close"), this);

    topLayout->addWidget(labStatus);
    topLayout->addWidget(m_btnOpen);
    topLayout->addWidget(m_btnClose);
    topLayout->addStretch();

    m_listWidget = new QListWidget(this);
    m_listWidget->setFocusPolicy(Qt::NoFocus);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_listWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    layout->addLayout(topLayout);
    layout->addWidget(m_listWidget);

    QVBoxLayout *descBtnLayout = new QVBoxLayout();
    descBtnLayout->setMargin(0);
    descBtnLayout->setSpacing(0);

    QToolButton *funcDescBtn = new QToolButton(this);
    funcDescBtn->setObjectName("funcDescBtn");
    funcDescBtn->setText("Function\nInstruction");
    funcDescBtn->setFixedSize(50, 40);
    funcDescBtn->setCursor(Qt::PointingHandCursor);
    connect(funcDescBtn, &QToolButton::clicked, this, &SecurityListTab::popuoFuncDesc);

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

    if (m_type == PROTECT_FILE_LIST)
    {
        funcDescBtn->hide();
    }

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
