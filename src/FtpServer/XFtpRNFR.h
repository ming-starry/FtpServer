#pragma once
#include "XFtpTask.h"
class XFtpRNFR :
	public XFtpTask
{
public:
	virtual void Parse(std::string type, std::string msg);
};

