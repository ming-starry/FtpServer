#include "XFtpPORT.h"
#include <iostream>
#include <vector>
using namespace std;
void XFtpPORT::Parse(std::string type, std::string msg)
{
	//PORT PORT 127,0,0,1,27,208\r\n
	//PORT n1,n2,n3,n4,n5,n6\r\n
	//port n5 *256 + n6;

	//只获取ip和端口，不连接
	vector<string> vals;
	string tmp = "";
	for (int i = 5; i < msg.size(); i++)
	{
		if (msg[i] == ',' || msg[i] == '\r')
		{
			vals.push_back(tmp);
			tmp = "";
			continue;
		}
		tmp += msg[i];
	}
	if (vals.size() != 6)
	{
		ResponseCmd("501 Synatx error in parameters or arguments. \r\n");
	}
	ip = vals[0] + "." + vals[1] + "." + vals[2] + "." + vals[3];
	port = atoi(vals[4].c_str()) * 256 + atoi(vals[5].c_str());
	cout << "PORT ip is " << ip << endl;
	cout << "PORT port is " << port << endl;
	ResponseCmd("200 PORT command successful.\r\n");

}