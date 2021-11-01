#pragma once
#include "XFtpTask.h"
#include <map>
class XFtpServerCmd :
	public XFtpTask
{
public:
	//初始化任务
	virtual bool Init();
	virtual void Read(struct bufferevent *bev);
	virtual void Event(struct bufferevent *bev, short what);


	//注册命令处理对象
	//不需要考虑线程安全，在调用时，还未分发到线程
	//
	void Reg(std::string cmd, XFtpTask* call);
 


	XFtpServerCmd();
	~XFtpServerCmd();

private:
	std::map<std::string, XFtpTask*> calls;
	//用于空间清理
	std::map<XFtpTask*,int> calls_del;
};

