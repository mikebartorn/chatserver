#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"

// user表的数据操作类
class Usermodel
{
public:
    // user表的增加方法
    bool insert(User &user);
    // 根据账号查找用户
    User query(int id);
    // 更新用户信息
    bool update(User user);
    // 重置用户的登录状态信息
    void resetstate();
};

#endif
