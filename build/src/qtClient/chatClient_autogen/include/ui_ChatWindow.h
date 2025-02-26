/********************************************************************************
** Form generated from reading UI file 'ChatWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATWINDOW_H
#define UI_CHATWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centraWidget;
    QWidget *topWidget;
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer_3;
    QLabel *curUserLab;
    QSpacerItem *horizontalSpacer_2;
    QLabel *curChatLab;
    QSpacerItem *horizontalSpacer;
    QWidget *widget;
    QPushButton *pushButton;
    QWidget *chatWidget;
    QGridLayout *gridLayout_2;
    QListWidget *chatMsgListWidget;
    QTextEdit *chatMsgSendEdit;
    QWidget *rightWidget;
    QVBoxLayout *verticalLayout;
    QWidget *friendList;
    QWidget *friendTop;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *addFriend;
    QListWidget *friendMess;
    QWidget *groupList;
    QWidget *groupTop;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_6;
    QPushButton *addGroup;
    QListWidget *groupMess;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpacerItem *horizontalSpacer_7;
    QPushButton *Send;

    void setupUi(QWidget *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(829, 534);
        centraWidget = new QWidget(MainWindow);
        centraWidget->setObjectName("centraWidget");
        centraWidget->setGeometry(QRect(0, 0, 811, 516));
        centraWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(246, 245, 244);"));
        topWidget = new QWidget(centraWidget);
        topWidget->setObjectName("topWidget");
        topWidget->setGeometry(QRect(0, 0, 811, 96));
        topWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(72, 73, 77);\n"
"border-top-left-radius: 10px;\n"
"border-top-right-radius: 10px;"));
        gridLayout = new QGridLayout(topWidget);
        gridLayout->setObjectName("gridLayout");
        horizontalSpacer_3 = new QSpacerItem(258, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 2, 2, 2, 1);

        curUserLab = new QLabel(topWidget);
        curUserLab->setObjectName("curUserLab");
        curUserLab->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"font: 700 italic 11pt \"Yrsa\";"));

        gridLayout->addWidget(curUserLab, 0, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(268, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 3, 0, 1, 1);

        curChatLab = new QLabel(topWidget);
        curChatLab->setObjectName("curChatLab");
        curChatLab->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"font: 700 italic 11pt \"Yrsa\";"));
        curChatLab->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout->addWidget(curChatLab, 2, 1, 2, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 1, 1, 1);

        widget = new QWidget(topWidget);
        widget->setObjectName("widget");
        pushButton = new QPushButton(widget);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(210, 0, 31, 21));

        gridLayout->addWidget(widget, 0, 2, 1, 1);

        chatWidget = new QWidget(centraWidget);
        chatWidget->setObjectName("chatWidget");
        chatWidget->setGeometry(QRect(0, 96, 575, 358));
        gridLayout_2 = new QGridLayout(chatWidget);
        gridLayout_2->setObjectName("gridLayout_2");
        chatMsgListWidget = new QListWidget(chatWidget);
        chatMsgListWidget->setObjectName("chatMsgListWidget");

        gridLayout_2->addWidget(chatMsgListWidget, 0, 0, 1, 1);

        chatMsgSendEdit = new QTextEdit(chatWidget);
        chatMsgSendEdit->setObjectName("chatMsgSendEdit");

        gridLayout_2->addWidget(chatMsgSendEdit, 1, 0, 1, 1);

        rightWidget = new QWidget(centraWidget);
        rightWidget->setObjectName("rightWidget");
        rightWidget->setGeometry(QRect(581, 96, 230, 420));
        verticalLayout = new QVBoxLayout(rightWidget);
        verticalLayout->setObjectName("verticalLayout");
        friendList = new QWidget(rightWidget);
        friendList->setObjectName("friendList");
        friendTop = new QWidget(friendList);
        friendTop->setObjectName("friendTop");
        friendTop->setGeometry(QRect(10, 0, 211, 41));
        horizontalLayout_2 = new QHBoxLayout(friendTop);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalSpacer_5 = new QSpacerItem(124, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_5);

        addFriend = new QPushButton(friendTop);
        addFriend->setObjectName("addFriend");
        addFriend->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"	width: 60px;\n"
"	height: 30px;\n"
"	border: none;\n"
"	background-color: rgb(20, 180, 206);\n"
"	color: rgb(255, 255, 255);\n"
"	font: 700 italic 11pt \"Yrsa\";\n"
"	border-radius: 5px;\n"
"}\n"
""));

        horizontalLayout_2->addWidget(addFriend);

        friendMess = new QListWidget(friendList);
        friendMess->setObjectName("friendMess");
        friendMess->setGeometry(QRect(10, 40, 201, 151));

        verticalLayout->addWidget(friendList);

        groupList = new QWidget(rightWidget);
        groupList->setObjectName("groupList");
        groupTop = new QWidget(groupList);
        groupTop->setObjectName("groupTop");
        groupTop->setGeometry(QRect(10, 0, 201, 41));
        horizontalLayout_3 = new QHBoxLayout(groupTop);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalSpacer_6 = new QSpacerItem(114, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_6);

        addGroup = new QPushButton(groupTop);
        addGroup->setObjectName("addGroup");
        addGroup->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"	width: 60px;\n"
"	height: 30px;\n"
"	border: none;\n"
"	background-color: rgb(20, 180, 206);\n"
"	color: rgb(255, 255, 255);\n"
"	font: 700 italic 11pt \"Yrsa\";\n"
"	border-radius: 5px;\n"
"}"));

        horizontalLayout_3->addWidget(addGroup);

        groupMess = new QListWidget(groupList);
        groupMess->setObjectName("groupMess");
        groupMess->setGeometry(QRect(10, 40, 201, 151));

        verticalLayout->addWidget(groupList);

        widget_3 = new QWidget(centraWidget);
        widget_3->setObjectName("widget_3");
        widget_3->setGeometry(QRect(0, 460, 575, 43));
        horizontalLayout = new QHBoxLayout(widget_3);
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(widget_3);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        horizontalSpacer_7 = new QSpacerItem(467, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_7);

        Send = new QPushButton(widget_3);
        Send->setObjectName("Send");
        Send->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"	width: 60px;\n"
"	height: 30px;\n"
"	border: none;\n"
"	background-color: rgb(20, 180, 206);\n"
"	color: rgb(255, 255, 255);\n"
"	font: 700 italic 11pt \"Yrsa\";\n"
"	 border-radius: 5px;\n"
"}"));

        horizontalLayout->addWidget(Send);


        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QWidget *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Form", nullptr));
        curUserLab->setText(QCoreApplication::translate("MainWindow", "not log in", nullptr));
        curChatLab->setText(QCoreApplication::translate("MainWindow", "chating", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "x", nullptr));
        addFriend->setText(QCoreApplication::translate("MainWindow", "addFriend", nullptr));
        addGroup->setText(QCoreApplication::translate("MainWindow", "addGroup", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "@YXZ", nullptr));
        Send->setText(QCoreApplication::translate("MainWindow", "Send", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATWINDOW_H
