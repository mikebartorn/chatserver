#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
using namespace std;

//user数据库的映射类，存放user的数据
class User
{
public:
    User(int id = -1, string name = "", string pwd = "", string state = "offline")
    {
        this->_id = id;
        this->_name = name;
        this->_passward = pwd;
        this->_state = state;
    }

    void setid(int id) { this->_id = id; }
    void setname(string name) { this->_name = name; }
    void setpwd(string pwd) { this->_passward = pwd; }
    void setstate(string state) { this->_state = state; }

    int getid() { return this->_id; }
    string getname() { return this->_name; }
    string getpwd() { return this->_passward; }
    string getstate() { return this->_state; }

private:
    int _id;
    string _name;
    string _passward;
    string _state;
};

#endif
