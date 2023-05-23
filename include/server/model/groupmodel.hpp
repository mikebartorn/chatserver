#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"
#include <string>
#include <vector>

// 维护群组信息的操作方法
class GroupModel
{
public:
    // 创建群组
    bool creategroup(Group &group);
    // 加入群组
    void addGroup(int userid, int groupid, string role);
    // 查询用户所在的群组
    vector<Group> queryGroups(int userid);
    // 根据指定的groupid查询群组用户id的列表，除了useriud自己，主要用户群聊业务给群组其他成员发送消息
    vector<int> queryGroupUsers(int userid, int groupid);
};

#endif
