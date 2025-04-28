/**
 * @file          src/pages/node/security-configuration/network-access-ctl-page.cpp
 * @brief
 * @author        yuanxing <yuanxing@kylinsec.com>
 * @copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
 */

#include "network-config-page.h"
#include <widget-property-helper.h>
#include <QButtonGroup>
#include "ui_network-config-page.h"
NetworkConfigPage::NetworkConfigPage(QWidget* parent) : Page(parent),
                                                        ui(new Ui::NetworkConfigPage),
                                                        m_accessCtrlEnabled(false),
                                                        m_whiteListEnabled(false),
                                                        m_accessList(nullptr),
                                                        m_processList(nullptr)
{
    ui->setupUi(this);
    initUI();
}

void NetworkConfigPage::updateInfo(QString keyword)
{
}

void NetworkConfigPage::setAccessCtrlInfo()
{
}

void NetworkConfigPage::getAccessCtrlInfo()
{
}

void NetworkConfigPage::setWhiteListInfo()
{
}

void NetworkConfigPage::getWhiteListInfo()
{
}
void NetworkConfigPage::setConfigEnabled(QAbstractButton* btn)
{
    auto radioBtn = qobject_cast<QRadioButton*>(btn);
    if (!radioBtn)
        return;

    struct ButtonAction
    {
        QRadioButton* button;
        QWidget* widget;
        bool& enabledFlag;
        bool isEnabled;
    };

    static const std::vector<ButtonAction> actions = {
        {ui->btn_access_close, m_accessList, m_accessCtrlEnabled, false},
        {ui->btn_access_open, m_accessList, m_accessCtrlEnabled, true},
        {ui->btn_whitelist_close, m_processList, m_whiteListEnabled, false},
        {ui->btn_whitelist_open, m_processList, m_whiteListEnabled, true}};

    for (const auto& action : actions)
    {
        if (radioBtn == action.button)
        {
            action.widget->setEnabled(action.isEnabled);
            action.enabledFlag = action.isEnabled;
            return;
        }
    }
}

void NetworkConfigPage::save()
{
}

void NetworkConfigPage::initUI()
{
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_save, Kiran::BUTTON_Default);

    m_accessList = new QListWidget(this);
    m_accessList->setFocusPolicy(Qt::NoFocus);
    m_accessList->setFrameShape(QFrame::NoFrame);
    m_accessList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_accessList->setEnabled(false);
    ui->layout_access_control->addWidget(m_accessList);

    m_processList = new QListWidget(this);
    m_processList->setFocusPolicy(Qt::NoFocus);
    m_processList->setFrameShape(QFrame::NoFrame);
    m_processList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_processList->setEnabled(false);
    ui->layout_process->addWidget(m_processList);

    auto accessBtnGroup = new QButtonGroup(this);
    accessBtnGroup->addButton(ui->btn_access_close);
    accessBtnGroup->addButton(ui->btn_access_open);
    ui->btn_access_close->setChecked(true);

    auto whiteListBtnGroup = new QButtonGroup(this);
    whiteListBtnGroup->addButton(ui->btn_whitelist_open);
    whiteListBtnGroup->addButton(ui->btn_whitelist_close);
    ui->btn_whitelist_close->setChecked(true);

    connect(ui->btn_save, &QPushButton::clicked, this, &NetworkConfigPage::save);
    connect(accessBtnGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked), this, &NetworkConfigPage::setConfigEnabled);
    connect(whiteListBtnGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked), this, &NetworkConfigPage::setConfigEnabled);
}

void NetworkConfigPage::initConnect()
{
}
