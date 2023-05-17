#include "mytcpsocket.h"
#include <QDebug>
#include "mytcpserver.h"
#include <QDir>
#include <QFileInfoList>

MyTcpSocket::MyTcpSocket()
{
    connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    connect(this,SIGNAL(disconnected()),this,SLOT(clientOffline()));
    m_bUpload = false;
    m_pTimer = new QTimer;
    connect(m_pTimer, SIGNAL(timeout()),this, SLOT(sendFileToClient()));
}

// 返回当前请求的用户用户名
QString MyTcpSocket::getName()
{
    return m_strName;
}

// 拷贝文件夹
void MyTcpSocket::copyDir(QString strSrcDir, QString strDestDir)
{
    QDir dir;
    dir.mkdir(strDestDir);
    dir.setPath(strSrcDir);
    QFileInfoList fileInfoList = dir.entryInfoList();
    QString srcTemp;
    QString destTemp;
    for(int i = 0;i<fileInfoList.size();i++)
    {
        qDebug() << "filename:" << fileInfoList[i].fileName();
        if(fileInfoList[i].isFile())
        {
            srcTemp = strSrcDir + '/' + fileInfoList[i].fileName();
            destTemp = strDestDir + '/' + fileInfoList[i].fileName();
            QFile::copy(srcTemp,destTemp);
        }
        else if(fileInfoList[i].isDir())
        {
            if(QString(".") == fileInfoList[i].fileName() || QString("..") == fileInfoList[i].fileName())
            {
                continue;
            }
            srcTemp = strSrcDir + '/' + fileInfoList[i].fileName();
            destTemp = strDestDir + '/' + fileInfoList[i].fileName();
            copyDir(srcTemp,destTemp);
        }
    }
}

