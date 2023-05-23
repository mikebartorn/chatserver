/*
mudup网络库提供的两个类
tcpserver:用于编写服务器程序
tcpclient:用于编写客户端程序

将epoll+线程池封装起来，将网络I/O代码和业务代码区分开
*/

#include <iostream>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <functional>
#include <string>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

/*基于muduo网络库开发服务器程序
1。组合Tcpserver对象
2.创建Eventloop事件循环对象的指针
3.明确Tcpserver的构造函数需要什么参数，输出Chatserver的构造函数
4.在当前服务器类的构造函数当中，注册处理连接的回调函数和处理读写事件的回调函数
5.设置合适的服务端线程数量，muduo库会自己分配I/O线程和worker线程
*/

class Chatserver
{
public:
    Chatserver(EventLoop *loop, const InetAddress &listenaddr, // loop事件循环
               const string &nameArg) : _server(loop, listenaddr, nameArg), _loop(loop)
    {
        // 给服务器注册用户连接的创建和断开回调
        _server.setConnectionCallback(std::bind(&Chatserver::onConnection, this, _1));

        // 给服务器注册用户读写事件的回调函数
        _server.setMessageCallback(std::bind(&Chatserver::onMessage, this, _1, _2, _3));

        // 设置服务端的线程数量 1个I/O线程，3个工作线程
        _server.setThreadNum(4);
    }

    void start()
    {
        _server.start();
    }

private:
    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected())
        {
            cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " state:online" << endl;
        }
        else
        {
            cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " state:offline" << endl;
            conn->shutdown(); // close fd
            _loop->quit();
        }
    }

    void onMessage(const TcpConnectionPtr &conn, // 连接
                   Buffer *buffer,               // 缓冲区
                   Timestamp time)
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data:" << buf << "time:" << time.toString() << endl;
        conn->send(buf);
    }

    TcpServer _server;//组合Tcpserver对象
    EventLoop *_loop;//创建EventLoop事件循环对象指针
};

int main()
{
    EventLoop loop;
    InetAddress addr("192.168.25.128", 10000);
    Chatserver server(&loop, addr, "Chatserver");

    server.start(); // listenfd通过epoll_ctl添加到epoll上
    loop.loop();    // epoll_wait阻塞方式等待新用户连接，已连接用户的读写事件
    return 0;
}
