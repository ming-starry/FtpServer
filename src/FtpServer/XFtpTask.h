#pragma once
#include "XTask.h"
#include <string>
class XFtpTask :
	public XTask
{
public:
	//PORT数据通道 ip和端口
	std::string ip = "";
	int port = -1;

public:
	std::string oldfilename;

public:
	std::string curDir = "/";
	std::string rootDir = ".";


	XFtpTask *cmdTask = 0;

public:
	//解析协议
	virtual void Parse(std::string type, std::string msg) {}
	//回复cmd消息
	void ResponseCmd(std::string msg);
	
	//连接数据通道
	void ConnectPORT();
	void Close();

	//用来发送建立连接的数据通道
	void Send(std::string data);
	void Send(const char *data, int datasize);

public:
	virtual void Read(struct bufferevent *bev) {}
	virtual void Write(struct bufferevent *bev) {}
	virtual void Event(struct bufferevent *bev, short what) {}
	void SetCallback(struct bufferevent *bev);
	bool Init() { return true; }

protected:
	static void ReadCB(bufferevent *bev, void *arg);
	static void WriteCB(bufferevent *bev, void *arg);
	static void EventCB(struct bufferevent *bev, short what, void *arg);

	//添加命令bev
	struct bufferevent *bev = 0;
	FILE * fp = 0;
	
};

