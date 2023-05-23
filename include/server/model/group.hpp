#ifndef GROUP_H
#define GROUP_H

#include <string>
#include <iostream>
#include <vector>
#include "groupuser.hpp"
using namespace std;

// 聊天群组类，记录allgroup表的信息
class Group
{
public:
    Group(int id = -1, string name = "", string desc = "")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
    }
    // 设置信息
    void setid(int id) { this->id = id; }
    void setname(string name) { this->name = name; }
    void setdesc(string desc) { this->desc = desc; }
    // 获取信息
    int getid() { return this->id; }
    string getname() { return this->name; }
    string getdesc() { return this->desc; }
    vector<GroupUser> &getusers() { return this->users; }

private:
    int id;
    string name;
    string desc;
    vector<GroupUser> users;
};

#endif
