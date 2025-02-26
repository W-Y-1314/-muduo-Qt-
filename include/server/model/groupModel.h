#ifndef GROUPMODEL_H_
#define GROUPMODEL_H_

#include "public.h"
#include "group.h"
#include "groupUser.h"

/*
提供四个功能：
    1、创建组
    2、添加组用户信息
    3、查询用户所在群组的信息
    4、查询指定组的成员ID，除了userId自己
*/

class GroupModel
{
public:  
    bool createGroup(Group& group);

    bool addGroup(int userId, int groupId, std::string role="normal");

    std::vector<Group> queryGroups(int userId);

    std::vector<int> queryRroupUsers(int userId, int groupId);
};

#endif // !GROUPMODEL_H_
