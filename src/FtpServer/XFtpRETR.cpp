#include "XFtpRETR.h"
#include <string>
#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
using namespace std;


void XFtpRETR::Write(struct bufferevent *bev)
{
	if (!fp)
	{
		return; 
	}

	int len = fread(buf, 1, sizeof(buf), fp);
	if (len <= 0)
	{
		ResponseCmd("226 Transfer complete.\r\n");
		Close();
		return;
	}
		
	cout << "[" << len << "]" << flush;
	Send(buf, len);

}
void XFtpRETR::Event(struct bufferevent *bev, short what)
{
	//如果对方网络断掉，有可能收不到BEV_EVENT_EOF，
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
	{
		cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT" << endl;
		Close();
		this->bev = 0;
	}
	else if (what & BEV_EVENT_CONNECTED)
	{
		cout << "XFtpLIST BEV_EVENT_CONNECTED" << endl;
	}
}


void XFtpRETR::Parse(std::string type, std::string msg)
{
	//取出文件名
	//RETR filepath\r\n
	int post = msg.rfind(" ") + 1;
	string filename = msg.substr(post, msg.size() - post - 2);
	string path = cmdTask->rootDir;
	path += cmdTask->curDir;
	path += filename;
	fp = fopen(path.c_str(), "rb");
	if (fp)
	{
		//连接数据通道
		//发送开始下载文件的指令
		ConnectPORT();
		ResponseCmd("150 Transfer start.\r\n");
		//触发写入事件
		bufferevent_trigger(bev, EV_WRITE, 0);
	}
	else
	{
		ResponseCmd("450 file open failed.\r\n");
	}

}