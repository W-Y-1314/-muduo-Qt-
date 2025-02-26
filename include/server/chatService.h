#ifndef CHATSERVICE_H_
#define CHATSERVICE_H_

#include "public.h"
#include "userModel.h"
#include "offlineMessModel.h"
#include "friendModel.h"
#include "groupModel.h"
#include "redis.h"
#include <unordered_map>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/TcpServer.h>
#include <functional>
#include <string>
#include <mutex>

// 封装函数
using MsgHandle = std::function<void(const muduo::net::TcpConnectionPtr&, json&, muduo::Timestamp)>;

class ChatService
{
public:
    // 获取单例
    static ChatService* getInstance();
    // 根据消息ID，获得不同业务处理方法
    MsgHandle getHandleMethod(int msgId);

    // 登录模块
    void Login(const muduo::net::TcpConnectionPtr& conn, json &js, muduo::Timestamp time);
    // 注册模块
    void Resgiter(const muduo::net::TcpConnectionPtr& conn, json &js, muduo::Timestamp time);

    // 一对一聊天、存储离线消息
    void oneToOneChat(const muduo::net::TcpConnectionPtr& conn, json &js, muduo::Timestamp time);

    // 添加好友哦信息
    void addFriend(const muduo::net::TcpConnectionPtr& conn, json &js, muduo::Timestamp time);

    // 创建组
    void createGroup(const muduo::net::TcpConnectionPtr& conn, json &js, muduo::Timestamp time);
    // 加入组
    void addGroup(const muduo::net::TcpConnectionPtr& conn, json &js, muduo::Timestamp time);
    // 群组聊天
    void groupChat(const muduo::net::TcpConnectionPtr& conn, json &js, muduo::Timestamp time);

    // 处理客户端异常退出
    void clientCloseException(const muduo::net::TcpConnectionPtr& conn);
    // 服务器异常退出，ctrl+c
    void reset();

    // 从redis消息队列中获取消息
    void handleRedisSubsrcibeMessage(int userId, std::string msg);

private: 
    // 单例模式
    ChatService();

    // 储存消息处理函数，形成映射关系
    std::unordered_map<int, MsgHandle> m_msgHandle;

    // 储存连接的用户信息
    std::map<int, muduo::net::TcpConnectionPtr> m_userConnMap;
    // 锁
    std::mutex m_mtx;

    // 数据模型
    UserModel m_userModel;
    OfflineMessModel m_offlineMessModel;
    FriendModel m_friendModel;
    GroupModel m_groupModel;

    // Redis
    Redis m_redis;
};

#endif // !CHATSERVICE_H_
