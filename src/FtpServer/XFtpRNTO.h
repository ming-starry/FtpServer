#pragma once
#include "XFtpTask.h"
class XFtpRNTO :
	public XFtpTask
{
public:
	virtual void Parse(std::string type, std::string msg);
};

