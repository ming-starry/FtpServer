#pragma once
#include "XFtpTask.h"
class XFtpPORT :
	public XFtpTask
{
public:
	virtual void Parse(std::string type, std::string msg);
};

