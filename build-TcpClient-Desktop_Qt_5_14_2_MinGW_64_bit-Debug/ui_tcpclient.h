/********************************************************************************
** Form generated from reading UI file 'tcpclient.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPCLIENT_H
#define UI_TCPCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TcpClient
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *name_lab;
    QLineEdit *name_le;
    QHBoxLayout *horizontalLayout_2;
    QLabel *pwd_lab_2;
    QLineEdit *pwd_le;
    QPushButton *login_pb;
    QPushButton *regist_pb;
    QPushButton *cancel_pb;

    void setupUi(QWidget *TcpClient)
    {
        if (TcpClient->objectName().isEmpty())
            TcpClient->setObjectName(QString::fromUtf8("TcpClient"));
        TcpClient->resize(543, 356);
        QFont font;
        font.setFamily(QString::fromUtf8("Adobe \345\256\213\344\275\223 Std L"));
        font.setPointSize(24);
        TcpClient->setFont(font);
        layoutWidget = new QWidget(TcpClient);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 511, 321));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        name_lab = new QLabel(layoutWidget);
        name_lab->setObjectName(QString::fromUtf8("name_lab"));
        name_lab->setFont(font);

        horizontalLayout->addWidget(name_lab);

        name_le = new QLineEdit(layoutWidget);
        name_le->setObjectName(QString::fromUtf8("name_le"));
        QFont font1;
        font1.setPointSize(20);
        name_le->setFont(font1);

        horizontalLayout->addWidget(name_le);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        pwd_lab_2 = new QLabel(layoutWidget);
        pwd_lab_2->setObjectName(QString::fromUtf8("pwd_lab_2"));
        QFont font2;
        font2.setPointSize(24);
        pwd_lab_2->setFont(font2);

        horizontalLayout_2->addWidget(pwd_lab_2);

        pwd_le = new QLineEdit(layoutWidget);
        pwd_le->setObjectName(QString::fromUtf8("pwd_le"));
        pwd_le->setFont(font1);
        pwd_le->setEchoMode(QLineEdit::Password);

        horizontalLayout_2->addWidget(pwd_le);


        verticalLayout->addLayout(horizontalLayout_2);

        login_pb = new QPushButton(layoutWidget);
        login_pb->setObjectName(QString::fromUtf8("login_pb"));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Adobe \345\256\213\344\275\223 Std L"));
        font3.setPointSize(20);
        login_pb->setFont(font3);

        verticalLayout->addWidget(login_pb);

        regist_pb = new QPushButton(layoutWidget);
        regist_pb->setObjectName(QString::fromUtf8("regist_pb"));
        regist_pb->setFont(font3);

        verticalLayout->addWidget(regist_pb);

        cancel_pb = new QPushButton(layoutWidget);
        cancel_pb->setObjectName(QString::fromUtf8("cancel_pb"));
        cancel_pb->setFont(font3);

        verticalLayout->addWidget(cancel_pb);


        retranslateUi(TcpClient);

        QMetaObject::connectSlotsByName(TcpClient);
    } // setupUi

    void retranslateUi(QWidget *TcpClient)
    {
        TcpClient->setWindowTitle(QCoreApplication::translate("TcpClient", "TcpClient", nullptr));
        name_lab->setText(QCoreApplication::translate("TcpClient", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        pwd_lab_2->setText(QCoreApplication::translate("TcpClient", "\345\257\206     \347\240\201\357\274\232", nullptr));
        login_pb->setText(QCoreApplication::translate("TcpClient", "\347\231\273\345\275\225", nullptr));
        regist_pb->setText(QCoreApplication::translate("TcpClient", "\346\263\250\345\206\214", nullptr));
        cancel_pb->setText(QCoreApplication::translate("TcpClient", "\346\263\250\351\224\200", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TcpClient: public Ui_TcpClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPCLIENT_H
