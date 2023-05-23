#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;

class Chatserver
{
public:
    // 初始化聊天服务器对象
    Chatserver(EventLoop *loop,
               const InetAddress &listenAddr,
               const string &nameArg);
    // 开始
    void start();

private:
    // 给服务器注册用户连接的创建和断开回调
    void onConnection(const TcpConnectionPtr &conn);
    // 给服务器注册用户读写事件的回调函数
    void onMessage(const TcpConnectionPtr &conn,
                   Buffer *buffer,
                   Timestamp time);
    EventLoop *_loop;  // 指向事件循环对象的指针
    TcpServer _server; // 组合muduo库，实现服务器功能的类对象
};

#endif
