#ifndef GROUPUSER_H
#define GROUPUSER_H

#include "user.hpp"

// 描述groupuser类的信息，包括用户的基本信息和群组的角色
class GroupUser : public User
{
public:
    void setrole(string role) { this->role = role; }
    string getrole() { return this->role; }

private:
    string role;
};

#endif
