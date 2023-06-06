#include "chatserver.hpp"
#include <iostream>
#include <functional>
#include <string>
#include <json.hpp>
#include "chatservice.hpp"
using namespace std;
using namespace placeholders;
using json = nlohmann::json;

// 初始化聊天服务器对象
Chatserver::Chatserver(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg) : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册链接回调函数
    _server.setConnectionCallback(std::bind(&Chatserver::onConnection, this, _1));
    // 注册消息回调函数
    _server.setMessageCallback(std::bind(&Chatserver::onMessage, this, _1, _2, _3));
    // 设置线程数量
    _server.setThreadNum(4);
}

// 开始
void Chatserver::start()
{
    _server.start();
}

// 给服务器注册用户连接的创建和断开回调
void Chatserver::onConnection(const TcpConnectionPtr &conn)
{
    // 客户端断开连接
    if (!conn->connected())
    {
        Chatservice::instance()->clientCloseException(conn);
        conn->shutdown();
        _loop->quit();
    }
}
// 给服务器注册用户读写事件的回调函数
void Chatserver::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buffer,
                           Timestamp time)
{
    string buf = buffer->retrieveAllAsString();
    // 数据的反序列化
    json js = json::parse(buf);
    // 完全解耦网络模块的代码
    // 通过js["msgid"]消息的类型，获取->相关业务handler的处理函数
    auto msgHandler = Chatservice::instance()->getHandler(js["msgid"].get<int>());
    // 回调消息绑定好的事件处理器，来执行相应的业务处理
    msgHandler(conn, js, time);
}
