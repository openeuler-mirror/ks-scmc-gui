#include "global-security-controller.h"
#include <kiran-switch-button.h>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
GlobalSecurityController::GlobalSecurityController(QWidget *parent) : Page(parent), m_checkbox(nullptr)
{
    initUI();
}

void GlobalSecurityController::updateInfo(QString keyword)
{
}

void GlobalSecurityController::setApprovable(bool)
{
}

void GlobalSecurityController::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(20, 0, 20, 0);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(10);
    QLabel *label = new QLabel(this);
    label->setText(tr("Enable image approval,and verity signature files"));

    m_checkbox = new KiranSwitchButton(this);
    m_checkbox->setObjectName("cb_global_security");
    m_checkbox->setFocusPolicy(Qt::NoFocus);

    layout->addWidget(label);
    layout->addWidget(m_checkbox);
    layout->addStretch();

    mainLayout->addLayout(layout);
    mainLayout->addStretch();

    connect(m_checkbox, &QCheckBox::toggled, this, &GlobalSecurityController::setApprovable);
}

void GlobalSecurityController::setStatus(bool isEnable)
{
    m_checkbox->setChecked(isEnable ? Qt::Checked : Qt::Unchecked);
}
