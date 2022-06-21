#include "node-operate-dialog.h"
#include <widget-property-helper.h>
#include <QRegExpValidator>
#include "common/message-dialog.h"
#include "def.h"
#include "ui_node-operate-dialog.h"
NodeOperateDialog::NodeOperateDialog(NodeOperateType type, QWidget *parent) : KiranTitlebarWindow(parent),
                                                                              ui(new Ui::NodeOperateDialog),
                                                                              m_type(type)
{
    ui->setupUi(getWindowContentWidget());
    setAttribute(Qt::WA_DeleteOnClose);
    setIcon(QIcon(":/images/logo.png"));
    setTitle(tr("Add Node"));
    ui->lineEdit_node_name->setFocus();
    ui->lineEdit_node_name->setMaxLength(50);
    ui->lineEdit_node_name->setPlaceholderText(tr("Please input 1 to 50 characters"));
    ui->text_comment->setPlaceholderText(tr("Please input 0 to 200 characters"));

    if (type == NODE_OPERATE_TYPE_CREATE)
        setWindowTitle(tr("Add Node"));
    else
    {
        setWindowTitle(tr("Update Node"));
        ui->lineEdit_node_ip->setDisabled(true);
    }

    QRegExp rx("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    ui->lineEdit_node_ip->setValidator(new QRegExpValidator(rx, parent));

    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_save, Kiran::BUTTON_Default);
    Kiran::WidgetPropertyHelper::setButtonType(ui->btn_cancel, Kiran::BUTTON_Normal);
    connect(ui->btn_save, &QPushButton::clicked, this, &NodeOperateDialog::onSave);
    connect(ui->btn_cancel, &QPushButton::clicked, this, &NodeOperateDialog::onCancel);

    connect(ui->text_comment, &QTextEdit::textChanged, this, &NodeOperateDialog::limitLength);
}

NodeOperateDialog::~NodeOperateDialog()
{
    delete ui;
}

void NodeOperateDialog::setNodeInfo(int64_t nodeId, QString name, QString addr, QString comment)
{
    m_nodeId = nodeId;
    ui->lineEdit_node_name->setText(name);
    ui->lineEdit_node_ip->setText(addr);
    ui->text_comment->setText(comment);
}

NodeOperateType NodeOperateDialog::getType()
{
    return m_type;
}

int64_t NodeOperateDialog::getNodeId()
{
    return m_nodeId;
}

void NodeOperateDialog::onSave()
{
    QString name = ui->lineEdit_node_name->text();
    QString ip = ui->lineEdit_node_ip->text();
    QString comment = ui->text_comment->toPlainText();
    if (name.isEmpty() || ip.isEmpty())
    {
        MessageDialog::message(tr("Add node"),
                               tr("Add node failed!"),
                               tr("Please improve the content!"),
                               ":/images/warning.svg",
                               MessageDialog::StandardButton::Ok);
        return;
    }
    QMap<QString, QString> newNodeInfo;
    newNodeInfo.insert(NODE_NAME, name);
    newNodeInfo.insert(NODE_ADDRESS, ip);
    newNodeInfo.insert(NODE_COMMENT, comment);
    emit sigSave(newNodeInfo);
    this->close();
}

void NodeOperateDialog::onCancel()
{
    this->close();
}

void NodeOperateDialog::limitLength()
{
    QString textContent = ui->text_comment->toPlainText();
    int length = textContent.count();
    int maxLength = 200;  // 最大字符数
    if (length > maxLength)
    {
        int position = ui->text_comment->textCursor().position();
        QTextCursor textCursor = ui->text_comment->textCursor();
        textContent.remove(position - (length - maxLength), length - maxLength);
        ui->text_comment->setText(textContent);
        textCursor.setPosition(position - (length - maxLength));
        ui->text_comment->setTextCursor(textCursor);
    }
}
