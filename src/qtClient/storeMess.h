#ifndef STOREMESS_H_
#define STOREMESS_H_

#include <iostream>
#include <semaphore.h>
#include <queue>
#include <mutex>
#include <unordered_map>
#include <list>
#include <atomic>

#include "json.hpp"

using json = nlohmann::json;

class StoreMess
{
public:  
    static StoreMess* getInstall();

    // ----------- 登录
    bool m_ifLogin{false};   // 是否可以登录
    std::string m_loginMess;
    sem_t m_loginSem;            // 用于登录读写信号量(pv操作)，因为登录从数据库中返回数据需要时间, 常见使用：直接问 ai

    // -------- 注册
    bool m_isRegister{false};   
    std::string m_registerMess;
    sem_t m_registerSem;      
    
    // -------- 添加好友信息
    bool isAddFriend{false};
    std::string m_addFriendMess;
    sem_t m_addFriendSem;

    // 好友发送信息
    std::mutex m_recvMessLock;
    std::atomic<int> m_recvMessNum{0};    // 统计收到多少条信息
    std::queue<json> m_recvRriendMess;

    // 不在线时候的好有发送的消息
    std::vector<std::string> m_loginFriendMess;
    
private: 
    StoreMess();
};

#endif // !STOREMESS_H_