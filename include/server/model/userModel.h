#ifndef USERMODEL_H_
#define USERMODEL_H_

#include "user.h"
#include <mysql/mysql.h>

/* 具体根据实际需求提供，这里提供：
    插入
    查询
    更新账号状态
    从新设置用户状态，如：服务器关闭
*/

class UserModel
{
public: 
    // 插入信息：创建新用户
    bool insert(User& user);

    // 获取查询结果
    User query(int id);

    // 更新状态信息
    bool updateState(User user);

    // 从新设置用户状态
    void resetState();
};

#endif // !USERMODEL_H_
