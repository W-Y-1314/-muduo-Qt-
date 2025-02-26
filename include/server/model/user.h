#ifndef USER_H_
#define USER_H_

#include <string>

class User
{
public:
    User(int id = -1, std::string name = "", std::string pwd = "", std::string state="offline")
        :m_id(id), m_pwd(pwd), m_name(name), m_state(state)
    {
    }

    int getId()const { return m_id; }
    std::string getName()const { return m_name; }
    std::string getPwd()const {  return m_pwd; }
    std::string getState()const { return m_state; }

    void setId(int id) { m_id = id; }
    void setName(std::string name) { m_name = name; }
    void setPwd(std::string pwd) { m_pwd = pwd; }
    void setState(std::string state) { m_state = state; }

private: 
    int m_id;
    std::string m_name;
    std::string m_pwd;
    std::string m_state;
};

#endif // !USER_H_
