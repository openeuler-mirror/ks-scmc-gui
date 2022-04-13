#include "config-operate-widget.h"
#include "ui_config-operate-widget.h"

ConfigOperateWidget::ConfigOperateWidget(int curRow, int page, QWidget *parent) : QWidget(parent),
                                                                                  ui(new Ui::ConfigOperateWidget),
                                                                                  m_CurRow(curRow)
{
    ui->setupUi(this);

    initControl();
    initConnections();
    ui->stackedWidget->setCurrentIndex(page);
}

ConfigOperateWidget::~ConfigOperateWidget()
{
    delete ui;
}

int ConfigOperateWidget::getCurRow()
{
    return m_CurRow;
}

int ConfigOperateWidget::getCurPage()
{
    return ui->stackedWidget->currentIndex();
}

void ConfigOperateWidget::setPage(int page)
{
    ui->stackedWidget->setCurrentIndex(page);
}

void ConfigOperateWidget::initControl()
{
    ui->m_pBtnSave->setText(tr("Save"));
    ui->m_pBtnEdit->setFocusPolicy(Qt::NoFocus);
    ui->m_pBtnEdit->setCursor(Qt::PointingHandCursor);
    ui->m_pBtnEdit->setIcon(QIcon(":/images/edit_item.svg"));
    ui->m_pBtnAdd->setFocusPolicy(Qt::NoFocus);
    ui->m_pBtnAdd->setCursor(Qt::PointingHandCursor);
    ui->m_pBtnAdd->setIcon(QIcon(":/images/add_item.svg"));
    ui->m_pBtnDel->setFocusPolicy(Qt::NoFocus);
    ui->m_pBtnDel->setCursor(Qt::PointingHandCursor);
    ui->m_pBtnDel->setIcon(QIcon(":/images/delete_item.svg"));
}

void ConfigOperateWidget::initConnections()
{
    connect(ui->m_pBtnSave, SIGNAL(clicked()), this, SLOT(sendSave()));
    connect(ui->m_pBtnEdit, SIGNAL(clicked()), this, SLOT(sendEdit()));
    connect(ui->m_pBtnAdd, SIGNAL(clicked()), this, SLOT(sendAdd()));
    connect(ui->m_pBtnDel, SIGNAL(clicked()), this, SLOT(sendDelete()));
}

void ConfigOperateWidget::sendSave()
{
    emit sendSaveSig(this);
}

void ConfigOperateWidget::sendEdit()
{
    ui->stackedWidget->setCurrentWidget(ui->page);
    emit sendEditSig(this);
}

void ConfigOperateWidget::sendAdd()
{
    emit sendAddSig(this);
}

void ConfigOperateWidget::sendDelete()
{
    emit sendDeleteSig(this);
}
