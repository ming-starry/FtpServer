#pragma once
#include "XFtpTask.h"
class XFtpDELE :
	public XFtpTask
{
public:
	virtual void Parse(std::string type, std::string msg);
};

