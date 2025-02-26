#include "chatService.h"
#include "dbConnectionPool.h"
#include "connection.h"
#include "resultCode.h"
#include "user.h"
#include "public.h"
#include "offlineMessModel.h"
#include "redis.h"
#include <muduo/base/Logging.h>
#include <functional>
#include <iostream>

ChatService::ChatService()
{
    using namespace std;
    m_msgHandle.insert({LOGIN_MSG, bind(&ChatService::Login, this, placeholders::_1, placeholders::_2, placeholders::_3)});  // 登录
    m_msgHandle.insert({REGISTER_MSG, bind(&ChatService::Resgiter, this, placeholders::_1, placeholders::_2, placeholders::_3)});  // 注册
    m_msgHandle.insert({ONE_CHAT_MSG, bind(&ChatService::oneToOneChat, this, placeholders::_1, placeholders::_2, placeholders::_3)});  // 聊天 
    m_msgHandle.insert({ADD_FREIEND_MSG, bind(&ChatService::addFriend, this, placeholders::_1, placeholders::_2, placeholders::_3)});  // 添加好友
    // 群
    m_msgHandle.insert({CREATE_GROUP_MSG, bind(&ChatService::createGroup, this, placeholders::_1, placeholders::_2, placeholders::_3)}); // 创建群
    m_msgHandle.insert({ADD_GROUP_MSG, bind(&ChatService::addGroup, this, placeholders::_1, placeholders::_2, placeholders::_3)});  // 加入组
    m_msgHandle.insert({GROUP_CHAT_MSG, bind(&ChatService::groupChat, this, placeholders::_1, placeholders::_2, placeholders::_3)}); // 群聊天

    // 连接redis
    if(m_redis.connect()) {
        // 这里是：在通道中，监听到有消息
        m_redis.init_notify_hander(std::bind(&ChatService::handleRedisSubsrcibeMessage, this, std::placeholders::_1, std::placeholders::_2));
    }

}

// 根据消息ID，获得不同业务处理方法
MsgHandle ChatService::getHandleMethod(int msgId)
{
    // 查找
    auto it = m_msgHandle.find(msgId);
    if(it == m_msgHandle.end()) {
        return [=](const muduo::net::TcpConnectionPtr& conn, json &js, muduo::Timestamp time) {
            // muduo库的日志
            LOG_ERROR << "not find method for msgId: " << msgId; 
        };
    }
    return m_msgHandle[msgId];
}

// 获取单例
ChatService* ChatService::getInstance()
{
    static ChatService service;
    return &service;
}

// 处理客户端异常退出 
/*
两件事：从用户链接表中移除；设置用户下线状态；
注意：线程安全
*/
void ChatService::clientCloseException(const muduo::net::TcpConnectionPtr& conn)
{
    std::cout << "exception" << std::endl;
    User user;
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        for(auto it = m_userConnMap.begin(); it != m_userConnMap.end(); it++) {
            if(it->second == conn) {
                user.setId(it->first);
                m_userConnMap.erase(it->first);
                break;
            }
        }
    }

    // 用户下线， 解除订阅通道
    m_redis.unsubscribe(user.getId());

    // 重设状态
    if(user.getId() != -1) {
        user.setState("offline");
        m_userModel.updateState(user);
    }
}

// 服务器异常退出，从新设置状态, 全员状态 offline
void ChatService::reset()
{
    // 从新设置所有人状态
    m_userModel.resetState();
}

