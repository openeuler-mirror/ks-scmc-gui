/**
 * @file          /ks-scmc-gui/src/common/config-view.cpp
 * @brief         
 * @author        yuanxing <yuanxing@kylinos.com>
 * @copyright (c) 2022 KylinSec. All rights reserved.
 */
#include "config-view.h"
#include <kiran-log/qt5-log-i.h>
#include "common/message-dialog.h"
#include "config-group.h"
#include "config-operate-widget.h"

ConfigView::ConfigView(Qt::Orientation orientation, QWidget *parent) : QHeaderView(orientation, parent)
{
}

ConfigDelegate::ConfigDelegate(ConfigTableType whichTable, QWidget *parent) : QStyledItemDelegate(parent),
                                                                              m_nWidth(25),
                                                                              m_nHeight(25),
                                                                              m_ChooseTable(whichTable)
{
    m_listPage.reserve(10);
    m_listMode << tr("ReadWrite")
               << tr("ReadOnly");
}

ConfigDelegate::~ConfigDelegate()
{
    dealMemberVar();
}

QWidget *ConfigDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    const int &row = index.row();
    int curPage = 0;
    if (m_listPage.size() > row)
        curPage = m_listPage[row];

    if (index.column() == FIRST_COLUMN_HEADER || index.column() == SECOND_COLUMN_HEADER)
    {
        QSharedPointer<QLineEdit> pEdit(new QLineEdit(parent));
        pEdit->setFocus();
        if (index.column() == FIRST_COLUMN_HEADER)
            m_pEditFirst.append(pEdit);
        else
            m_pEditSecond.append(pEdit);
        if (curPage)
            pEdit->setDisabled(true);
        pEdit->setStyleSheet("QLineEdit{border:1px solid #E4E7ED;}");
        return pEdit.get();
    }
    else if (index.column() == THIRD_COLUMN_HEADER)
    {
        QSharedPointer<QComboBox> pBox(new QComboBox(parent));
        for (int i = 0; i < m_listMode.count(); ++i)
        {
            pBox->addItem(m_listMode[i]);
        }
        pBox->setItemDelegate(new QStyledItemDelegate(parent));
        m_pComboBoxMode.append(pBox);
        if (curPage)
            pBox->setDisabled(true);
        return pBox.get();
    }
    else if (index.column() == OPERATE_HEADER)
    {
        ConfigOperateWidget *pWidget = new ConfigOperateWidget(row, curPage, parent);
        dealOperateSig(pWidget);
        //pWidget->setFocus();
        return pWidget;
    }

    return nullptr;
}

void ConfigDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    Q_UNUSED(editor);
    int Curcolumn = index.column();
    if (Curcolumn == FIRST_COLUMN_HEADER || Curcolumn == SECOND_COLUMN_HEADER)
    {
        QLineEdit *pEdit = static_cast<QLineEdit *>(editor);
        pEdit->setText(index.model()->data(index, Qt::EditRole).toString());
    }
    else if (Curcolumn == THIRD_COLUMN_HEADER)
    {
        int value = index.model()->data(index, Qt::EditRole).toInt();
        QComboBox *pBox = static_cast<QComboBox *>(editor);
        pBox->setCurrentIndex(value);
    }
}

void ConfigDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    int Curcolumn = index.column();
    if (Curcolumn == FIRST_COLUMN_HEADER || Curcolumn == SECOND_COLUMN_HEADER)
    {
        QLineEdit *pEdit = static_cast<QLineEdit *>(editor);
        model->setData(index, pEdit->text(), Qt::EditRole);
    }
    else if (Curcolumn == THIRD_COLUMN_HEADER)
    {
        QComboBox *pBox = static_cast<QComboBox *>(editor);
        model->setData(index, pBox->currentIndex(), Qt::EditRole);
    }
}

void ConfigDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

