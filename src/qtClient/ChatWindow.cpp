#include "ChatWindow.h"
#include "ui_ChatWindow.h"
#include "EventFilterObject.h"
#include "storeMess.h"
#include "group.h"
#include "groupUser.h"
#include "public.h"
#include "resultCode.h"
#include "ChatBubble.h"

#include <QPushButton>
#include <QString>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QListWidget>
#include <QPixmap>

#include <sys/types.h>
#include <sys/socket.h>
#include <thread>

ChatWindow::ChatWindow(int cfd, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
    , m_cfd(cfd)
{
    ui->setupUi(this);

    // 发送消息和接受消息
    std::thread readMess(&ChatWindow::readMessage, this);
    readMess.detach();  // 驻留后台

    // 存储不同用户的信息

    initUi();

}
ChatWindow::~ChatWindow()
{
    delete ui;
}

void ChatWindow::initUi()
{
    // 去边框
    setWindowFlag(Qt::FramelessWindowHint);
    // 设置透明背景
    setAttribute(Qt::WA_TranslucentBackground);

    // 安装过滤器
    installEventFilter(new EventFilterObject(this));

    // 关闭
    connect(ui->pushButton, &QPushButton::clicked, this, &ChatWindow::close);

    // top修改页面
    json js = json::parse(StoreMess::getInstall()->m_loginMess);
    std::string name = "user: " + js["data"]["username"].get<std::string>() + " pop up online";
    ui->curUserLab->setText(QString(name.c_str()));

    // 初始化，好友、群友列表
    listMessage();

    // 连接信号 ---------------
    connect(ui->addFriend, &QPushButton::clicked, this, &ChatWindow::addFriend);

    // 朋友列表
    connect(ui->friendMess, &QListWidget::itemClicked, this, &ChatWindow::onItemClickedFriend);

    // 发送按钮
    connect(ui->Send, &QPushButton::clicked, this, &ChatWindow::chat);
}

// 初始化一些信息，如好友和群友列表
void ChatWindow::listMessage()
{
     /*
    main msgType: 2 json: {"code":2,"codeStr":"LoginSuccess",
    "data":{"friendList":["{\"id\":2,\"name\":\"yy\",\"state\":\"offline\"}","{\"id\":3,\"name\":\"ww\",\"state\":\"offline\"}"],
        "groups":["{\"groupId\":1,\"groupdesc\":\"wy\",\"groupname\":\"wxg\",\"users\":[\"{\\\"name\\\":\\\"yxz\\\",\\\"role\\\":\\\"normal\\\",\\\"state\\\":\\\"online\\\",\\\"userId\\\":1}\",\"{\\\"name\\\":\\\"yy\\\",\\\"role\\\":\\\"normal\\\",\\\"state\\\":\\\"offline\\\",\\\"userId\\\":2}\"]}"],
        "userId":1,
        "username":"yxz"},
    "message":"the user login success!!","msgId":2}
    */

    // 获取信息
    if(StoreMess::getInstall()->m_ifLogin && !StoreMess::getInstall()->m_loginMess.empty()) {  // 登录了, 且获取了信息
        json js = json::parse(StoreMess::getInstall()->m_loginMess);

        // 1、存储当前用户信息
        m_curUser.setId(js["data"]["userId"].get<int>());
        m_curUser.setName(js["data"]["username"].get<std::string>());

        // 2、获取好友列表
        if(js["data"].contains("friendList")) {
            std::vector<std::string> friendMessage = js["data"]["friendList"].get<std::vector<std::string>>();
            for(std::string& str : friendMessage) {
                json u = json::parse(str);
                User user;
                int id = u["id"].get<int>();
                
                user.setId(id);
                user.setName(u["name"].get<std::string>());
                user.setState(u["state"].get<std::string>());
                // 存储
                m_friendList.insert({id, user});

                // 显示到列表上
                QListWidgetItem* item = new QListWidgetItem(u["name"].get<std::string>().c_str());
                // 存储 id，在额外项
                item->setData(Qt::UserRole, u["id"].get<int>());
                // 显示在图形上
                ui->friendMess->addItem(item);
            }
        }

        // 3、获取群组信息
        if(js["data"].contains("group")) {
            std::vector<std::string> groupMessage = js["data"]["groups"].get<std::vector<std::string>>();
            for(std::string& str : groupMessage) {
                json js = json::parse(str);

                Group group;

                int id = js["groupId"].get<int>();
                std::string name = js["groupname"].get<std::string>();
                std::string desc = js["groupdesc"].get<std::string>();
                std::vector<std::string> users = js["users"].get<std::vector<std::string>>();

                group.setId(id);
                group.setName(name);
                group.setDesc(desc);
                
                for(auto& it : users) {
                    json ujs = json::parse(it);
                    GroupUser groupUser;
                    groupUser.setId(ujs["userId"]);
                    groupUser.setName(ujs["name"].get<std::string>());
                    groupUser.setRole(ujs["role"].get<std::string>());
                    groupUser.setState(ujs["state"].get<std::string>());

                    group.getGroupUser().push_back(groupUser);
                }

                // 添加
                m_groupList.insert({id, group});

                // 显示
                QListWidgetItem* item = new QListWidgetItem(name.c_str());
                // 存储群 id
                item->setData(Qt::UserRole, id);
                ui->groupMess->addItem(item);
            }
        }
    }
}

// 读取服务器消息, 一对一好友聊天
void ChatWindow::readMessage()
{
    for(;;) {
        // 找到点击光标位置
        auto item = ui->friendMess->currentItem();
        if(item == nullptr) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 短暂休眠
            continue;;
        }
        int id = item->data(Qt::UserRole).toInt();
        // 有消息, 存储
        while(StoreMess::getInstall()->m_recvMessNum > 0) {
            m_recvChatMess.push_back(StoreMess::getInstall()->m_recvRriendMess.front());
            StoreMess::getInstall()->m_recvRriendMess.pop();  // 弹出
            StoreMess::getInstall()->m_recvMessNum--;
        }

        std::list<json> messagesToRemove; // 记录需要移除的消息
        // 遍历新接收的消息列表，找到符合条件的消息并更新UI
        for(auto it = m_recvChatMess.begin(); it != m_recvChatMess.end(); ++it) {
            int reId = (*it)["fromId"].get<int>();
            if(reId == id) {
                QString msg = QString::fromStdString((*it)["msg"].get<std::string>());

                // 这个是核心：用于不同
                QMetaObject::invokeMethod(this, [this, msg]() {
                    ChatBubble* bubble = new ChatBubble(QPixmap(), msg, ChatBubble::BubbleLeft);
                    ui->chatMsgListWidget->addItem(bubble);
                    ui->chatMsgListWidget->setItemWidget(bubble, bubble);
                    
                    // 滚轮滚动
                    ui->chatMsgListWidget->scrollToItem(bubble);

                }, Qt::QueuedConnection);   

                messagesToRemove.push_back(*it);
            }
        }

        // 统一移除已处理的消息
        for(const auto& message : messagesToRemove) {
            m_recvChatMess.remove(message);
        }
    }
}

