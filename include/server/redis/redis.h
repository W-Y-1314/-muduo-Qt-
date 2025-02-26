#ifndef REDIS_H_
#define REDIS_H_

#include <hiredis/hiredis.h>
#include <string>
#include <functional>

/*
问题参考
https://blog.csdn.net/QIANGWEIYUAN/article/details/97895611

核心：实现两个API
    发布：每个服务器将相关数据定于在消息队列中
    订阅：每个服务器都是订阅者，当服务器需要找内容的时候，就去消息队列中找
*/

class Redis
{
public:  
    Redis();
    ~Redis();

    // 连接Redis
    bool connect();

    // 向redis的指定通道发送消息, redis数据模式：kv存储
    bool publish(int channel, std::string message);

    // 向redis的指定通道订阅消息(subscribe)
    bool subscribe(int channel);
    // 向redis的指定通道中unsubscribe取消订阅
    bool unsubscribe(int channel);

    // 在独立线程中收发消息(订阅的消息)
    void observer_channel_message();

    // 初始化向业务层中上报通道信息的回调，这个就是封装了一些更具业务所需要的一些 “增删改查” 操作
    void init_notify_hander(std::function<void(int, std::string)> fn);

private: 
    // hiredis 同步上下文对象，负责publish信息
    redisContext* m_publish_context;
    // hiredis 同步上下文对象, 复杂subscribe消息
    redisContext* m_subscribe_context;

    // 回调操作，收到订阅消息, 给service层上报， 这个需要结合业务层函数理解
    std::function<void(int, std::string)> m_notify_message_handler;
};


#endif // !REDIS_H_

