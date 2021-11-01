#include "XThread.h"
#include <thread>
#include <iostream>
#include <event2/event.h>
#include <event2/util.h>
#include "XTask.h"
using namespace std;

#ifdef _WIN32
#else
#include <unistd.h>
#endif

XThread::XThread()
{
	
}

XThread::~XThread()
{

}

//启动线程
void XThread::Start()
{
	
	Setup();
	//启动线程
	thread th(&XThread::Main, this);
	
	//断开与主线程联系
	th.detach();
}

//启动线程
void XThread::Main()
{
	cout << "XThread::Main() begin" << endl;
	event_base_dispatch(base);
	//cout << "XThread::Main() end" << endl;
}

void XThread::AddTask(XTask *t)
{
	if (!t) return;
	//XThread传递base给XTask
	t->base = this->base;
	tasks_mutex.lock();
	//tasks属于临界资源
	tasks.push_back(t);
	tasks_mutex.unlock();
}

void XThread::Activate()
{
#ifdef _WIN32
	int re = send(this->notify_send_fd, "c", 1, 0);
#else
	int re = write(this->notify_send_fd, "c", 1);
#endif
	if (re <= 0)
	{
		cerr << "XThread::Activate() failed " << endl;
	}
}

//激活线程任务的回调函数
static void NotifyCB(evutil_socket_t fd, short which, void *arg)
{
	XThread *t = (XThread *)arg;
	t->Notify(fd, which);
}

void XThread::Notify(evutil_socket_t fd, short which)
{
	//水平触发
	char buf[2] = { 0 };
#ifdef _WIN32
	int re = recv(fd, buf, 1, 0);
#else
	//linux中是管道，不能用recv
	int re = read(fd, buf, 1);
#endif
	if (re <= 0)
	{
		return;
	}
	cout << id << " thread  " << buf <<endl;

	//获取任务，并初始化任务
	XTask *task = NULL;
	tasks_mutex.lock();
	if (tasks.empty())
	{
		tasks_mutex.unlock();
		return;
	}
	task = tasks.front();  //先进先出
	tasks.pop_front();
	tasks_mutex.unlock();
	task->Init();
}

//安装线程，初始化event_base和管道监听事件用于激活
bool XThread::Setup()
{
	//windows socketpair , linux 管道
#ifdef _WIN32
	//创建一个socketpair fds[0]读 fds[1]写
	evutil_socket_t fds[2];
	if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds) < 0)
	{
		cout << "evutil_socketpair failed" << endl; 
		return false;
	}
	//设置成非阻塞
	evutil_make_socket_nonblocking(fds[0]);
	evutil_make_socket_nonblocking(fds[1]);
#else
	//创建管道，不能用send recv，读取read write
	int fds[2];
	if (pipe(fds))
	{
		cerr << "pipe failed" << endl;
		return false;
	}
#endif 

	//读取绑定到event事件，写入保存

	notify_send_fd = fds[1];

	//创建libevent上下文，（无锁）
	event_config *ev_config = event_config_new();

	event_config_set_flag(ev_config, EVENT_BASE_FLAG_NOLOCK);
	//每个线程都含有一个event_base
	this->base = event_base_new_with_config(ev_config);
	event_config_free(ev_config);
	if (!base)
	{
		cerr << "event_base_new_with_config failed in thread!" << endl;
		return false;
	}

	//添加管道监听事件，用于激活线程执行任务
	event *ev = event_new(base, fds[0], EV_READ | EV_PERSIST, NotifyCB, this);
	event_add(ev, 0);

	return true;
}


