#include "redis.h"
#include <iostream>
#include <thread>

/*
本案例用处：存储发送给用户的用户id和和他发送的信息
客户端会进入订阅状态，处于此状态下客户端不能使用除subscribe、unsubscribe、psubscribe和punsubscribe这四个属于"发布/订阅"之外的命令，否则会报错
1、redis发布订阅模式，不用的时候需要
2、开启线程，监听消息队列是是否有所需要的数据
3、执行成功一句redis，需要释放(freeReplyObject)
4、订阅的时候，SUBSCRIBE是上一个阻塞的，这里用一个异步超做，把数据放到缓冲区
*/

Redis::Redis()
    : m_publish_context(nullptr)
    , m_subscribe_context(nullptr)
{

}

Redis::~Redis()
{
    // 释放发布订阅模式
    if(m_publish_context == nullptr) {
        redisFree(m_publish_context);
    } 
    if(m_subscribe_context == nullptr) {
        redisFree(m_subscribe_context);
    }
}

// 连接Redis
bool Redis::connect()
{
    // 负责发布消息的连接上下文
    m_publish_context = redisConnect("127.0.0.1", 6379);
    if(m_publish_context == nullptr) {
        std::cerr << "the redis connect failed!!" << std::endl;
        return false;
    }

    // 负责订阅消息的连接上下文
    m_subscribe_context = redisConnect("127.0.0.1", 6379);
    if(m_subscribe_context == nullptr) {
        std::cerr << "the redis connect failed!!" << std::endl;
        return false;
    }

    // 在单独线程中监听通道上的事件， 这样获取消息，给业务层上报
    std::thread t([&](){
        observer_channel_message();   // 这个函数具体实现后，业务层具体处理是在业务层中
    });
    t.detach();

    std::cout << "the redis-server connect success!!" << std::endl;
    
    return true;
}

// 向redis的指定通道发送消息, redis数据模式：kv存储
bool Redis::publish(int channel, std::string message)
{
    // redis: kv存储形式
    redisReply* reply = (redisReply*)redisCommand(m_publish_context, "PUBLISH %d %s", channel, message.c_str());
    if(nullptr == reply) {
        std::cerr << "publish command failed!" << std::endl;
        return false;
    }
    freeReplyObject(reply);  // 注意要释放
    return true;
}

// 向redis的指定通道订阅消息(subscribe)
bool Redis::subscribe(int channel)
{
    // SUBSCRIBE命令本身会造成线程阻塞等待通道里面发生消息，这里只做订阅通道，不接收通道消息
    // 通道消息的接收专门在observer_channel_message函数中的独立线程中进行
    // 只负责发送命令，不阻塞接收redis server响应消息，否则和notifyMsg线程抢占响应资源
    
    /*
    redisAppendCommand 是 hiredis 库中的一个异步 API，它允许你将 Redis 命令追加到输出缓冲区中，而不立即等待响应。这对于实现非阻塞操作特别有用
    原理：redisAppendCommand将数据发送到缓冲区中，redisBufferWrite再把数据发生给redis服务器中
    */
    if(REDIS_ERR == redisAppendCommand(m_publish_context, "SUBSCRIBE %d", channel)) {
        std::cerr << "subscribe command failed!!" << std::endl;
        return false;
    }

    // 读取缓冲区数据, redisBufferWrite 可以将缓冲区中的数据写入到 Redis 服务器
    int done = 0;
    while(!done) {
        if(REDIS_ERR == redisBufferWrite(m_subscribe_context, &done)) {
            std::cerr << "subscrice command failed!!" << std::endl;
            return false;
        }
    }

    return true;
}
// 向redis的指定通道中unsubscribe取消订阅
bool Redis::unsubscribe(int channel)
{
    // 同样采用异步模式
    if(REDIS_ERR == redisAppendCommand(m_subscribe_context, "UNSUBSRIBE %d", channel)) {
        std::cerr << "unsubscribe command failed!!" << std::endl;
        return false;
    }

    int done = 0;
    while(!done) {
        if(REDIS_ERR == redisBufferWrite(m_subscribe_context, &done)) {
            std::cerr << "unsubscribe command failed!!" << std::endl;
            return false;
        }
    }

    return true;
}

// 在独立线程中收发消息(订阅的消息)
void Redis::observer_channel_message()
{
    redisReply* reply = nullptr;
    // 得到订阅消息
    while(REDIS_OK == redisGetReply(m_subscribe_context, (void**)&reply)) {
        // 收到订阅消息的是一个三元素信息: subscribe、messgae、unsubscribe
        if(reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr) {
            // 给业务层发送消息
            m_notify_message_handler(atoi(reply->element[1]->str), reply->element[2]->str);
        }

        freeReplyObject(reply);
    }

    std::cerr << "------- observer_channel_message quit -----------" << std::endl;
}

// 初始化向业务层中上报通道信息的回调，这个就是封装了一些更具业务所需要的一些 “增删改查” 操作
void Redis::init_notify_hander(std::function<void(int, std::string)> fn)
{
    m_notify_message_handler = fn;
}