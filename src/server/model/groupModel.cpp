#include "groupModel.h"
#include "public.h"
#include "group.h"
#include "user.h"

// 创建组
bool GroupModel::createGroup(Group& group)
{
    char sql[1024] = { 0 };
    sprintf(sql, "insert into allGroup(groupname, groupdesc) values('%s', '%s');", group.getName().c_str(), group.getDesc().c_str());
    if(DbConnPool::getDbConnPool()->getMysqlConn()->modify(sql)) {
        return true;
    }

    return false;
}

// 加入组
bool GroupModel::addGroup(int userId, int groupId, std::string role)
{
    char sql[1024] = { 0 };
    sprintf(sql, "insert into groupUser(groupId, userId, groupRole) values(%d, %d, '%s');", groupId, userId, role.c_str());

    DbConnPool::getDbConnPool()->getMysqlConn()->modify(sql);

    return true;
}

// 查询所有组信息
std::vector<Group> GroupModel::queryGroups(int userId)
{
    /*
    1. 先根据userid在groupuser表中查询出该用户所属的群组信息
    2. 在根据群组信息，查询属于该群组的所有用户的userid，并且和user表进行多表联合查询，查出用户的详细信息
    */

    std::vector<Group> vec; 

    // 查组具体信息
    char sql[1024] = { 0 };
    sprintf(sql, "select a.id, a.groupname, a.groupdesc from groupUser g inner join allGroup a on g.groupId=a.id where g.userId=%d;", userId);

    MYSQL_RES* res = DbConnPool::getDbConnPool()->getMysqlConn()->query(sql);
    if(res != nullptr) {
        MYSQL_ROW row;
        while(row = mysql_fetch_row(res)) {
            Group g;
            g.setId(atoi(row[0]));
            g.setName(row[1]);
            g.setDesc(row[2]);
            vec.push_back(g);
        }
        mysql_free_result(res);
    }

    // 查每一个组的 所有用户信息
    char sql2[1024] = { 0 };
    
    for(auto& it : vec) {
        sprintf(sql2, "select u.id, u.name, u.state, g.groupRole from User u inner join groupUser g on u.id=g.userId where g.groupId=%d;", it.getId());
        res = DbConnPool::getDbConnPool()->getMysqlConn()->query(sql2);
        if(res != nullptr) {
            MYSQL_ROW row;
            while(row = mysql_fetch_row(res)) {
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);
                // 加入每组用户中
                it.getGroupUser().push_back(user);
            }   
            mysql_free_result(res);
        }
    }

    return vec;
}

std::vector<int> GroupModel::queryRroupUsers(int userId, int groupId)
{
    char sql[1024] = { 0 };
    sprintf(sql, "select userId from groupUser where userId != %d and groupId = %d;", userId, groupId);

    std::vector<int> vec;
    MYSQL_RES* res = DbConnPool::getDbConnPool()->getMysqlConn()->query(sql);   
    if(res != nullptr) {
        MYSQL_ROW row;
        while(row = mysql_fetch_row(res)) {
            vec.push_back(atoi(row[0]));
        }
        mysql_free_result(res);
    }
    return vec;
}