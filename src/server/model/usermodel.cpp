#include <iostream>
#include "usermodel.hpp"
#include "database.hpp"
#include <string>
using namespace std;

// 往sql里面增加注册用户数据
bool Usermodel::insert(User &user)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name, password, state) values('%s', '%s', '%s')",
            user.getname().c_str(), user.getpwd().c_str(), user.getstate().c_str());
    // 连接数据库更新数据库
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            user.setid(mysql_insert_id(mysql.getconnection()));
            return true;
        }
    }
    return false;
}

// 根据账号查找用户信息
User Usermodel::query(int id)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d", id);
    // 连接数据库更新数据库
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.setid(atoi(row[0]));
                user.setname(row[1]);
                user.setpwd(row[2]);
                user.setstate(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }
    return User();
}

// 更新用户信息
bool Usermodel::update(User user)
{
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where id = %d",
            user.getstate().c_str(), user.getid());
    // 连接数据库更新数据库
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}

// 重置用户的登录状态信息
void Usermodel::resetstate()
{
    // 组装sql语句
    char sql[1024] = "update user set state = 'offline' where state = 'online'";
    // 连接数据库更新数据库
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}
