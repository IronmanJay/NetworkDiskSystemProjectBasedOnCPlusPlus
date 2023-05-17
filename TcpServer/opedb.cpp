#include "opedb.h"
#include <QMessageBox>
#include <QDebug>

OpeDB::OpeDB(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");

}

// 生成实例
OpeDB &OpeDB::getInstance()
{
    static OpeDB instance;
    return instance;
}

// 数据库连接初始化
void OpeDB::init()
{
    m_db.setHostName("localhost");
    // 不同的数据库存放的位置，要更改为对应的位置
    m_db.setDatabaseName("D:\\Software\\C++_Code\\NetworkDiskSystem\\TcpServer\\cloud.db");
    if(m_db.open())
    {
        QSqlQuery query;
        query.exec("select * from usrInfo");
        while(query.next())
        {
            QString data = QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug() << data;
        }
    }
    else
    {
        QMessageBox::critical(NULL,"打开数据库","打开数据库失败");
    }
}

// 析构函数用来关闭数据库连接
OpeDB::~OpeDB()
{
    m_db.close();
}

// 处理客户端的用户注册信息
bool OpeDB::handleRegist(const char *name, const char *pwd)
{
    if(NULL == name || NULL == pwd)
    {
        return false;
    }
    QString data = QString("insert into usrInfo(name,pwd) values(\'%1\',\'%2\')").arg(name).arg(pwd);
    QSqlQuery query;
    return query.exec(data);
}

// 处理客户端的用户登录信息
bool OpeDB::handleLogin(const char *name, const char *pwd)
{
    if(NULL == name || NULL == pwd)
    {
        return false;
    }
    QString data = QString("select * from usrInfo where name = \'%1\' and pwd = \'%2\' and online = 0").arg(name).arg(pwd);
    QSqlQuery query;
    query.exec(data);
    if(query.next())
    {
        data = QString("update usrInfo set online = 1 where name = \'%1\' and pwd = \'%2\'").arg(name).arg(pwd);
        QSqlQuery query;
        query.exec(data);
        return true;
    }
    else
    {
        return false;
    }
}

// 处理客户端的用户退出信息
void OpeDB::handleOffline(const char *name)
{
    if(NULL == name)
    {
        return;
    }
    QString data = QString("update usrInfo set online = 0 where name = \'%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
}

// 查询所有在线用户的用户名列表
QStringList OpeDB::handleAllOnline()
{
    QString data = QString("select name from usrInfo where online = 1");
    QSqlQuery query;
    query.exec(data);
    QStringList result;
    result.clear();
    while(query.next())
    {
        result.append(query.value(0).toString());
    }
    return result;
}

// 搜索用户
int OpeDB::handleSearchUsr(const char *name)
{
    if(NULL == name)
    {
        return -1;
    }
    QString data = QString("select online from usrInfo where name = \'%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
    if(query.next())
    {
        int res = query.value(0).toInt();
        if(1 == res)
        {
            return 1;
        }
        else if(0 == res)
        {
            return 0;
        }
    }
    else
    {
        return -1;
    }
}

// 查看当前用户和待添加好友之间的关系
int OpeDB::handleAddFriend(const char *pername, const char *name)
{
    if(NULL == pername || NULL == name)
    {
        return -1;
    }
    QString data = QString("select * from friend where (id = (select id from usrInfo where name=\'%1\') and friendId = (select id from usrInfo where name=\'%2\')) or (id = (select id from usrInfo where name=\'%3\') and friendId = (select id from usrInfo where name=\'%4\'))").arg(pername).arg(name).arg(name).arg(pername);
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);
    if(query.next())
    {
        return 0;
    }
    else
    {
        QString data = QString("select online from usrInfo where name = \'%1\'").arg(pername);
        QSqlQuery query;
        query.exec(data);
        if(query.next())
        {
            int res = query.value(0).toInt();
            if(1 == res)
            {
                return 1;
            }
            else if(0 == res)
            {
                return 2;
            }
        }
        else
        {
            return 3;
        }
    }
}

// 在同意添加好友之后向数据库中写入相关数据
void OpeDB::handleAgreeAddFriend(const char *pername, const char *name)
{
    if (NULL == pername || NULL == name)
    {
        return;
    }
    QString data = QString("insert into friend(id, friendId) values((select id from usrInfo where name=\'%1\'), (select id from usrInfo where name=\'%2\'))").arg(pername).arg(name);
    QSqlQuery query;
    query.exec(data);
}

// 获取当前用户的好友列表
QStringList OpeDB::handleFlushFriend(const char *name)
{
    QStringList strFriendList;
    strFriendList.clear();
    if(NULL == name)
    {
        return strFriendList;
    }
    QString data = QString("select name from usrInfo where online=1 and id in (select id from friend where friendId=(select id from usrInfo where name=\'%1\'))").arg(name);
    QSqlQuery query;
    query.exec(data);
    while(query.next())
    {
        strFriendList.append(query.value(0).toString());
        qDebug() << query.value(0).toString();
    }
    data = QString("select name from usrInfo where online=1 and id in (select friendId from friend where id=(select id from usrInfo where name=\'%1\'))").arg(name);
    query.exec(data);
    while(query.next())
    {
        strFriendList.append(query.value(0).toString());
        qDebug() << query.value(0).toString();
    }
    return strFriendList;
}

// 数据库删除好友操作
bool OpeDB::handleDelFriend(const char *name, const char *friendName)
{
    if(NULL == name || NULL == friendName)
    {
        return false;
    }
    QString data = QString("delete from friend where id = (select id from usrInfo where name = \'%1\') and friendId = (select id from usrInfo where name = \'%2\')").arg(name).arg(friendName);
    QSqlQuery query;
    query.exec(data);
    data = QString("delete from friend where id = (select id from usrInfo where name = \'%1\') and friendId = (select id from usrInfo where name = \'%2\')").arg(friendName).arg(name);
    query.exec(data);
    return true;
}