void ChatWindow::chat()
{
    // 朋友
    if(m_chatClass == ChatWindow::FRIEND_CHAT) {
        std::string mess = ui->chatMsgSendEdit->toPlainText().toStdString();

        json js;
        js["msgId"] = ONE_CHAT_MSG;
        js["id"] = m_curUser.getId();
        js["from"] = m_curUser.getName();
        js["to"] = m_toFriend;
        js["msg"] = mess;

        qDebug() << js.dump();
    
        send(m_cfd, js.dump().c_str(), js.dump().size(), 0);

        // 显示发送内容
        ChatBubble* bubble = new ChatBubble(QPixmap(), mess.c_str(), ChatBubble::BubbleRight);
        ui->chatMsgListWidget->addItem(bubble);
        ui->chatMsgListWidget->setItemWidget(bubble, bubble);

        // 清空
        ui->chatMsgSendEdit->clear();

    } else if(m_chatClass == ChatWindow::GROUP_CHAT) {

    }
}

// 添加好友
void ChatWindow::addFriend()
{
    // 参考：https://blog.csdn.net/MrHHHHHH/article/details/134154374
    int id = QInputDialog::getInt(this, "welcome to you!!", "Please enter the friend id: ");

    //qDebug() << id;

    // 封装json信息
    json js;
    js["msgId"] = ADD_FREIEND_MSG;
    js["id"] = m_curUser.getId();
    js["friendId"] = id;

    // 发送给客户端
    send(m_cfd, js.dump().c_str(), js.dump().size(), 0);
    
    // 收到消息响应
    if(sem_wait(&StoreMess::getInstall()->m_addFriendSem) == 0) {
        json js = json::parse(StoreMess::getInstall()->m_addFriendMess);
        if(js["code"].get<int>() == 6) {  // 成功，resultCode.h 文件
            QMessageBox::information(this, "notie", js["message"].get<std::string>().c_str());

            // 添加进列表中 
            // ui->friendList->addAction()
        }
    }
}

void ChatWindow::onItemClickedFriend(QListWidgetItem* item)
{
    // 得到要发送好友的Id
    int toId = item->data(Qt::UserRole).toInt();
    m_toFriend = toId;
    m_toFriendName = item->text().toStdString();

    m_chatClass = ChatWindow::FRIEND_CHAT;

    // 修改标题
    std::string title = m_toFriendName + " 聊天~~";
    ui->curUserLab->setText(QString(title.c_str()));

    // 显示在离线时候的消息
    std::vector<std::string> mess = StoreMess::getInstall()->m_loginFriendMess;
    for(auto& it : mess) {
        // id
        int pos = it.find(':');
        int id = atoi(it.substr(0, pos).c_str());
        std::string m = it.substr(pos + 1, it.length());

        std::cout << "-------" << id << ": " << m << std::endl;

        // new一个窗口
        QPixmap p;
        ChatBubble* bubble = new ChatBubble(p, m.c_str(), ChatBubble::BubbleLeft);
        ui->chatMsgListWidget->addItem(bubble);
        ui->chatMsgListWidget->setItemWidget(bubble, bubble);
    }
}



