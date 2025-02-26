#include "friendModel.h"
#include "user.h"
#include "public.h"

// 增加好友
bool FriendModel::insert(int userId, int friendId)
{
    char sql[1024] = { 0 };
    sprintf(sql, "insert into friend(userId, friendId) values(%d, %d);", userId, friendId);

    if(DbConnPool::getDbConnPool()->getMysqlConn().get()->modify(sql)) {
        return true;
    }
    return false;
}
// 删除好友
void FriendModel::remove(int userId, int friendId)
{
    // delete from friend where userId=1;
    char sql[1024] = { 0 };
    sprintf(sql, "delete from friend where userId=%d and friendId=%d;", userId, friendId);

    DbConnPool::getDbConnPool()->getMysqlConn().get()->modify(sql);

}
// 查询好友
std::vector<User> FriendModel::query(int userId)
{
    char sql[1024] = { 0 };
    sprintf(sql, "select u.id, u.name, u.state from User u inner join friend f on u.id = f.friendId where f.userId=%d;", userId);

    std::vector<User> vec;
    MYSQL_RES* res = DbConnPool::getDbConnPool()->getMysqlConn()->query(sql);
    if(nullptr == res) {
        return {};
    } 

    MYSQL_ROW row;
    while(row = mysql_fetch_row(res)) {
        User user;
        user.setId(atoi(row[0]));
        user.setName(row[1]);
        user.setState(row[2]);
        vec.push_back(user);
    }
    mysql_free_result(res);

    return vec;
}