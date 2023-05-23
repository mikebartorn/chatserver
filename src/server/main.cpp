#include "chatserver.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <signal.h>
using namespace std;

// 处理服务器异常退出，重置用户的状态信息
void resetHanlder(int)
{
    Chatservice::instance()->reset();
    exit(0);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cerr << "command invaild! expamle: ./ChatServer 192.168.25.128 10000" << endl;
        exit(0);
    }

    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    //捕捉服务端ctrl+c退出信号
    signal(SIGINT, resetHanlder);
    EventLoop loop;
    InetAddress addr(ip, port);
    Chatserver server(&loop, addr, "Chatserver");

    server.start();
    loop.loop();
    return 0;
}
