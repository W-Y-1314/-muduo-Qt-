#include "dbConnectionPool.h"
#include "connection.h"
#include "log.h"

#include <iostream>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>
#include <functional>
#include <chrono>

// 创建连接：new，但是拿取连接后是交给shared_ptr

// 单例模式
DbConnPool* DbConnPool::getDbConnPool()
{
    // 最简单的方式：static
    static DbConnPool pool;
    return &pool;
}

// 对外提供接口: 获取连接的数据库，通过智能指针回收
std::shared_ptr<Connection> DbConnPool::getMysqlConn()
{
    std::unique_lock<std::mutex> lock(m_queueMuetx);
    // 判断队列是否为空
    while(m_connQueue.empty()) {
        // 队列为空，则等待 最大连接时间, 即这个时候客户端请求连接，但是池里没有连接了，则会等待，如果超过了最大时间，则：连接失败
        if(std::cv_status::timeout == m_cv.wait_for(lock, std::chrono::seconds(m_maxConnTime))) {
            // 再次判断是否为空
            if(m_connQueue.empty()) {
                LOGGER("no conntion !!!!");
                return nullptr;
            }
        }
    }

    /*
    从队列中获取一个连接，交给**智能指针管理**
        注意：删除连接有回收线程监控，而这里获取的连接使用完后，需要还给**队列中**，所以**需要重写智能指针的回收函数**
    */
   std::shared_ptr<Connection> sp(m_connQueue.front(), [&](Connection* pconn){
        // 注意，这里需要加锁
        std::unique_lock<std::mutex> lock(m_queueMuetx);
        pconn->setStartActivateTime();
        m_connQueue.push(pconn);  // 入队
        m_conntionCnt++;   // +1
   });
   // 弹出队列
    m_connQueue.pop();
    m_conntionCnt--;   // -1

    return sp;
}

// 单例模型：构造函数私有化
DbConnPool::DbConnPool()
{
    if(readConfigurationFile() == false) {
        return;
    }

    std::unique_lock<std::mutex> lock(m_queueMuetx);
    for(int i = 0; i < m_maxSize; i++) {
        Connection* newConn = new Connection();
        newConn->connectionSqlBase(m_ip, m_port, m_username, m_password, m_dbname);
        newConn->setStartActivateTime();   // 设置 空闲时间 的起始点
        m_connQueue.push(newConn);      // 入队
        m_conntionCnt++;     // 存储到队列中数据+1
    }

    // 开启线程：检查是否需要需要**新创建连接**
    std::thread produce(std::bind(&DbConnPool::produceNewConn, this));
    produce.detach();   // 驻留后台
    // 开启线程，检查是否需要**删除连接**
    std::thread search(std::bind(&DbConnPool::recycleConn, this));
    search.detach();    // 驻留后台
}

// 读取配置文件
bool DbConnPool::readConfigurationFile()
{
    FILE* fp = fopen("./mysql.ini", "r");
    if(fp == nullptr) {
        LOGGER("mysql.ini open false!!");
        return false;
    }

    char buff[BUFSIZ] = { 0 };

    while(!feof(fp)) {
        // clear
        memset(buff, 0, sizeof(buff));
        // 读取
        fgets(buff, BUFSIZ, fp);
        std::string str = buff;
        // 判空
        if(str.empty()) {
            continue;
        }
        // 截断
        int idx = str.find('=', 0);
        if(idx == -1) {
            continue;
        }
        int end = str.find('\n', idx);
        std::string key = str.substr(0, idx);
        std::string value = str.substr(idx + 1, end - idx - 1);
        //std::cout << "key: " << key << " value: " << value << std::endl;

        if(key == "ip") {
            m_ip = value;
        } else if(key == "port") {
            m_port = atoi(value.c_str());
        } else if(key == "username") {
            m_username = value;
        } else if(key == "password") {
            m_password = value;
        } else if(key == "dbname") {
            m_dbname = value;
        } else if(key == "initSize") {
            m_initSize = atoi(value.c_str());
        } else if(key == "maxSize") {
            m_maxSize = atoi(value.c_str());
        } else if(key == "maxFreeTime") {
            m_maxFreeTime = atoi(value.c_str());
        } else if(key == "maxConnTime") {
            m_maxConnTime = atoi(value.c_str());
        }
    }

    //std::cout << m_ip << " " << m_port << " " << m_username << " " << m_password << " " << m_dbname << " " << m_initSize << " " << m_maxSize << " " << m_maxFreeTime << " " << m_maxConnTime << std::endl;

    return true;
}

// 如果池里没有Mysql连接了，则产生新连接，这个线程驻留后台(像守护线程一样)
/*
实现思路：
    设置一个循环：循环检查
        如果队列不为空，则条件变量一直等待
*/
void DbConnPool::produceNewConn()
{
    for(;;) {
        std::unique_lock<std::mutex> lock(m_queueMuetx);
        while(!m_connQueue.empty()) {
            m_cv.wait(lock);    // 条件变量一直等待
        }

        // 这个时候，队列为空，从新创建连接
        for(int i = 0; i < m_maxSize; i++) {
            Connection* newConn = new Connection();
            newConn->setStartActivateTime();   // 刷新时间
            m_connQueue.push(newConn);
            m_conntionCnt++;   // +1
        }

        // 通知等待线程
        m_cv.notify_all();
    }
}

// 如果队列线程 > initSize， 且空闲时间大于最大空闲时间，则回收
void DbConnPool::recycleConn()
{
    for(;;) {
        std::unique_lock<std::mutex> lock(m_queueMuetx);
        while(m_conntionCnt > m_initSize) {
            Connection* conn = m_connQueue.front();
            // 超过最大空闲时间
            if((static_cast<double>(conn->getActivateTime()) / CLOCKS_PER_SEC) > m_maxFreeTime) {
                m_connQueue.pop();
                m_conntionCnt--;
                delete conn;
            } else {   // 对头没超过，则直接退出
                break;
            }
        }
    }
}