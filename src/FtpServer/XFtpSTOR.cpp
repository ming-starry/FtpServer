#include "XFtpSTOR.h"
#include <iostream>
#include <string>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
using namespace std;
//上传文件
//解析协议
void XFtpSTOR::Parse(std::string type, std::string msg)
{
	//取出文件名
	int post = msg.rfind(" ") + 1;
	string filename = msg.substr(post, msg.size() - post - 2);
	string path = cmdTask->rootDir;
	path += cmdTask->curDir;
	path += filename;
	fp = fopen(filename.c_str(), "wb");
	if (fp)
	{
		//连接数据通道
		ConnectPORT();

		//发送开始接受文件的指令
		ResponseCmd("125 Transfer start.\r\n");
		//触发读取事件
		bufferevent_trigger(bev, EV_READ, 0);
	}
	else
	{
		ResponseCmd("450 file open failed.\r\n");
	}
}

void XFtpSTOR::Read(struct bufferevent *bev)
{
	if (!fp) return;
	for (;;)
	{
		int len = bufferevent_read(bev, buf, sizeof(buf));
		if (len <= 0)
			return;
		int size = fwrite(buf, 1, len, fp);
		cout << " < " << len << " : " << size << " > " << flush;
	}

}
void XFtpSTOR::Event(struct bufferevent *bev, short what)
{
	//如果对方网络断掉，有可能收不到BEV_EVENT_EOF，
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
	{
		cout << "XFtpSTOR BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT" << endl;
		Close();
		ResponseCmd("226 Transfer complete. \r\n");
	}
	else if (what & BEV_EVENT_CONNECTED)
	{
		cout << "XFtpLIST BEV_EVENT_CONNECTED" << endl;
	}
}