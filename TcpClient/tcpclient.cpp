#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include "opewidget.h"
#include "privatechat.h"

TcpClient::TcpClient(QWidget *parent) : QWidget(parent), ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();
    connect(&m_tcpSockey,SIGNAL(connected()),this,SLOT(showConnect()));
    connect(&m_tcpSockey,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    // 连接服务器
    m_tcpSockey.connectToHost(QHostAddress(m_strIP),m_usPort);
}

TcpClient::~TcpClient()
{
    delete ui;
}

// 加载配置文件
void TcpClient::loadConfig()
{
    QFile file(":/client.config");
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
        strData.replace("\r\n"," ");
        QStringList strList = strData.split(" ");
        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        qDebug() << "IP地址为：" << m_strIP << "端口为：" << m_usPort;
        file.close();
    }
    else
    {
        QMessageBox::critical(this,"open config","open config failed");
    }
}


// 返回TcpClient实例对象
TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

// 获取TcpSocket
QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSockey;
}

// 获取当前登录用户的用户名
QString TcpClient::loginName()
{
    return m_strLoginName;
}

// 返回当前文件夹目录
QString TcpClient::curPath()
{
    return m_strCurPath;
}

// 设置当前目录位置
void TcpClient::setCurPath(QString strCurPath)
{
    m_strCurPath = strCurPath;
}

// 获取和服务器连接结果
void TcpClient::showConnect()
{
    QMessageBox::information(this,"连接服务器","连接服务器成功");
}

# if 0
// 发送点击事件
void TcpClient::on_send_pd_clicked()
{
    QString strMsg = ui->lineEdit->text();
    if(!strMsg.isEmpty())
    {
        PDU *pdu = mkPDU(strMsg.size());
        pdu->uiMsgType = 8888;
        memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());
        m_tcpSockey.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this,"信息发送","发送的信息不能为空");
    }
}
# endif

// 登录事件
void TcpClient::on_login_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isNull() && !strPwd.isNull())
    {
        m_strLoginName = strName;
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData + 32,strName.toStdString().c_str(),32);
        m_tcpSockey.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this,"登录","登录失败：用户名或密码为空");
    }
}

// 注册事件
void TcpClient::on_regist_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isNull() && !strPwd.isNull())
    {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData + 32,strName.toStdString().c_str(),32);
        m_tcpSockey.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this,"注册","注册失败：用户名或密码为空");
    }
}

// 注销事件
void TcpClient::on_cancel_pb_clicked()
{

}

