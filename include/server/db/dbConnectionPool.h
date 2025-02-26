#ifndef DBCONNECTION_H_
#define DBCONNECTION_H_

#include "connection.h"
#include <mysql/mysql.h>
#include <queue>
#include <string>
#include <condition_variable>
#include <atomic>
#include <memory>

// 核心：生产者、消费者模式

class DbConnPool
{
public: 
    // 单例模式
    static DbConnPool* getDbConnPool();
    // 对外提供接口: 获取连接的数据库，通过智能指针回收
    std::shared_ptr<Connection> getMysqlConn();
    
    // 测试
    // void test()
    // {
    //     readConfigurationFile();
    // }

private: 
    // 单例模型：构造函数私有化, 目的：创建最小连接数量
    DbConnPool();
    DbConnPool(const DbConnPool&) = delete;
    DbConnPool operator=(const DbConnPool&) = delete;
    
    // 读取配置文件
    bool readConfigurationFile();

    // 如果没有Mysql连接了，则产生新连接，这个线程驻留后台(像守护线程一样)
    void produceNewConn();

    // 如果队列线程 > initSize， 且空闲时间大于最大空闲时间，则回收
    void recycleConn();

private:
    // MYSQL连接信息
    std::string m_ip;
    unsigned int m_port;
    std::string m_username;
    std::string m_password;
    std::string m_dbname;

    // 数据库连接池信息
    int m_initSize;
    int m_maxSize;
    int m_maxFreeTime;
    int m_maxConnTime;

    // 生产者、消费者共享内存：获取连接
    std::queue<Connection*> m_connQueue;
    // 存储当前存储到队列中存储的数量
    std::atomic_int m_conntionCnt{};

    // 锁
    std::mutex m_queueMuetx;

    // 生产者、消费者：产生连接和取连接
    std::condition_variable m_cv;  // 用于生产线程和消费线程之间的通信
};

#endif // !DBCONNECTION_H_
