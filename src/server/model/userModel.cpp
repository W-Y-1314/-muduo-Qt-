#include "userModel.h"
#include "dbConnectionPool.h"
#include "connection.h"

#include <iostream>
#include <memory>

// 插入信息：创建新用户
bool UserModel::insert(User& user)
{
    char sql[BUFSIZ] = { 0 };
    sprintf(sql, "insert into User(id , name, password) values('%d' ,'%s', '%s');", user.getId(), user.getName().c_str(), user.getPwd().c_str());
    std::cout << sql << std::endl;

    // 获取连接
    std::shared_ptr<Connection> sqlConn = DbConnPool::getDbConnPool()->getMysqlConn(); 
    // 查询
    bool res = sqlConn->modify(sql);
    if(res) {
        // 设置用户id
        user.setId(mysql_insert_id(sqlConn->getMysql()));
        return true;
    } 

    return false;
}
// 获取查询结果
User UserModel::query(int id)
{
    char sql[BUFSIZ] = { 0 };
    sprintf(sql, "select * from User where id=%d;", id);

    std::shared_ptr<Connection> sqlConn = DbConnPool::getDbConnPool()->getMysqlConn(); 

    MYSQL_RES* res = sqlConn->query(sql);
    if(res != nullptr) {
        // 抓取一行信息
        MYSQL_ROW row = mysql_fetch_row(res);
        if(row != nullptr) {
            User user;
            user.setId(atoi(row[0]));
            user.setName(row[1]);
            user.setPwd(row[2]);
            user.setState(row[3]);
            mysql_free_result(res);
            return user;
        }
    }

    return User();
}

// 更新状态信息
bool UserModel::updateState(User user)
{
    char sql[BUFSIZ] = { 0 };
    sprintf(sql, "update User set state='%s' where id=%d;", user.getState().c_str(), user.getId());
    //std::cout << sql << std::endl;
    std::shared_ptr<Connection> sqlConn = DbConnPool::getDbConnPool()->getMysqlConn(); 
    if(sqlConn->modify(sql)) {
        return true;
    }

    return false;
}

// 从新设置用户状态
void UserModel::resetState()
{
    char sql[BUFSIZ] = { "update User set state='offline' where state='online';" };

    std::shared_ptr<Connection> sqlConn = DbConnPool::getDbConnPool()->getMysqlConn(); 
    sqlConn->modify(sql);
}