#ifndef OFFLINEMESSMODEL_H_
#define OFFLINEMESSMODEL_H_

/*
存储离线消息：
    1、存储用户离线消息
    2、删除用户离线消息
    3、查询用户离线消息(vector)
*/

#include "public.h"

class OfflineMessModel
{
public: 
    // 存储用户离线消息
    bool insert(int userId ,std::string msg);

    // 删除用户离线信息
    bool remove(int userId);

    // 查询离线信息
    std::vector<std::string> query(int userId);

};

#endif // !OFFLINEMESSMODEL_H_
