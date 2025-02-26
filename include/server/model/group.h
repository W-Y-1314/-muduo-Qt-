#ifndef GROUP_H_
#define GROUP_H_

#include "public.h"
#include "groupUser.h"

// allGroup 表的ORM框架
class Group
{
public: 
    Group(int id=-1, std::string name="", std::string desc="") :m_groupId(id), m_groupName(name), m_groupDesc(desc) {}

    // get方法
    int getId() { return m_groupId; }
    std::string getName() { return m_groupName; }
    std::string getDesc() { return m_groupDesc; }

    // set方法
    void setId(int id) { m_groupId = id; }
    void setName(std::string name) { m_groupName = name; }
    void setDesc(std::string desc) { m_groupDesc = desc; }

    // 得到组内所有人信息：组id、人id、人信息、组信息
    std::vector<GroupUser>& getGroupUser() { return m_groupUser; }

private: 
    int m_groupId;
    std::string m_groupName;
    std::string m_groupDesc;  // 组描述
    std::vector<GroupUser> m_groupUser;   // 存储这个组用户信息
};

#endif // !GROUP_H_
