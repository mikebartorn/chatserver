#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include "redis.hpp"
using namespace std;
using namespace muduo;
using namespace muduo::net;

#include "json.hpp"
using json = nlohmann::json;

// 表示处理消息的事件回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;

// 聊天服务器业务类
class Chatservice
{
public:
    // 单例模式
    static Chatservice *instance();

    // 处理登录业务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp);

    // 处理注册业务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp);

    // 获取消息对应的处理回调函数
    MsgHandler getHandler(int msgid);

    // 处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);

    // 处理服务器异常退出
    void reset();

    // 一对一聊天
    void Onechat(const TcpConnectionPtr &conn, json &js, Timestamp);

    // 添加好友业务
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp);

    // 创建群组业务
    void creategroup(const TcpConnectionPtr &conn, json &js, Timestamp);

    // 加入群组
    void addgroup(const TcpConnectionPtr &conn, json &js, Timestamp);

    // 群组聊天业务
    void groupchat(const TcpConnectionPtr &conn, json &js, Timestamp);

    // 处理注销业务
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp);

    // 处理redis上报消息回调
    void handleRedisSubscribeMessage(int id, string message);

private:
    // 聊天服务初始化
    Chatservice();

    // 存储消息id和对应的业务处理的方法
    unordered_map<int, MsgHandler> _msgHandlerMap;

    // 存储用户id和对应的连接信息
    unordered_map<int, TcpConnectionPtr> _userConnMap;

    // 定义互斥锁，保证_userConnMap的线程安全
    mutex _connMutex;

    // 用户表操作类
    Usermodel _usermodel;
    OfflinemsgModel _offlinemodel;
    FriendModel _friendmodel;
    GroupModel _groupmodel;

    // redis操作对象
    Redis _redis;
};

#endif