// 接收来自客户端的消息
void MyTcpSocket::recvMsg()
{
    if(!m_bUpload)
    {
        qDebug() << this->bytesAvailable();
        uint uiPDULen  = 0;
        this->read((char*)&uiPDULen,sizeof(uint));
        uint uiMsgLen = uiPDULen - sizeof(PDU);
        PDU *pdu = mkPDU(uiMsgLen);
        this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
        switch (pdu->uiMsgType)
        {
            // 注册请求
            case ENUM_MSG_TYPE_REGIST_REQUEST:
            {
                char caName[32] = {'\0'};
                char caPwd[32] = {'\0'};
                strncpy(caName,pdu->caData,32);
                strncpy(caPwd,pdu->caData+32,32);
                bool res = OpeDB::getInstance().handleRegist(caName,caPwd);
                PDU *respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
                if(res)
                {
                    strcpy(respdu->caData,REGIST_OK);
                    QDir dir;
                    qDebug() << "create dir:" << dir.mkdir(QString("./%1").arg(caName));
                }
                else
                {
                    strcpy(respdu->caData,REGIST_FAILED);
                }
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            // 登录请求
            case ENUM_MSG_TYPE_LOGIN_REQUEST:
            {
                char caName[32] = {'\0'};
                char caPwd[32] = {'\0'};
                strncpy(caName,pdu->caData,32);
                strncpy(caPwd,pdu->caData+32,32);
                bool res = OpeDB::getInstance().handleLogin(caName,caPwd);
                PDU *respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
                if(res)
                {
                    strcpy(respdu->caData,LOGIN_OK);
                    m_strName= caName;
                }
                else
                {
                    strcpy(respdu->caData,LOGIN_FAILED);
                }
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            // 查看在线用户请求
            case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
            {
                QStringList res = OpeDB::getInstance().handleAllOnline();
                uint uiMsgLen = res.size() * 32;
                PDU *resPdu = mkPDU(uiMsgLen);
                resPdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
                for(int i = 0;i<res.size();i++)
                {
                    memcpy((char*)(resPdu->caMsg) + i * 32,res.at(i).toStdString().c_str(),res.at(i).size());
                }
                write((char*)resPdu,resPdu->uiPDULen);
                free(resPdu);
                resPdu = NULL;
                break;
            }
            // 查看查找用户请求
            case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:
            {
                int res = OpeDB::getInstance().handleSearchUsr(pdu->caData);
                PDU *resPdu = mkPDU(0);
                resPdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
                if(-1 == res)
                {
                    strcpy(resPdu->caData,SEARCH_USR_NO);
                }
                else if(1 == res)
                {
                    strcpy(resPdu->caData,SEARCH_USR_ONLINE);
                }
                else if(0 == res)
                {
                    strcpy(resPdu->caData,SEARCH_USR_OFFLINE);
                }
                write((char*)resPdu,resPdu->uiPDULen);
                free(resPdu);
                resPdu = NULL;
                break;
            }
            // 查看添加好友请求
            case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
            {
                char caPerName[32] = {'\0'};
                char caName[32] = {'\0'};
                strncpy(caPerName,pdu->caData,32);
                strncpy(caName,pdu->caData+32,32);
                int res = OpeDB::getInstance().handleAddFriend(caPerName,caName);
                qDebug() << "当前这个人在不在？" << res;
                PDU *respdu = NULL;
                if(-1== res)
                {
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                    strcpy(respdu->caData,UNKNOW_ERROR);
                    write((char*)respdu,respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;
                }
                else if(0 == res)
                {
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                    strcpy(respdu->caData,EXISTED_FRIEND);
                    write((char*)respdu,respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;
                }
                else if(1 == res)
                {
                    MyTcpServer::getInstance().resend(caPerName,pdu);
                }
                else if(2 == res)
                {
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                    strcpy(respdu->caData,ADD_FRIEND_OFFLINE);
                    write((char*)respdu,respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;
                }
                else if(3 == res)
                {
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                    strcpy(respdu->caData,ADD_FRIEND_NO_EXIST);
                    write((char*)respdu,respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;
                }
                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:
            {
                char caPerName[32] = {'\0'};
                char caName[32] = {'\0'};
                strncpy(caPerName, pdu->caData, 32);
                strncpy(caName, pdu->caData+32, 32);
                OpeDB::getInstance().handleAgreeAddFriend(caPerName, caName);
                MyTcpServer::getInstance().resend(caName, pdu);
                break;
            }
            case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
            {
                char caName[32] = {'\0'};
                strncpy(caName, pdu->caData+32, 32);
                MyTcpServer::getInstance().resend(caName, pdu);
                break;
            }
            // 查看刷新在线好友列表请求
            case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
            {
                char caName[32] = {'\0'};
                strncpy(caName, pdu->caData, 32);
                QStringList res = OpeDB::getInstance().handleFlushFriend(caName);
                uint uiMsglen = res.size() * 32;
                PDU *respdu = mkPDU(uiMsglen);
                respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
                for(int i = 0;i<res.size();i++)
                {
                    memcpy((char*)(respdu->caMsg) + i * 32,res.at(i).toStdString().c_str(),res.at(i).size());
                }
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            // 查看删除好友请求
            case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
            {
                char caSelName[32] = {'\0'};
                char caFriendName[32] = {'\0'};
                strncpy(caSelName,pdu->caData,32);
                strncpy(caFriendName,pdu->caData+32,32);
                OpeDB::getInstance().handleDelFriend(caSelName,caFriendName);
                PDU *respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
                strcpy(respdu->caData,DEL_FRIEND_OK);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                MyTcpServer::getInstance().resend(caFriendName,pdu);
                break;
            }
            // 查看私聊请求
            case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
            {
                char caPerName[32] = {'\0'};
                memcpy(caPerName,pdu->caData + 32,32);
                qDebug() << caPerName;
                MyTcpServer::getInstance().resend(caPerName,pdu);
                break;
            }
            // 查看群聊请求
            case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
            {
                char caName[32] = {'\0'};
                strncpy(caName, pdu->caData, 32);
                QStringList onlineFriend = OpeDB::getInstance().handleFlushFriend(caName);
                QString temp;
                for(int i = 0;i<onlineFriend.size();i++)
                {
                    temp = onlineFriend.at(i);
                    MyTcpServer::getInstance().resend(temp.toStdString().c_str(),pdu);
                }
                break;
            }
            // 查看创建文件夹请求
            case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:
            {
                QDir dir;
                QString strCurPath = QString("%1").arg((char*)(pdu->caMsg));
                bool res = dir.exists(strCurPath);
                PDU *respdu = NULL;
                // 当前目录存在
                if(res)
                {
                    char caNewDir[32] = {'\0'};
                    memcpy(caNewDir,pdu->caData + 32,32);
                    QString strNewPath = strCurPath + "/" + caNewDir;
                    qDebug() << strNewPath;
                    res = dir.exists(strNewPath);
                    // 创建的文件名已存在
                    if(res)
                    {
                        respdu = mkPDU(0);
                        respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                        strcpy(respdu->caData,FILE_NAME_EXIST);
                    }
                    // 创建的文件名不存在
                    else
                    {
                        dir.mkdir(strNewPath);
                        respdu = mkPDU(0);
                        respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                        strcpy(respdu->caData,CREAT_DIR_OK);
                    }
                }
                // 当前目录不存在
                else
                {
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                    strcpy(respdu->caData,DIR_NO_EXIST);
                }
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            // 查看查看所有文件请求
            case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:
            {
                char *pCurPath = new char[pdu->uiMsgLen];
                memcpy(pCurPath,pdu->caMsg,pdu->uiMsgLen);
                QDir dir(pCurPath);
                QFileInfoList fileInfoList = dir.entryInfoList();
                int iFileCount = fileInfoList.size();
                PDU *respdu = mkPDU(sizeof(FileInfo) * iFileCount);
                respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
                FileInfo *pFileInfo = NULL;
                QString strFileName;
                for(int i = 0;i<fileInfoList.size();i++)
                {
                    pFileInfo = (FileInfo*)(respdu->caMsg) + i;
                    strFileName = fileInfoList[i].fileName();
                    memcpy(pFileInfo->caFileName,strFileName.toStdString().c_str(),strFileName.size());
                    if(fileInfoList[i].isDir())
                    {
                        pFileInfo->iFileType = 0;
                    }
                    else if(fileInfoList[i].isFile())
                    {
                        pFileInfo->iFileType = 1;
                    }
                }
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            // 查看删除文件夹请求
            case ENUM_MSG_TYPE_DEL_DIR_REQUEST:
            {
                char caName[32] = {'\0'};
                strcpy(caName,pdu->caData);
                char *pPath = new char[pdu->uiMsgLen];
                memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
                QString strPath = QString("%1/%2").arg(pPath).arg(caName);
                qDebug() << strPath;
                QFileInfo fileInfo(strPath);
                bool res = false;
                // 文件夹
                if(fileInfo.isDir())
                {
                    QDir dir;
                    dir.setPath(strPath);
                    res = dir.removeRecursively();
                }
                // 常规文件
                else if(fileInfo.isFile())
                {
                    res = false;
                }
                PDU *respdu = NULL;
                if(res)
                {
                    respdu = mkPDU(strlen(DEL_DIR_OK) + 1);
                    respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
                    memcpy(respdu->caData,DEL_DIR_OK,strlen(DEL_DIR_OK));
                }
                else
                {
                    respdu = mkPDU((strlen(DEL_DIR_FAILURED)) + 1);
                    respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
                    memcpy(respdu->caData,DEL_DIR_FAILURED,strlen(DEL_DIR_FAILURED));
                }
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            // 查看重命名文件请求
            case ENUM_MSG_TYPE_RENAME_FILE_REQUEST:
            {
                char caOldName[32] = {'\0'};
                char caNewName[32] = {'\0'};
                strncpy(caOldName,pdu->caData,32);
                strncpy(caNewName,pdu->caData + 32,32);
                char *pPath = new char[pdu->uiMsgLen];
                memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
                QString strOldPath = QString("%1/%2").arg(pPath).arg(caOldName);
                QString strNewPath = QString("%1/%2").arg(pPath).arg(caNewName);
                QDir dir;
                bool res = dir.rename(strOldPath,strNewPath);
                PDU *respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
                if(res)
                {
                    strcpy(pdu->caData,RENAME_FILE_OK);
                }
                else
                {
                    strcpy(pdu->caData,RENAME_FILE_FAILURED);
                }
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            // 查看进入文件夹请求
            case ENUM_MSG_TYPE_ENTER_DIR_REQUEST:
            {
                char caEnterName[32] = {'\0'};
                strncpy(caEnterName,pdu->caData,32);
                char *pPath = new char[pdu->uiMsgLen];
                memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
                QString strPath = QString("%1/%2").arg(pPath).arg(caEnterName);
                QFileInfo fileInfo(strPath);
                PDU *respdu = NULL;
                if(fileInfo.isDir())
                {
                    QDir dir(strPath);
                    QFileInfoList fileInfoList = dir.entryInfoList();
                    int iFileCount = fileInfoList.size();
                    respdu = mkPDU(sizeof(FileInfo) * iFileCount);
                    respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
                    FileInfo *pFileInfo = NULL;
                    QString strFileName;
                    for(int i = 0;i<fileInfoList.size();i++)
                    {
                        pFileInfo = (FileInfo*)(respdu->caMsg) + i;
                        strFileName = fileInfoList[i].fileName();
                        memcpy(pFileInfo->caFileName,strFileName.toStdString().c_str(),strFileName.size());
                        if(fileInfoList[i].isDir())
                        {
                            pFileInfo->iFileType = 0;
                        }
                        else if(fileInfoList[i].isFile())
                        {
                            pFileInfo->iFileType = 1;
                        }
                    }
                    write((char*)respdu,respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;
                }
                else if(fileInfo.isFile())
                {
                    respdu = mkPDU(0);
                    respdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
                    strcpy(respdu->caData,ENTER_DIR_FAILURED);
                    write((char*)respdu,respdu->uiPDULen);
                    free(respdu);
                    respdu = NULL;
                }
                break;
            }
            // 查看上传文件请求
            case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:
            {
                char caFileName[32] = {'\0'};
                qint64 fileSize = 0;
                sscanf(pdu->caData,"%s %lld",caFileName,&fileSize);
                char *pPath = new char[pdu->uiMsgLen];
                memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
                QString strPath = QString("%1/%2").arg(pPath).arg(caFileName);
                delete []pPath;
                pPath = NULL;
                m_file.setFileName(strPath);
                // 以只写的方式打开文件，若文件不存在，则会自动创建文件
                if(m_file.open(QIODevice::WriteOnly))
                {
                    m_bUpload = true;
                    m_iTotal = fileSize;
                    m_iRecved = 0;
                }
                break;
            }
            // 查看删除文件请求
            case ENUM_MSG_TYPE_DEL_FILE_REQUEST:
            {
                char caName[32] = {'\0'};
                strcpy(caName,pdu->caData);
                char *pPath = new char[pdu->uiMsgLen];
                memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
                QString strPath = QString("%1/%2").arg(pPath).arg(caName);
                qDebug() << strPath;
                QFileInfo fileInfo(strPath);
                bool res = false;
                // 文件夹
                if(fileInfo.isDir())
                {
                    res = false;
                }
                // 常规文件
                else if(fileInfo.isFile())
                {
                    QDir dir;
                    res = dir.remove(strPath);
                }
                PDU *respdu = NULL;
                if(res)
                {
                    respdu = mkPDU(strlen(DEL_FILE_OK) + 1);
                    respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPOND;
                    memcpy(respdu->caData,DEL_FILE_OK,strlen(DEL_FILE_OK));
                }
                else
                {
                    respdu = mkPDU((strlen(DEL_FILE_FAILURED)) + 1);
                    respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPOND;
                    memcpy(respdu->caData,DEL_FILE_FAILURED,strlen(DEL_FILE_FAILURED));
                }
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            // 查看下载文件请求
            case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:
            {
                char caFileName[32] = {'\0'};
                strcpy(caFileName, pdu->caData);
                char *pPath = new char[pdu->uiMsgLen];
                memcpy(pPath, pdu->caMsg, pdu->uiMsgLen);
                QString strPath = QString("%1/%2").arg(pPath).arg(caFileName);
                qDebug() << strPath;
                delete []pPath;
                pPath = NULL;
                QFileInfo fileInfo(strPath);
                qint64 fileSize = fileInfo.size();
                PDU *respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
                sprintf(respdu->caData, "%s %lld", caFileName, fileSize);
                write((char*)respdu, respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                m_file.setFileName(strPath);
                m_file.open(QIODevice::ReadOnly);
                m_pTimer->start(1000);
                break;
            }
            // 查看分享文件请求
            case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:
            {
                char caSendName[32] = {'\0'};
                int num = 0;
                sscanf(pdu->caData,"%s%d",caSendName,&num);
                int size = num * 32;
                PDU *respdu = mkPDU(pdu->uiMsgLen - size);
                respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE_REQUEST;
                strcpy(respdu->caData,caSendName);
                memcpy(respdu->caMsg,(char*)(pdu->caMsg) + size,pdu->uiMsgLen - size);
                char caRecvName[32] = {'\0'};
                for(int i = 0;i<num;i++)
                {
                    memcpy(caRecvName,(char*)(pdu->caMsg) + i * 32,32);
                    MyTcpServer::getInstance().resend(caRecvName,respdu);
                }
                free(respdu);
                respdu = NULL;
                respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
                strcpy(respdu->caData,"share file ok");
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
                break;
            }
            // 查看分享文件通知回复
            case ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND:
            {
                QString strRecvPath = QString("./%1").arg(pdu->caData);
                QString strShareFilePath = QString("%1").arg((char*)(pdu->caMsg));
                int index = strShareFilePath.lastIndexOf('/');
                QString strFileName = strShareFilePath.right(strShareFilePath.size() - index - 1);
                strRecvPath = strRecvPath + '/' + strFileName;
                QFileInfo fileInfo(strShareFilePath);
                if(fileInfo.isFile())
                {
                    QFile::copy(strShareFilePath,strRecvPath);
                }
                else if(fileInfo.isDir())
                {
                    copyDir(strShareFilePath,strRecvPath);
                }
                break;
            }
            // 查看移动文件请求
            case ENUM_MSG_TYPE_MOVE_FILE_REQUEST:
            {
                char caFileName[32] = {'\0'};
                int srcLen = 0;
                int destLen = 0;
                sscanf(pdu->caData,"%d%d%s",&srcLen,&destLen,caFileName);
                char *pSrcPath = new char[srcLen + 1];
                char *pDestPath = new char[destLen + 1 + 32];
                memset(pSrcPath,'\0',srcLen + 1);
                memset(pDestPath,'\0',destLen + 1 + 32);
                memcpy(pSrcPath,pdu->caMsg,srcLen);
                memcpy(pDestPath,(char*)(pdu->caMsg) + (srcLen + 1),destLen);
                PDU *respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_RESPOND;
                QFileInfo fileInfo(pDestPath);
                if(fileInfo.isDir())
                {
                    strcat(pDestPath,"/");
                    strcat(pDestPath,caFileName);
                    bool res = QFile::rename(pSrcPath,pDestPath);
                    if(res)
                    {
                        strcpy(respdu->caData,MOVE_FILE_OK);
                    }
                    else
                    {
                        strcpy(respdu->caData,COMMON_ERR);
                    }
                }
                else if(fileInfo.isFile())
                {
                    strcpy(respdu->caData,MOVE_FILE_FAILURED);
                }
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
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
        PDU *respdu = NULL;
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
        QByteArray buff = readAll();
        m_file.write(buff);
        m_iRecved += buff.size();
        if(m_iTotal == m_iRecved)
        {
            m_file.close();
            m_bUpload = false;
            strcpy(respdu->caData,UPLOAD_FILE_OK);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }
        else if(m_iTotal < m_iRecved)
        {
            m_file.close();
            m_bUpload = false;
            strcpy(respdu->caData,UPLOAD_FILE_FAILURED);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }
    }
}

// 处理客户端用户下线的信号
void MyTcpSocket::clientOffline()
{
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());
    emit offline(this);
}

// 向客户端发送文件数据
void MyTcpSocket::sendFileToClient()
{
    char *pData = new char[4096];
    qint64 res = 0;
    while(true)
    {
        res = m_file.read(pData,4096);
        if(res > 0 && res <= 4096)
        {
            write(pData,res);
        }
        else if(0 == res)
        {
            m_file.close();
            break;
        }
        else if(0 > res)
        {
            qDebug() << "发送文件内容给客户端过程中失败";
            m_file.close();
            break;
        }
    }
    delete []pData;
    pData = NULL;
}
