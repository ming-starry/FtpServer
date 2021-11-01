#include "XFtpDELE.h"
#include <string>
#include <stdio.h>
using namespace std;

void XFtpDELE::Parse(std::string type, std::string msg)
{
	//取出文件名
	//DELE 122.txt
	int post = msg.rfind(" ") + 1;
	string filename = msg.substr(post, msg.size() - post - 2);
	remove(filename.c_str());
	string res;
	res += "250 DELE " + filename + " success.\r\n";
	ResponseCmd(res);
}