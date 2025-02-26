#ifndef LOG_H_
#define LOG_H_

#include <iostream>

// 作用：封装简单的LOG
#define LOGGER(str) std::cout << "====》" << __LINE__ << " time: " << __TIME__ << " message: " << str << std::endl;

#endif // !LOG_H_