// 登录模块，id、pwd、name
/*
通过id：在数据库中找信息
*/
void ChatService::Login(const muduo::net::TcpConnectionPtr& conn, json &js, muduo::Timestamp time)
{
    // 获取用户id
    int id = js["userId"].get<int>();
    std::string name = js["username"].get<std::string>();
    std::string pwd = js["password"].get<std::string>();

    // 传入数据，数据查询
    User user = m_userModel.query(id);
    // 验证
    if(user.getName() == name && user.getPwd() == pwd) {
        // 检查状态是否已经是登录状态
        if(user.getState() == "online") {
            // 已经登录，返回响应
            conn->send(Result::success(LOGIN_MSG_ACK, ResultCode::HaveLogin));
        } else {  // 登录成功
            // 储存连接用户信息，注意：线程安全
            {
                std::unique_lock<std::mutex> locl(m_mtx);
                m_userConnMap.insert({user.getId(), conn});
            }

            // 登录成功后，向redis订阅id
            m_redis.subscribe(id);

            // 创建响应数据
            json response;
            response["userId"] = user.getId();
            response["username"] = user.getName();

            // 更新状态, online
            user.setState("online");
            m_userModel.updateState(user);

            //-------------------------------------------- 朋友列表 -------------------------------
            std::vector<User> friendList = m_friendModel.query(id);
            if(!friendList.empty()) {
                std::vector<std::string> v;
                for(auto it : friendList) {
                    json js;
                    js["id"] = it.getId();
                    js["name"] = it.getName();
                    js["state"] = it.getState();
                    v.push_back(js.dump());
                }
                // 响应
                response["friendList"] = v;
            }

            // ------------------------------------ 群组消息 -----------------------------------
            std::vector<Group> groupUsers = m_groupModel.queryGroups(user.getId());
            std::vector<std::string> groupMess;
            if(!groupUsers.empty()) {
                // [{"groupId": xxx, "groupname": xxx, "groupdesc": xxx, "users": [...]}, ...]
                for(auto& it : groupUsers) {
                    json resJs; 
                    resJs["groupId"] = it.getId();
                    resJs["groupname"] = it.getName();
                    resJs["groupdesc"] = it.getDesc();
                    // 具体且群人消息
                    std::vector<std::string> userV; 
                    for(GroupUser& v : it.getGroupUser()) {
                        json js;
                        js["userId"] = v.getId();
                        js["name"] = v.getName();
                        js["state"] = v.getState();
                        js["role"] = v.getRole();
                        userV.push_back(js.dump());
                    }
                    resJs["users"] = userV;
                    // 加入群中
                    groupMess.push_back(resJs.dump());
                }
            }
            response["groups"] = groupMess;

            //---------------------------------------------- 是否有离线消息 -------------------------
            std::vector<std::string> offlineVec = m_offlineMessModel.query(user.getId());
            
            if(!offlineVec.empty()) {   // 有离线消息
                // 添加离线消息
                response["OfflineMessage"] = offlineVec;
                // 移除信息
                m_offlineMessModel.remove(id);
            }


            // 登录成功，返回响应
            conn->send(Result::success(LOGIN_MSG_ACK, ResultCode::LoginSuccess, response));
        }
    } else {  // 登录失败
        conn->send(Result::failure(LOGIN_MSG_ACK, ResultCode::LoginFailure));
    }

}
// 注册模块, id、name、password
void ChatService::Resgiter(const muduo::net::TcpConnectionPtr& conn, json &js, muduo::Timestamp time)
{
    // 获取信息
    int id = js["id"].get<int>();
    std::string name = js["username"].get<std::string>();
    std::string pwd = js["password"].get<std::string>();

    User user;
    user.setId(id);
    user.setName(name);
    user.setPwd(pwd);

    // 数据更新在模型里面
    bool res = m_userModel.insert(user);
    if(res) {  // 成功
        conn->send(Result::success(REGISTER_MSG_ACK, ResultCode::RegisterSuccess));
    } else {  // 失败
        conn->send(Result::success(REGISTER_MSG_ACK, ResultCode::RegisterFailure)); 
    }
}

