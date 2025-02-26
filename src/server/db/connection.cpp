#include "connection.h"
#include "log.h"

// 初始化数据库连接
Connection::Connection()
{
    m_sqlConn = mysql_init(nullptr);
    if(m_sqlConn == nullptr) {
        LOGGER("mysql init false !!!");
        
        return;
    }
}
// 释放连接
Connection::~Connection()
{
    mysql_close(m_sqlConn);
}
// 连接数据库
bool Connection::connectionSqlBase(std::string ip, unsigned int port, std::string user, std::string passward, std::string dbName)
{
    if(nullptr == mysql_real_connect(m_sqlConn, ip.c_str(), user.c_str(), passward.c_str(), dbName.c_str(), port, NULL, 0)) {
        LOGGER("mysql connects error!!");
        return false;
    }
    return true;
}

// 获取MYSQL
MYSQL* Connection::getMysql()
{
    return m_sqlConn;
}

// 查询mysql
MYSQL_RES* Connection::query(std::string sql)
{
    int res = mysql_query(m_sqlConn, sql.c_str());
    if(0 != res) {
        LOGGER("sql query false!!!");
        return nullptr;
    }
    return mysql_use_result(m_sqlConn);
}
// 修改数据库数据
bool Connection::modify(std::string sql)
{
    int res = mysql_query(m_sqlConn, sql.c_str());
    if(0 != res) {
        LOGGER("sql update/insert/select false!!!");
        return false;

    }
    return true;
}
// 刷新/设置空闲时间的起始时间点
void Connection::setStartActivateTime()
{
    m_activateTime = clock();
}
// 返回空闲时间
clock_t Connection::getActivateTime()
{
    return clock() - m_activateTime;
}