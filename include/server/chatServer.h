#ifndef CHATSERVER_H_
#define CHATSERVER_H_

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <string>

class ChatServer
{
public:
    // 初始化服务器
    ChatServer(muduo::net::EventLoop* loop, const muduo::net::InetAddress& listenAddr, const std::string& nameArg);

    // 启动服务器
    void start();
private:  
    // 连接、读写回调
    void onConnection(const muduo::net::TcpConnectionPtr& conn);
    void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp receiveTime);

    muduo::net::TcpServer m_server;
    muduo::net::EventLoop* m_loop;
};

#endif // !CHATSERVER_H_
