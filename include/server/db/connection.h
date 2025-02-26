#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <mysql/mysql.h>
#include <ctime>
#include <string>

/*
功能：
    初始化数据库连接
    释放连接
    连接数据库
    查询mysql
    修改数据库数据
    刷新/设置空闲时间的起始时间点
    返回空闲时间
*/

class Connection
{
public:
    // 初始化数据库连接
    Connection();
    // 释放连接
    ~Connection();
    // 连接数据库
    bool connectionSqlBase(std::string ip, unsigned int port, std::string user, std::string passward, std::string dbName);

    // 获取MYSQL
    MYSQL* getMysql();

    // 查询mysql
    MYSQL_RES* query(std::string sql);
    // 修改数据库数据
    bool modify(std::string sql);
    // 刷新/设置空闲时间的起始时间点
    void setStartActivateTime();
    // 返回空闲时间
    clock_t getActivateTime();
private: 
    MYSQL* m_sqlConn{};   // 连接mysql服务器
    clock_t m_activateTime;  // 记录空闲时间的起始点
};

#endif // !CONNECTION_H_
