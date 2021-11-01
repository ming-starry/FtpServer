#include "XFtpRNFR.h"
#include <string>
using namespace std;
void XFtpRNFR::Parse(std::string type, std::string msg)
{
	//取出旧文件名,存放于name
	//RNFR rfc959.txt
	int post = msg.rfind(" ") + 1;
	cmdTask->oldfilename = msg.substr(post, msg.size() - post - 2);
	string res;
	res = "250 Rename oldname " + oldfilename + "\r\n";
	ResponseCmd(res);
}