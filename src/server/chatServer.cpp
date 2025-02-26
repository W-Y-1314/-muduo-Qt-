#include "chatServer.h"
#include "public.h"
#include "chatService.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
#include <algorithm>



ChatServer::ChatServer(muduo::net::EventLoop* loop, const muduo::net::InetAddress& listenAddr, const std::string& nameArg)
    :m_server(loop, listenAddr, nameArg)
    ,m_loop(loop)
{
    // 设置回调函数
    m_server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, std::placeholders::_1));
    m_server.setMessageCallback(std::bind(&ChatServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // 设置线程数
    m_server.setThreadNum(4);
}

void ChatServer::start()
{
    m_server.start();
}

    
void ChatServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if(!conn->connected()) {
        // 异常退出处理
        ChatService::getInstance()->clientCloseException(conn);

        conn->shutdown();
    }
}
    
void ChatServer::onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp receiveTime)
{
    // 获取数据
    std::string buff = buffer->retrieveAllAsString();
    std::string temp = buff;
    // 测试
    std::cout << "onMessage: " << buff << std::endl;

    try {
        // 反序列化, 里面有一个处理对应业务的消息处理方法的id
        json js = json::parse(buff);

        // 根据消息id，获取对应的业务处理方法
        auto handleMethod = ChatService::getInstance()->getHandleMethod(js["msgId"].get<int>());
        // 调用业务处理方法
        handleMethod(conn, js, receiveTime);
        
    } catch (const nlohmann::json::parse_error& e) {
        // 如果解析失败，记录错误信息或采取其他措施
        std::cerr << "JSON parse error: " << e.what() << '\n';
    }
}

