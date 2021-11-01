#include "XFtpLIST.h"
#include <string>
#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#ifdef _WIN32
#include <io.h>
#endif

using namespace std;
//解析协议
void XFtpLIST::Write(struct bufferevent *bev)
{
	//4.发送完成
	ResponseCmd("226 Transfer complete.\r\n");
	Close();
}

void XFtpLIST::Event(struct bufferevent *bev, short what)
{
	//如果对方网络断掉，有可能收不到BEV_EVENT_EOF，
	//添加超时！！！
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


void XFtpLIST::Parse(std::string type, std::string msg)
{
	string resmsg = "";
	if (type == "PWD")
	{
		resmsg = "257 \"";
		resmsg += cmdTask->curDir;
		resmsg += "\" is current dir.\r\n";
		ResponseCmd(resmsg);
	}
	else if (type == "LIST")
	{
		
		//1. 连接数据通道
		//2. 150
		//3. 发送数据通道
		//4. 发送完成回应
		//5. 关闭连接

		//回复消息，使用数据通道发送目录
		//-rwxrwxrwx 1 root group 64463 Mar 14 09:53 101.jpg\r\n
		//1.连接数据通道

		ConnectPORT();

		//2.150
		ResponseCmd("150 Here comes the directory listing.\r\n");

		//string listdata = "-rwxrwxrwx 1 root group 64463 Mar 14 09:53 101.jpg\r\n";
		string listdata = GetListData(cmdTask->rootDir  + cmdTask->curDir);

		//3.数据通道发送
		Send(listdata);

		//4.发送完成回应
		//void XFtpLIST::Write(struct bufferevent *bev);

		//5.关闭连接
	}
	else if (type == "CWD")
	{
		//取出命令中的路径
		//CWD test\r\n


		int pos = msg.rfind(" ") + 1;
		string path = msg.substr(pos, msg.size() - pos - 2);
		if (path[0] == '/')  //绝对路径
		{
			cmdTask->curDir = path;
		}
		else
		{
			if (cmdTask->curDir[cmdTask->curDir.size() - 1] != '/')
			{
				cmdTask->curDir += "/";
			}
			cmdTask->curDir += path + "/";
		}

		ResponseCmd("250 Directory succes changed. \r\n");
	}
	else if (type == "CDUP")
	{
		string path = cmdTask->curDir;
		if (path[path.size() - 1] == '/')
		{
			path = path.substr(0, path.size() - 1);
		}
		int pos = path.rfind("/");
		if (pos < 0)
		{
			ResponseCmd("250 Directory success changed. null \r\n");
			return;
		}
		path = path.substr(0, pos);
		cmdTask->curDir = path;
		ResponseCmd("250 Directory succes changed. \r\n");
	}
}

std::string XFtpLIST::GetListData(std::string path)
{
	string data = "";
#ifdef _WIN32
	//存储文件信息
	_finddata_t file;
	//目录上下文
	path += "/*.*";
	intptr_t dir = _findfirst(path.c_str(), &file);
	if (dir < 0)
	{
		return data;
	}
	do
	{
		string tmp = "";
		//是否是目录,去掉. ..
		if (file.attrib & _A_SUBDIR)
		{
			if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0)
			{
				continue;
			}
			tmp = "drwxrwxrwx 1 root group ";
		}
		else
		{
			tmp = "-rwxrwxrwx 1 root group ";
		}
		//文件大小
		char buf[1024];
		// _CRT_SECURE_NO_WARNINGS
		sprintf(buf, "%u ", file.size);
		tmp += buf;

		//日期时间
		strftime(buf, sizeof(buf) - 1, "%b %d %H:%M ", localtime(&file.time_write));
		tmp += buf;
		tmp += file.name;
		tmp += "\r\n";
		data += tmp;
		
	} while (_findnext(dir, &file) == 0);
#else
	string cmd = "ls -l ";
	cmd += path;
	char buf[1024];
	FILE *f = popen(cmd.c_str(), "r");
	if (!f)
		return data;
	for (;;)
	{
		int len = fread(buf, 1, sizeof(buf) - 1, f);
		if (len <= 0)
			break;
		buf[len] = '\0';
		data += buf;
	}
	pclose(f);
#endif
	return data;

}