#ifndef FRIENDMODEL_H_
#define FRIENDMODEL_H_

#include "public.h"
#include "user.h"

class FriendModel
{
public:
    // 增加好友
    bool insert(int userId, int friendId);
    // 删除好友
    void remove(int userId, int friendId);
    // 查询好友
    std::vector<User> query(int userId);
};

#endif // !FRIENDMODEL_H_