// 一对一聊天、存储离线消息
void ChatService::oneToOneChat(const muduo::net::TcpConnectionPtr& conn, json &js, muduo::Timestamp time)
{
    int fromId = js["id"].get<int>();
    std::string fromName = js["from"].get<std::string>();
    int toId = js["to"].get<int>();
    std::string toMsg = js["msg"].get<std::string>();

    User user = m_userModel.query(toId);
    // toid是否在线，在线存储
    if(user.getState() == "online") {
        m_redis.publish(toId, js.dump());
    }

    // 转发, 注意：加锁
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        auto toUser = m_userConnMap.find(toId);
        if(toUser != m_userConnMap.end()) {  // 在线
            // 存在用户，转发
            json js;
            js["fromId"] = fromId;
            js["fromUsername"] = fromName;
            js["msg"] = toMsg;

            // 转发给该用户
            toUser->second->send(Result::success(ONE_CHAT_MSG_ACK, ResultCode::OneChatSuccess, js));
            
            return;
        }
    }

    // 离线，存储消息
    if(!m_offlineMessModel.insert(toId, toMsg)) {
        LOG_ERROR << __FUNCTION__ << " SQL execute error";
    }
}

// 添加好友哦信息
void ChatService::addFriend(const muduo::net::TcpConnectionPtr& conn, json &js, muduo::Timestamp time)
{
    int userId = js["id"].get<int>();
    int friendId = js["friendId"].get<int>();

    if(m_friendModel.insert(userId, friendId)) {
        conn->send(Result::success(ADD_FRIEND_MSG_ACK, ResultCode::AddFriendSuccess));
    } else {
        conn->send(Result::failure(ADD_FRIEND_MSG_ACK, ResultCode::AddFriendFailure));
    }   
}

// 创建组
/*
创建组、存储组用户信息
*/
void ChatService::createGroup(const muduo::net::TcpConnectionPtr& conn, json &js, muduo::Timestamp time)
{
    int userId = js["userId"].get<int>();
    std::string groupname = js["groupname"].get<std::string>();
    std::string groupdesc = js["groupdesc"].get<std::string>();

    // 创建组
    Group group;
    group.setName(groupname);
    group.setDesc(groupdesc);
    if(m_groupModel.createGroup(group)) {
        // 存储创建组 用户人信息
        m_groupModel.addGroup(userId, group.getId(), "creator");
        conn->send(Result::success(CREATE_GROUP_MSG_ACK, ResultCode::CreateGroupSuccess));
    } else {
        conn->send(Result::success(CREATE_GROUP_MSG_ACK, ResultCode::CreateGroupFailure));
    }
}

// 加入组
void ChatService::addGroup(const muduo::net::TcpConnectionPtr& conn, json &js, muduo::Timestamp time)
{
    int userId = js["userId"].get<int>();
    int groupId = js["groupId"].get<int>();

    if(m_groupModel.addGroup(userId, groupId)) {
        conn->send(Result::success(ADD_GROUP_MSG_ACK, ResultCode::AddGroupSuccess));
    } else {
        conn->send(Result::success(ADD_GROUP_MSG_ACK, ResultCode::AddGroupFailure));
    }
}

// 群组聊天
void ChatService::groupChat(const muduo::net::TcpConnectionPtr& conn, json &js, muduo::Timestamp time)
{
    int userId = js["userId"].get<int>();
    int groupId = js["groupId"].get<int>();
    std::string msg = js["msg"].get<std::string>();

    auto users = m_groupModel.queryRroupUsers(userId, groupId);
    for(auto& it : users) {
        auto otherConn = m_userConnMap.find(it);
        if(otherConn != m_userConnMap.end()) {
            otherConn->second->send(js.dump());
        } else {
            // 有好友且离线
            User us = m_userModel.query(it);
            if(us.getState() == "offline") {
                m_offlineMessModel.insert(userId, js.dump());
            } else {
                m_redis.publish(us.getId(), js.dump());
            }
        }
    }
}

// 从redis消息队列中获取消息
void ChatService::handleRedisSubsrcibeMessage(int userId, std::string msg)
{
    std::unique_lock<std::mutex> lock(m_mtx);
    auto it = m_userConnMap.find(userId);  // 找对应用户的消息
    if(it != m_userConnMap.end()) {
        it->second->send(msg);   // 发送给对方
        return;
    }

    // 存储离线消息
    m_offlineMessModel.insert(userId, msg);
}