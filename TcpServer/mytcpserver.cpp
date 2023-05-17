#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer()
{

}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

// 接收客户端的请求
void MyTcpServer::incomingConnection(qintptr handle)
{
    qDebug() << "new client connected";
    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(handle);
    m_tcpSocketList.append(pTcpSocket);
    connect(pTcpSocket,SIGNAL(offline(MyTcpSocket*)),this,SLOT(deleteSocket(MyTcpSocket*)));
}

// 将添加好友的请求转发给客户端
void MyTcpServer::resend(const char *pername, PDU *pdu)
{
    if(NULL == pername || NULL == pdu)
    {
        return;
    }
    QString strName = pername;
    for(int i = 0;i<m_tcpSocketList.size();i++)
    {
        if(strName == m_tcpSocketList.at(i)->getName())
        {
            qDebug() << "待添加用户的用户名为：" << strName;
            m_tcpSocketList.at(i)->write((char*)pdu,pdu->uiPDULen);
            break;
        }
    }
}

// 删除当前登录用户的socket
void MyTcpServer::deleteSocket(MyTcpSocket *mysocket)
{
    QList<MyTcpSocket*>::iterator iter = m_tcpSocketList.begin();
    for(;iter!=m_tcpSocketList.end();iter++)
    {
        if(mysocket == *iter)
        {
            delete *iter;
            *iter = NULL;
            m_tcpSocketList.erase(iter);
            break;
        }
    }
}
