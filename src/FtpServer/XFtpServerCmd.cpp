#include "XFtpServerCmd.h"
#include <iostream>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <string.h>
#include <stdlib.h>
#include <string>
using namespace std;
//初始化任务,运行在子线程中

void XFtpServerCmd::Reg(std::string cmd, XFtpTask* call)
{
	if (!call)
	{
		cout << " XFtpServerCmd::Reg call is null " << endl;
		return;
	}
	if (cmd.empty())
	{
		cout << " XFtpServerCmd::Reg cmd is null " << endl;
		return;
	}

	//已经注册的是否覆盖，，不覆盖，提示错误
	if (calls.find(cmd) != calls.end())
	{
		cout << cmd << "is already register" << endl;
		return;
	}
	calls[cmd] = call;
	calls_del[call] = 0;

}


void XFtpServerCmd::Read(struct bufferevent *bev)
{
	char data[1024] = { 0 };
	for (;;)
	{
		int len = bufferevent_read(bev, data, sizeof(data) - 1);
		if (len <= 0) break;
		data[len] = '\0';
		cout << "Recv Cmd:" << data << flush;

		//分发到处理对象
		//分析出类型 USER anonymous
		string type = "";
		for (int i = 0; i < len; i++)
		{
			if (data[i] == ' '|| data[i] == '\r')
			{
				break;
			}
			type += data[i];
		}
		cout << "type is [" << type  << "]"<< endl;
		if (calls.find(type) != calls.end())
		{
			XFtpTask *t = calls[type];
			//XFtpServerCmd,  用来维护目录，记录旧文件名称
			t->cmdTask = this;   //用来处理回复命令和目录以及修改名称
			t->ip = ip;   //【2】.数据通道
			t->port = port;
			t->base = base;

			//假设type="PORT"，XFtpPORT会率先获取ip,port;此时先赋值给XFtpTask
			//当type="LIST"时，XFtpLIST就获得了用于数据通道连接的ip和port
			t->Parse(type, data);
			if (type == "PORT")
			{
				ip = t->ip;   //【1】.命令通道率先获取ip和port
				port = t->port;//
			}
		}
		else
		{
			string msg = "200 OK\r\n";
			bufferevent_write(bev, msg.c_str(), msg.size());
		}
	}
}

void XFtpServerCmd::Event(struct bufferevent *bev, short what)
{
	//如果对方网络断掉，有可能收不到BEV_EVENT_EOF，
	//添加超时！！！
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
	{
		cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT" << endl;
		delete this;
	}
}


bool XFtpServerCmd::Init()
{
	cout << "XFtpServerCmd::Init()" << endl;
	//监听socket bufferevent
	//base socket
	//释放 bufferevent 时关闭底层传输端口。这将关闭底层套接字,释放底层 bufferevent 等
	bufferevent *bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		delete this; //释放XFtpServerCmd
		return false;
	}
	this->bev = bev;
	//设置回调！！！
	this->SetCallback(bev);

	//添加超时！！！
	timeval rt = { 60 ,0 };
	bufferevent_set_timeouts(bev, &rt,NULL);
	string msg = "220 welcome to XFtpServer\r\n";
	bufferevent_write(bev, msg.c_str(), msg.size());
	return true;
}

XFtpServerCmd::XFtpServerCmd()
{

}
XFtpServerCmd::~XFtpServerCmd()
{
	//释放bev
	Close();
	for (auto ptr = calls_del.begin(); ptr != calls_del.end(); ptr++)
	{
		ptr->first->Close();
		delete ptr->first;
	}
}