#ifndef GROUPUSER_H_
#define GROUPUSER_H_

#include "user.h"
#include "public.h"

// 组用户信息，继承User
/*
提供set、get方法：设置/得到角色，其他信息都在group、user中
*/
class GroupUser : public User
{
public:  
    void setRole(std::string role) { m_role = role; }
    std::string getRole() { return m_role; }

private:    
    std::string m_role;
};

#endif // !GROUPUSER_H_
