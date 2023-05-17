#include "online.h"
#include "ui_online.h"
#include <QDebug>
#include "tcpclient.h"

Online::Online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

// 将所有的在线用户的用户名显示在UI界面上
void Online::showUsr(PDU *pdu)
{
    if(NULL == pdu)
    {
        return;
    }
    uint uiSize = pdu->uiMsgLen / 32;
    char caTmp[32];
    for(uint i = 0;i < uiSize;i++)
    {
        memcpy(caTmp,(char*)pdu->caMsg + i * 32,32);
        ui->online_lw->addItem(caTmp);
    }
}

// 加好友功能实现
void Online::on_addFriend_pb_clicked()
{
    QListWidgetItem *pItem = ui->online_lw->currentItem();
    QString strPerUsrName = pItem->text();
    QString strLoginName = TcpClient::getInstance().loginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData,strPerUsrName.toStdString().c_str(),strPerUsrName.size());
    memcpy(pdu->caData + 32,strLoginName.toStdString().c_str(),strLoginName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}
