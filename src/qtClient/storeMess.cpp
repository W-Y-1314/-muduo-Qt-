#include "storeMess.h"

StoreMess::StoreMess()
{
    // 初始化信号量
    sem_init(&m_loginSem, 0, 0);  // 初始值为0
    sem_init(&m_registerSem, 0, 0);
    sem_init(&m_addFriendSem, 0, 0);
}

StoreMess* StoreMess::getInstall()
{
    static StoreMess mess;
    return &mess;
}