#include "opewidget.h"

// 操作主界面UI
OpeWidget::OpeWidget(QWidget *parent) : QWidget(parent)
{
    m_pListW = new QListWidget(this);
    m_pListW->addItem("好友");
    m_pListW->addItem("图书");
    m_pFriend = new Friend;
    m_pBook = new Book;
    m_pSW = new QStackedWidget;
    m_pSW->addWidget(m_pFriend);
    m_pSW->addWidget(m_pBook);
    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pListW);
    pMain->addWidget(m_pSW);
    setLayout(pMain);
    connect(m_pListW,SIGNAL(currentRowChanged(int)),m_pSW,SLOT(setCurrentIndex(int)));
}

// 生成OpeWidget实例对象
OpeWidget &OpeWidget::getInstance()
{
    static OpeWidget instance;
    return instance;
}

// 返回当前好友的操作对象
Friend *OpeWidget::getFriend()
{
    return m_pFriend;
}

// 获取图书对象
Book *OpeWidget::getBook()
{
    return m_pBook;
}
