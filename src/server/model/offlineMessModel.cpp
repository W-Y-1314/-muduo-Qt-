#include "offlineMessModel.h"

// 存储用户离线消息
bool OfflineMessModel::insert(int userId ,std::string msg) 
{
    // sql
    char sql[1024] = { 0 };
    sprintf(sql, "insert into offlineMessage(userId, messgae) values(%d, '%s');", userId, msg.c_str());
    // 执行
    std::shared_ptr<Connection> sqlConn = DbConnPool::getDbConnPool()->getMysqlConn();
    if(sqlConn->modify(sql)) {  // 成功
        return true;
    }

    return false;
}
// 删除用户离线信息
bool OfflineMessModel::remove(int userId)
{
    // sql
    char sql[1024] = { 0 };
    sprintf(sql, "delete from offlineMessage where userId=%d;", userId);
    // 执行
    std::shared_ptr<Connection> sqlConn = DbConnPool::getDbConnPool()->getMysqlConn();
    if(sqlConn->modify(sql)) {  // 成功
        return true;
    }

    return false;
}
// 查询离线信息
std::vector<std::string> OfflineMessModel::query(int userId)
{
    std::vector<std::string> vec;
    
    // sql
    char sql[1024] = { 0 };
    sprintf(sql, "select userId, messgae from offlineMessage where userId=%d;", userId);
    // 执行
    std::shared_ptr<Connection> sqlConn = DbConnPool::getDbConnPool()->getMysqlConn();
    MYSQL_RES* res = sqlConn->query(sql);
    if(res == nullptr) {  // 没找到
        return {};
    } else {
        // 查找数据
        MYSQL_ROW row;
        while(row = mysql_fetch_row(res)) {
            std::string mess = row[0];
            mess = mess + ":" + row[1];
            vec.push_back(mess);
        }
        mysql_free_result(res);
    }

    return vec;
}