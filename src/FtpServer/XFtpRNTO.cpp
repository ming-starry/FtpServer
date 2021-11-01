#include "XFtpRNTO.h"
#include <string>
#include <stdio.h>
#include <iostream>
using namespace std;
void XFtpRNTO::Parse(std::string type, std::string msg)
{
	//取出新文件名
	//RNTO rfc959.txt
	int post = msg.rfind(" ") + 1;
	string newfilename = msg.substr(post, msg.size() - post - 2);
	
	if (!rename(cmdTask->oldfilename.c_str(), newfilename.c_str()))
	{
		cout << "Rename failed." << endl;
	}
	string res;
	res = "250 Rename newname " + newfilename + " success.\r\n";
	ResponseCmd(res);
}