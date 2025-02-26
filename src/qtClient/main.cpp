#include <QApplication>
#include <QCoreApplication> // 确保包含此头文件
#include "LoginResDlg.h"
#include "ChatWindow.h"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <semaphore.h>
#include <thread>
#include "storeMess.h"
#include "json.hpp"
#include "resultCode.h"

using json = nlohmann::json;

// 创建线程，作为守护线程，持续接受服务器的消息
void recvMessage(int cfd);

int main(int argc, char* argv[]) 
{
    qputenv("QT_QPA_PLATFORM", "xcb");

    if(argc < 3) {
        std::cerr << "command invaild!!, example: ./chatClient 127.0.0.1 8888" << std::endl;
    }

    // 创建socket
    int cfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == cfd) {
        std::cerr << "create socket error!!" << std::endl;
        exit(-1);
    }

    // 创建协议地址族
    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);  // ip地址
    addr.sin_port = htons(atoi(argv[2]));  // 端口

    // 连接
    if(-1 == ::connect(cfd, (struct sockaddr*)&addr, sizeof(addr))) {
        std::cerr << "connect server false!!" << std::endl;
        exit(-1);
    }

    // 创建守护线程, 接受消息
    std::thread response(recvMessage, cfd);
    response.detach();

    QApplication a(argc, argv);

    {
        LoginResDlg login(cfd);
        login.show();
        login.exec();
    }

    ChatWindow w(cfd);
    w.show();

    return a.exec();
}

void recvMessage(int cfd)
{
    // 持续接受
    for(;;) {
        char buff[1024] = { 0 };  
        int len = recv(cfd, buff, sizeof(buff), 0);   // 阻塞函数
        if(len == -1 || len == 0) {
            close(cfd);
            exit(-1);
        }

        // 反序列化
        json js = json::parse(buff);
        int msgType = js["msgId"].get<int>();
        int code = js["code"].get<int>();

        std::cout << "main msgType: " << msgType << " json: " << js.dump() << std::endl;

        // 判断, 消息格式在 resultCode中
        if(LOGIN_MSG_ACK == msgType) {  // 登录
            StoreMess::getInstall()->m_ifLogin = true;
            StoreMess::getInstall()->m_loginMess = js.dump();

            // 设置信号量, v操作
            sem_post(&StoreMess::getInstall()->m_loginSem);

        } else if(REGISTER_MSG_ACK == msgType) {   // 注册
            StoreMess::getInstall()->m_isRegister = true;
            StoreMess::getInstall()->m_registerMess = js.dump();

            // 设置信号量, v操作
            sem_post(&StoreMess::getInstall()->m_registerSem);
        } else if(ADD_FRIEND_MSG_ACK == msgType) {
            StoreMess::getInstall()->isAddFriend = true;
            StoreMess::getInstall()->m_addFriendMess = js.dump();
            
            sem_post(&StoreMess::getInstall()->m_addFriendSem);
        } else if(ONE_CHAT_MSG_ACK == msgType) {
            // 加锁
            std::unique_lock<std::mutex> lock(StoreMess::getInstall()->m_recvMessLock);
            json data = js["data"];

            // ChatWindow::getInstall()->ui->friendList->

            StoreMess::getInstall()->m_recvRriendMess.push(data);   // 入队
            StoreMess::getInstall()->m_recvMessNum++;   // 数量+1
        }

    }
}