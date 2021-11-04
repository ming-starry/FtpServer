#include <event2/event.h>
#include <event2/listener.h>
#include <string.h>
#include "XThreadPool.h"
#include "XFtpFactory.h"
#ifndef _WIN32
#include <signal.h>
#endif
#include <iostream>
using namespace std;

#define SPORT 21

//XFtpTask连接超时
//XFtpServerCmd命令超时


void listen_cb(struct evconnlistener *e, evutil_socket_t s , struct sockaddr *a , int socklen, void * arg)
{
	cout << "listen" << endl;
	//task为XFtpServerCmd
	XTask *task = XFtpFactory::Get()->CreatTask();
	task->sock = s;
	//task类型为 XFtpServerCmd *
	XThreadPool::Get()->Dispatch(task);
}

int main()
{


#ifdef _WIN32
	//初始化socket库
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,2) ,&wsa);
#else
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)  //忽略管道信号，发送数据给已关闭的socket
	{
		return 1;
	}
#endif 

	//初始化线程池
	XThreadPool::Get()->Init(10);
	

	//创建libevent上下文
	event_base * base = event_base_new();
	if (base)
	{
		cout << "event_base_new success!" << endl;
	}
	//监听端口
	//socket bind listen
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SPORT);

	evconnlistener *ev = evconnlistener_new_bind(base,  //libevent的上下文
		listen_cb,								//接收到连接的回调函数
		base,									// 回调函数获取的参数arg
		LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, //地址重用，listen关闭同时清理socket
		10,								//连接队列大小，对应listen函数
		(sockaddr *)&sin,				//绑定的地址和端口
		sizeof(sin)
		);
	//事件分发处理
	if (base)
	{
		event_base_dispatch(base);
	}
	
	if (ev)
	{
		evconnlistener_free(ev);

	}

	if (base) 
	{
		event_base_free(base);
	}
	//销毁线程
	XThreadPool::Get()->Uninit();

#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}