// 接收服务器信息
void TcpClient::recvMsg()
{
    if(!OpeWidget::getInstance().getBook()->getDownloadStatus())
    {
        qDebug() << m_tcpSockey.bytesAvailable();
        uint uiPDULen = 0;
        m_tcpSockey.read((char*)&uiPDULen,sizeof(uint));
        uint uiMsgLen = uiPDULen - sizeof(PDU);
        PDU *pdu = mkPDU(uiMsgLen);
        m_tcpSockey.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
        switch (pdu->uiMsgType)
        {
            // 注册回复
            case ENUM_MSG_TYPE_REGIST_RESPOND:
            {
                if(0 == strcmp(pdu->caData,REGIST_OK))
                {
                    QMessageBox::information(this,"注册",REGIST_OK);
                }
                else if(0 == strcmp(pdu->caData,REGIST_FAILED))
                {
                    QMessageBox::warning(this,"注册",REGIST_FAILED);
                }
                break;
            }
            // 登录回复
            case ENUM_MSG_TYPE_LOGIN_RESPOND:
            {
                if(0 == strcmp(pdu->caData,LOGIN_OK))
                {
                    m_strCurPath = QString("./%1").arg(m_strLoginName);
                    QMessageBox::information(this,"登录",LOGIN_OK);
                    OpeWidget::getInstance().show();
                    this->hide();
                }
                else if(0 == strcmp(pdu->caData,LOGIN_FAILED))
                {
                    QMessageBox::warning(this,"登录",LOGIN_FAILED);
                }
                break;
            }
            // 查看在线用户回复
            case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:
            {
                OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);
                break;
            }
            // 查看查找用户回复
            case ENUM_MSG_TYPE_SEARCH_USR_RESPOND:
            {
                if(0 == strcmp(SEARCH_USR_NO,pdu->caData))
                {
                    QMessageBox::information(this, "搜索", QString("%1: not exist").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
                }
                else if(0 == strcmp(SEARCH_USR_ONLINE,pdu->caData))
                {
                    QMessageBox::information(this,"搜索",QString("%1: online").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
                }
                else if(0 == strcmp(SEARCH_USR_OFFLINE,pdu->caData))
                {
                    QMessageBox::information(this,"搜索",QString("%1: offline").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
                }
                break;
            }
            // 查看添加好友回复
            case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
            {
                char caName[32] = {'\0'};
                strncpy(caName,pdu->caData+32,32);
                qDebug() << "添加好友";
                int res = QMessageBox::information(this,"添加好友",QString("%1 wang to add you as friend").arg(caName),QMessageBox::Yes,QMessageBox::No);
                qDebug() << "结果为：" << res;
                PDU *respdu = mkPDU(0);
                memcpy(respdu->caData,pdu->caData,64);
                if(res == QMessageBox::Yes)
                {
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGGREE;
                }
                else
                {
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
                }
                m_tcpSockey.write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
            {
                QMessageBox::information(this,"添加好友",pdu->caData);
                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:
            {
                char caPerName[32] = {'\0'};
                memcpy(caPerName, pdu->caData, 32);
                QMessageBox::information(this, "添加好友", QString("添加%1好友成功").arg(caPerName));
                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
            {
                char caPerName[32] = {'\0'};
                memcpy(caPerName, pdu->caData, 32);
                QMessageBox::information(this, "添加好友", QString("添加%1好友失败").arg(caPerName));
                break;
            }
            // 查看刷新在线好友列表回复
            case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:
            {
                OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
                break;
            }
            // 查看删除好友回复
            case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
            {
                char caName[32] = {'\0'};
                memcpy(caName,pdu->caData,32);
                QMessageBox::information(this,"删除好友",QString("%1删除你作为他的好友").arg(caName));
                break;
            }
            case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:
            {
                QMessageBox::information(this,"删除好友","删除好友成功");
                break;
            }
            // 查看私聊回复
            case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
            {
                if(PrivateChat::getInstance().isHidden())
                {
                    PrivateChat::getInstance().show();
                }
                char caSendName[32] = {'\0'};
                memcpy(caSendName,pdu->caData,32);
                QString strSendName = caSendName;
                PrivateChat::getInstance().setChatName(caSendName);
                PrivateChat::getInstance().updateMsg(pdu);
                break;
            }
            // 查看群聊回复
            case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
            {
                OpeWidget::getInstance().getFriend()->updateGroupMsg(pdu);
                break;
            }
            // 查看创建文件夹回复
            case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:
            {
                QMessageBox::information(this,"创建文件",pdu->caData);
                break;
            }
            // 查看查看所有文件回复
            case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:
            {
                OpeWidget::getInstance().getBook()->updateFileList(pdu);
                QString strEnterDir = OpeWidget::getInstance().getBook()->enterDir();
                if(!strEnterDir.isEmpty())
                {
                    m_strCurPath = m_strCurPath + "/" + strEnterDir;
                }
                break;
            }
            // 查看删除文件夹回复
            case ENUM_MSG_TYPE_DEL_DIR_RESPOND:
            {
                QMessageBox::information(this,"删除文件夹",pdu->caData);
                break;
            }
            // 查看重命名文件回复
            case ENUM_MSG_TYPE_RENAME_FILE_RESPOND:
            {
                QMessageBox::information(this,"重命名文件",pdu->caData);
                break;
            }
            // 查看进入文件夹回复
            case ENUM_MSG_TYPE_ENTER_DIR_RESPOND:
            {
                OpeWidget::getInstance().getBook()->clearEnterDir();
                QMessageBox::information(this,"进入文件夹",pdu->caData);
                break;
            }
            // 查看上传文件回复
            case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:
            {
                QMessageBox::information(this,"上传文件",pdu->caData);
                break;
            }
            // 查看删除文件回复
            case ENUM_MSG_TYPE_DEL_FILE_RESPOND:
            {
                QMessageBox::information(this,"删除文件",pdu->caData);
                break;
            }
            // 查看下载文件回复
            case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND:
            {
                qDebug() << pdu->caData;
                char caFileName[32] = {'\0'};
                sscanf(pdu->caData,"%s %lld",caFileName,&(OpeWidget::getInstance().getBook()->m_iTotal));
                if(strlen(caFileName) > 0 && OpeWidget::getInstance().getBook()->m_iTotal > 0)
                {
                    OpeWidget::getInstance().getBook()->setDownloadStatus(true);
                    m_file.setFileName(OpeWidget::getInstance().getBook()->getSaveFilePath());
                    if(!m_file.open(QIODevice::WriteOnly))
                    {
                        QMessageBox::warning(this,"下载文件","获得保存文件的路径失败");
                    }
                }
                break;
            }
            // 查看分享文件回复
            case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:
            {
                QMessageBox::information(this,"分享文件",pdu->caData);
                break;
            }
            // 查看分享文件通知请求
            case ENUM_MSG_TYPE_SHARE_FILE_NOTE_REQUEST:
            {
                char *pPath = new char[pdu->uiMsgLen];
                memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
                char *pos = strrchr(pPath,'/');
                if(NULL != pos)
                {
                    pos++;
                    QString strNote = QString("%1 share file->%2 \n Do you accept?").arg(pdu->caData).arg(pos);
                    int res = QMessageBox::question(this,"分享文件",strNote);
                    if(QMessageBox::Yes == res)
                    {
                        PDU *respdu = mkPDU(pdu->uiMsgLen);
                        respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND;
                        memcpy(respdu->caMsg,pdu->caMsg,pdu->uiMsgLen);
                        QString strName = TcpClient::getInstance().loginName();
                        strcpy(respdu->caData,strName.toStdString().c_str());
                        m_tcpSockey.write((char*)respdu,respdu->uiPDULen);
                    }
                }
                break;
            }
            // 查看移动文件回复
            case ENUM_MSG_TYPE_MOVE_FILE_RESPOND:
            {
                QMessageBox::information(this,"移动文件",pdu->caData);
                break;
            }
            default:
            {
                break;
            }
        }
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QByteArray buffer = m_tcpSockey.readAll();
        m_file.write(buffer);
        Book *pBook = OpeWidget::getInstance().getBook();
        pBook->m_iRecved += buffer.size();
        if (pBook->m_iTotal == pBook->m_iRecved)
        {
            m_file.close();
            pBook->m_iTotal = 0;
            pBook->m_iRecved = 0;
            pBook->setDownloadStatus(false);
            QMessageBox::information(this, "下载文件", "下载文件成功");
        }
        else if (pBook->m_iTotal < pBook->m_iRecved)
        {
            m_file.close();
            pBook->m_iTotal = 0;
            pBook->m_iRecved = 0;
            pBook->setDownloadStatus(false);
            QMessageBox::critical(this, "下载文件", "下载文件失败");
        }
    }
}
