#include "chatservice.hpp"
#include "public.hpp"
#include <iostream>
#include <functional>
#include <muduo/base/Logging.h>
using namespace placeholders;
using namespace std;
using namespace muduo;

// 单例模式
Chatservice *Chatservice::instance()
{
    static Chatservice chatservice;
    return &chatservice;
}

// 处理登录业务 输入id，password
void Chatservice::login(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    // 获取登录的账号与密码
    int id = js["id"];
    string pwd = js["password"];

    // 根据账号查找mysql中的用户
    User user = _usermodel.query(id);
    if (user.getid() == id && user.getpwd() == pwd)
    {
        if (user.getstate() == "online")
        {
            // 已经登录
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "该账号已经登陆，请重新输入新的账号！";
            conn->send(response.dump());
        }
        else
        {
            // 登录成功，记录用户连接信息
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }

            // id用户登录成功后，需要订阅通道
            _redis.subscribe(id);

            // 修改登录状态,并更新到mysql中
            user.setstate("online");
            _usermodel.update(user);

            // 登录成功
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getid();
            response["name"] = user.getname();

            // 查询用户是否有离线消息
            vector<string> vec;
            vec = _offlinemodel.query(id);
            if (!vec.empty())
            {
                response["offlinemsg"] = vec;
                // 删除离线消息
                _offlinemodel.remove(id);
            }

            // 查询用户的好友信息
            vector<User> friendvec;
            friendvec = _friendmodel.query(id);
            if (!friendvec.empty())
            {
                vector<string> vec2;
                for (auto &user : friendvec)
                {
                    json js;
                    js["id"] = user.getid();
                    js["name"] = user.getname();
                    js["state"] = user.getstate();
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2;
            }

            // 查询用户的群组信息
            vector<Group> groupvec = _groupmodel.queryGroups(id);
            if (!groupvec.empty())
            {
                // group包含的内容：groupid,groupname,groupdesc, groupuser列表信息
                vector<string> groupv;
                for (auto &group : groupvec)
                {
                    json groupjs;
                    groupjs["id"] = group.getid();
                    groupjs["groupname"] = group.getname();
                    groupjs["groupdesc"] = group.getdesc();
                    vector<string> userv;
                    for (auto &user : group.getusers())
                    {
                        json userjs;
                        userjs["id"] = user.getid();
                        userjs["name"] = user.getname();
                        userjs["state"] = user.getstate();
                        userjs["role"] = user.getrole();
                        userv.push_back(userjs.dump());
                    }
                    groupjs["users"] = userv;
                    groupv.push_back(groupjs.dump());
                }
                response["groups"] = groupv;
            }

            conn->send(response.dump());
        }
    }
    else
    {
        // 登录失败
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "用户名或者密码错误";
        conn->send(response.dump());
    }
}

// 处理注册业务 输入name 和password
void Chatservice::reg(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setname(name);
    user.setpwd(pwd);

    bool state = _usermodel.insert(user);
    if (state)
    {
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getid();
        conn->send(response.dump());
    }
    else
    {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        response["id"] = user.getid();
        conn->send(response.dump());
    }
}

// 获取消息对应的处理回调函数
MsgHandler Chatservice::getHandler(int msgid)
{
    // 记录错误日志，msgid没有对应的事件处理回调函数
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        // 返回一个默认的处理函数，空操作
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp)
        {
            LOG_ERROR << "msgid: " << msgid << " can not find handler!";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}

// 聊天服务初始化
Chatservice::Chatservice()
{
    // 用户名基本的业务管理相关事件处理回调函数
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&Chatservice::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&Chatservice::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&Chatservice::Onechat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&Chatservice::addFriend, this, _1, _2, _3)});
    _msgHandlerMap.insert({LOGINOUT_MSG, std::bind(&Chatservice::loginout, this, _1, _2, _3)});

    // 群组业务相关事件处理回调
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&Chatservice::creategroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&Chatservice::addgroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&Chatservice::groupchat, this, _1, _2, _3)});

    // 连接redis服务器
    if (_redis.connect())
    {
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&Chatservice::handleRedisSubscribeMessage, this, _1, _2));
    }
}

// 处理客户端异常关闭代码
void Chatservice::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        // 删除map表中的用户连接信息
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); it++)
        {
            if (it->second == conn)
            {
                user.setid(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    // 用户注销，需要从redis中取消订阅
    _redis.unsubscribe(user.getid());

    // 更新用户的状态信息
    if (user.getid() != -1)
    {
        user.setstate("offline");
        _usermodel.update(user);
    }
}

// 处理服务器异常退出
void Chatservice::reset()
{
    _usermodel.resetstate();
}

// 处理一对一聊天业务 输入from（name），to（账号），msg（发送的消息）
void Chatservice::Onechat(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int toid = js["toid"].get<int>();
    // 根据要发送的账号id，再连接map中查找登录状态
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            // 用户处于登录状态
            it->second->send(js.dump());
            return;
        }
    }

    // 查询toid是否在线
    User user = _usermodel.query(toid);
    if (user.getstate() == "online")
    {
        // 用户在线，发布消息
        _redis.publish(toid, js.dump());
        return;
    }

    // 用户不在线，存储离线消息
    _offlinemodel.insert(toid, js.dump());
}

// 添加好友业务  msgid, id, friendid
void Chatservice::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    // 插入好友id关系
    _friendmodel.insert(userid, friendid);
}

// 创建群组业务
void Chatservice::creategroup(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    // 用户创建的群组
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    // 存储新创建的群组信息
    Group group(-1, name, desc);
    if (_groupmodel.creategroup(group))
    {
        // 存储群组创始人信息
        _groupmodel.addGroup(userid, group.getid(), "creator");
    }
}

// 加入群组
void Chatservice::addgroup(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupmodel.addGroup(userid, groupid, "normal");
}

// 群组聊天业务
void Chatservice::groupchat(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    // 查询群组中的所有成员
    vector<int> groupvec = _groupmodel.queryGroupUsers(userid, groupid);

    // 向所有成员发送消息
    lock_guard<mutex> lock(_connMutex);
    for (auto toid : groupvec)
    {
        // 根据要发送的账号id，再连接map中查找登录状态
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            // 在线发送消息
            it->second->send(js.dump());
        }
        else
        {
            // redis查询用户是否在线
            User user = _usermodel.query(toid);
            if (user.getstate() == "online")
            {
                _redis.publish(toid, js.dump());
            }
            else
            {
                // 用户不在线，存储离线消息
                _offlinemodel.insert(toid, js.dump());
            }
        }
    }
}

// 处理注销业务
void Chatservice::loginout(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int userid = js["id"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);
        }
    }

    // 用户注销，需要从redis中取消订阅
    _redis.unsubscribe(userid);

    User user;
    user.setid(userid);
    user.setstate("offline");
    _usermodel.update(user);
}

// 从redis消息队列中获取订阅的消息
void Chatservice::handleRedisSubscribeMessage(int id, string message)
{
    // id订阅的通道，msg通道获取的消息
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(id);
    if (it != _userConnMap.end())
    {
        it->second->send(message);
        return;
    }

    // 如果正好此时用户离线了，存储离线消息
    _offlinemodel.insert(id, message);
}
