# chatserver
c++集群聊天项目

必须先安装nginx和redis

安装好后开启redis和nginx
redis一般都是开启的
开启nginx： 切换root用户 sudo su 
然后  cd /usr/local/nginx/sbin  之后输入./nginx开启

查看是否开启
切换root用户， 输入 netstat -tanp查看

编译：cd build  cmake ..    make

运行：cd bin/
nginx中配置的服务器的ip地址和端口为192.168.25.128：10001和10002
nginx的ip地址和端口为192.168.25.128：10000

客户端运行：./ChatClient 192.168.25.128 10000
服务端运行：./ChatServer 192.168.25.128 10001
           ./ChatServer 192.168.25.128 10002

           
