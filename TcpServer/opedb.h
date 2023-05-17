#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>

class OpeDB : public QObject
{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);
    static OpeDB& getInstance();
    void init();
    ~OpeDB();
    bool handleRegist(const char *name,const char *pwd);
    bool handleLogin(const char *name,const char *pwd);
    void handleOffline(const char *name);
    QStringList handleAllOnline();
    int handleSearchUsr(const char *name);
    int handleAddFriend(const char *pername,const char *name);
    void handleAgreeAddFriend(const char *pername, const char *name);
    QStringList handleFlushFriend(const char *name);
    bool handleDelFriend(const char *name, const char *friendName);
signals:

public slots:

private:
    QSqlDatabase m_db; // 连接数据库

};

#endif // OPEDB_H
