#include "groupmodel.hpp"
#include "database.hpp"
#include "group.hpp"
#include <iostream>
using namespace std;

// 创建群组
bool GroupModel::creategroup(Group &group)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup(groupname, groupdesc) values('%s', '%s')",
            group.getname().c_str(), group.getdesc().c_str());
    // 连接数据库更新数据库
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            group.setid(mysql_insert_id(mysql.getconnection()));
            return true;
        }
    }
    return false;
}

// 加入群组
void GroupModel::addGroup(int userid, int groupid, string role)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser values(%d, %d, '%s')",
            groupid, userid, role.c_str());
    // 连接数据库更新数据库
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 查询用户所在的群组信息，包括id、name、desc、user信息
vector<Group> GroupModel::queryGroups(int userid)
{
    // 现根据userid再groupuser表中查询该用户所属的群组信息
    // 再根据群组信息，查询属于该群组的所有用户的userid，并且和userid表进行多表联合查询，查出用户的详细信息
    char sql[1024] = {0};
    sprintf(sql, "select a.id, a.groupname, a.groupdesc from allgroup a inner join groupuser b on a.id = b.groupid where b.userid=%d", userid);
    vector<Group> groupvec;
    // 连接数据库更新数据库
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            // 把userid用户的所有离线消息全部放入vec中
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                Group group;
                group.setid(atoi(row[0]));
                group.setname(row[1]);
                group.setdesc(row[2]);
                groupvec.push_back(group);
            }
            mysql_free_result(res);
        }
    }

    // 对group对象的user列表的信息进行查询存储
    for (auto &group : groupvec)
    {
        sprintf(sql, "select a.id, a.name, a.state, b.grouprole from user a inner join groupuser b on b.userid = a.id where b.groupid=%d", group.getid());
        MySQL mysql;
        if (mysql.connect())
        {
            MYSQL_RES *res = mysql.query(sql);
            if (res != nullptr)
            {
                // 把userid用户的所有离线消息全部放入vec中
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(res)) != nullptr)
                {
                    GroupUser user;
                    user.setid(atoi(row[0]));
                    user.setname(row[1]);
                    user.setstate(row[2]);
                    user.setrole(row[3]);
                    group.getusers().push_back(user);
                }
                mysql_free_result(res);
            }
        }
    }
    return groupvec;
}

// 根据指定的groupid查询群组用户id的列表，除了useriud自己，主要用户群聊业务给群组其他成员发送消息
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where userid != %d and groupid = %d", userid, groupid);
    vector<int> vec;
    // 连接数据库更新数据库
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            // 把userid用户的所有离线消息全部放入vec中
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                vec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return vec;
}
