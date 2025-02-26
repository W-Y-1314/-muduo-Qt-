#ifndef RESCULTCODE_H_
#define RESCULTCODE_H_

#include "public.h"
#include <string>


// 消息类型
enum MsgType
{
    LOGIN_MSG = 1,          // 登录消息 1
    LOGIN_MSG_ACK,          // 登录响应消息
    REGISTER_MSG,           // 注册消息   3
    REGISTER_MSG_ACK,       // 注册响应消息
    ONE_CHAT_MSG,           // 一对一聊天信息   5
    ONE_CHAT_MSG_ACK,       // 一对一聊天信息响应
    ADD_FREIEND_MSG,        // 添加好友信息  7
    ADD_FRIEND_MSG_ACK,     // 添加好友信息响应
    CREATE_GROUP_MSG,       // 创建群消息   9
    CREATE_GROUP_MSG_ACK,   // 创建群响应消息
    ADD_GROUP_MSG,          // 添加群消息  11
    ADD_GROUP_MSG_ACK,      // 添加群响应
    GROUP_CHAT_MSG,         // 群聊天消息  13
    GROUP_CHAT_MSG_ack,     // 群聊天响应
};

class ResultCode
{
    // 常用返回编码
    struct Code 
    {
        int _code;   // 消息码
        const char* _codeStr;   // 消息码解释
        const char* _messgae;   // 消息解释

        Code(int code, const char* codeStr, const char* message)
            :_code(code), _codeStr(codeStr), _messgae(message)
        {

        }

        std::string toJson(int msgId) const
        {
            json js;
            js["msgId"] = msgId;
            js["code"] = _code;
            js["message"] = _messgae;

            return js.dump();
        }
    };
public: // 封装消息不同种类
#define CODE(name,value, message) inline static Code name{value,#name, message}

    // 成功
    CODE(Success, 0, "success~~~~");
    CODE(HaveLogin, 1, "the user have logined!!");  // 用户已经登录
    CODE(LoginSuccess, 2, "the user login success!!");      // 登录成功
    CODE(RegisterSuccess, 3, "new user registers success!!");   // 注册成功
    CODE(OneChatSuccess, 5, "one to noe chat success!!");    // 一对一聊天成功
    CODE(AddFriendSuccess, 6, "to add the friend do success!!");  // 添加好友成功
    CODE(CreateGroupSuccess, 7, "to create the group do success!!");  // 创建组成功
    CODE(AddGroupSuccess, 8, "to add the group do success!!");  // 加入组成功

    // 失败,[1000, 无穷]
    CODE(LoginFailure, 1001, "the user logins failure!!");  // 登录失败
    CODE(RegisterFailure, 1002, "new user registers failure!!");  // 注册失败
    CODE(ChatUserNotOnline, 1003, "the user not online!!!");   // 不存在该用户
    CODE(AddFriendFailure, 1004, "to add the friend du failure!!");  // 添加好友失败
    CODE(CreateGroupFailure, 1005, "to create the group failure!!");  // 创建组失败
    CODE(AddGroupFailure, 1006, "to add the group do failure!!"); // 加入组失败

#undef CODE
    // 友元类
    friend class Result;
};

class Result
{
public: 
    // 声明静态变量，可以不用实例化类使用，因为静态变量在全局区

    // 成功
    static std::string success(int msgId, const ResultCode::Code& code)
    {
        return code.toJson(msgId);
    }
    static std::string success(int msgId, const ResultCode::Code& code, const json& data) 
    {
        json js;
        js["msgId"] = msgId;
        js["code"] = code._code;
        js["codeStr"] = code._codeStr;
        js["message"] = code._messgae;
        js["data"] = data;

        return js.dump();
    }
    // 失败
    static std::string failure(int msgId ,const ResultCode::Code& code)
    {
        return code.toJson(msgId);
    }
};

#endif // !RESCULTCODE_H_