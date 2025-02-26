#include "chatServer.h"
#include "chatService.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <signal.h>

void resetHandle(int)
{
    ChatService::getInstance()->reset();
    exit(0);
}

// ip + port
int main(int argc, char* argv[])
{
    signal(SIGINT, resetHandle);

    muduo::net::EventLoop loop;
    muduo::net::InetAddress address(argv[1], atoi(argv[2]));
    ChatServer server(&loop, address, "ChatServer");

    server.start();  // 服务器开启
    loop.loop();    // 事件循环监听
    
    return 0;
}