void ConfigDelegate::dealOperateSig(const ConfigOperateWidget *pWidget) const
{
    connect(pWidget, SIGNAL(sendSaveSig(ConfigOperateWidget *)), this, SLOT(sendSaveSlot(ConfigOperateWidget *)));
    connect(pWidget, SIGNAL(sendEditSig(ConfigOperateWidget *)), this, SLOT(sendEditSlot(ConfigOperateWidget *)));
    connect(pWidget, SIGNAL(sendAddSig(ConfigOperateWidget *)), this, SLOT(sendAddSlot(ConfigOperateWidget *)));
    connect(pWidget, SIGNAL(sendDeleteSig(ConfigOperateWidget *)), this, SLOT(sendDeleteSlot(ConfigOperateWidget *)));
}

void ConfigDelegate::dealMemberVar()
{
    foreach (QSharedPointer<QLineEdit> pEdit, m_pEditFirst)
    {
        if (pEdit)
        {
            m_pEditFirst.removeOne(pEdit);
        }
    }

    foreach (QSharedPointer<QLineEdit> pEdit, m_pEditSecond)
    {
        if (pEdit)
        {
            m_pEditSecond.removeOne(pEdit);
        }
    }

    foreach (QSharedPointer<QComboBox> pBox, m_pComboBoxMode)
    {
        if (pBox)
        {
            m_pComboBoxMode.removeOne(pBox);
        }
    }
}

void ConfigDelegate::sendSaveSlot(ConfigOperateWidget *pCurWidget)
{
    int row = pCurWidget->getCurRow();
    if (m_pEditFirst.size() <= row || m_pEditSecond.size() <= row)
        return;

    if (m_pEditFirst[row]->text().isEmpty() && !m_pEditSecond[row]->text().isEmpty())
    {
        QString str = tr("Please enter variable name");
        if (CONFIG_TABLE_TYPE_VOLUMES == m_ChooseTable)
            str = tr("Please enter container path");
        MessageDialog::message(tr("Save Data"),
                               tr("Data is empty!"),
                               str,
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }
    if (CONFIG_TABLE_TYPE_VOLUMES == m_ChooseTable && m_pEditSecond[row]->text().isEmpty() && !m_pEditFirst[row]->text().isEmpty())
    {
        QString str = tr("Please enter host path");
        MessageDialog::message(tr("Save Data"),
                               tr("Data is empty!"),
                               str,
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }

    pCurWidget->setPage(1);
    if (m_pEditFirst.size() > row)
        m_pEditFirst[row]->setDisabled(true);
    if (m_pEditSecond.size() > row)
        m_pEditSecond[row]->setDisabled(true);
    if (m_pEditSecond.size() > row)
        m_pComboBoxMode[row]->setDisabled(true);

    if (m_listPage.size() <= row)
        m_listPage.insert(row, pCurWidget->getCurPage());
    else
        m_listPage[row] = pCurWidget->getCurPage();

    emit sendSaveSig(row);
}

void ConfigDelegate::sendEditSlot(ConfigOperateWidget *pCurWidget)
{
    int row = pCurWidget->getCurRow();
    KLOG_DEBUG() << __func__ << __LINE__ << ", row:" << row << ", var:" << m_pEditFirst.size() << ", val:" << m_pEditSecond.size();

    if (m_pEditFirst.size() > row)
        m_pEditFirst[row]->setDisabled(false);

    if (m_pEditSecond.size() > row)
        m_pEditSecond[row]->setDisabled(false);

    if (m_pComboBoxMode.size() > row)
        m_pComboBoxMode[row]->setDisabled(false);

    if (m_listPage.size() <= row)
        m_listPage.insert(row, pCurWidget->getCurPage());
    else
        m_listPage.replace(row, pCurWidget->getCurPage());

    emit sendEditSig(row);
}

void ConfigDelegate::sendAddSlot(ConfigOperateWidget *pCurWidget)
{
    int row = pCurWidget->getCurRow();
    dealMemberVar();
    m_listPage.insert(row + 1, 0);
    emit sendAddSig(row);
}

void ConfigDelegate::sendDeleteSlot(ConfigOperateWidget *pCurWidget)
{
    int row = pCurWidget->getCurRow();
    if (m_listPage.size() == 1 || m_pEditFirst.size() == 1 || m_pComboBoxMode.size() == 1)
        return;

    dealMemberVar();
    m_listPage.removeAt(row);
    emit sendDeleteSig(row);
}
