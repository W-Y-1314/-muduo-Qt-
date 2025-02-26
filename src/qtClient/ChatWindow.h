#ifndef CHATWINDOW_H_
#define CHATWINDOW_H_

#include <QWidget> 
#include "ui_ChatWindow.h"
#include "user.h"
#include "group.h"
#include <vector>
#include <list>
#include "resultCode.h"
#include <unordered_map>

// /home/wy/Qt/6.5.3/gcc_64/libexec/uic

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE 

class QListWidgetItem;

/*
获得在本用户离线时候的消息，本人感觉最好就是在登录成功的时候获取，故这里应该设置单例模式
*/

class ChatWindow : public QWidget   
{
    Q_OBJECT

    enum {
        FRIEND_CHAT = 1,
        GROUP_CHAT,
    };

public: 
    static ChatWindow* getInstall() {
        static ChatWindow c;
        return &c;
    }

    ChatWindow(int cfd ,QWidget* parent = nullptr);
    ~ChatWindow();

    void initUi();

    // 初始化一些信息，如好友和群友列表
    void listMessage();

    // 读取服务器消息， 一对一好友聊天
    void readMessage();
    void one_chat();

    // 聊天
    void chat();

    // 添加好友
    void addFriend();

    // 创建添加群窗口

public slots:
    void onItemClickedFriend(QListWidgetItem* item);

private: 
    Ui::MainWindow* ui;
    // 启用默认的构造函数
    ChatWindow() = default;

    int m_cfd;  // 服务器socketfd

    // 存储当前用户信息
    User m_curUser;
    // 存储当前用户好友信息
    std::unordered_map<int, User> m_friendList;
    // 存储当前组用户信息
    std::unordered_map<int, Group> m_groupList;

    // 要发送给对方好友Id
    int m_toFriend{};
    std::string m_toFriendName{};

    // 当前是什么聊天
    int m_chatClass{};

public:
    // 获取好友在你离线的时候消息, 再点击好友的时候触发
    std::unordered_map<int, std::string> m_friendLeaveMess;

    // 存储聊天好友发送过来的信息，需要点击改用户的时候，能做到一对一聊天
    std::list<json> m_recvChatMess;
};


#endif // !CHATWINDOW_H_

