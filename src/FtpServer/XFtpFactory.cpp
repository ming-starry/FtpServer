#include "XFtpFactory.h"
#include "XFtpServerCmd.h"
#include "XFtpUSER.h"
#include "XFtpLIST.h"
#include "XFtpPORT.h"
#include "XFtpRETR.h"
#include "XFtpSTOR.h"
#include "XFtpDELE.h"
#include "XFtpRNTO.h"
#include "XFtpRNFR.h"
XTask * XFtpFactory::CreatTask()
{
	XFtpServerCmd * x = new XFtpServerCmd();
	x->Reg("USER", new XFtpUSER);
	

	XFtpLIST *list = new XFtpLIST();
	x->Reg("PWD", list);
	x->Reg("LIST", list);
	x->Reg("CWD", list);
	x->Reg("CDUP", list);

	x->Reg("PORT", new XFtpPORT());

	x->Reg("RETR", new XFtpRETR());
	x->Reg("STOR", new XFtpSTOR());
	x->Reg("DELE", new XFtpDELE());
	x->Reg("RNTO", new XFtpRNTO());
	x->Reg("RNFR", new XFtpRNFR());

	//注册ftp消息处理对象
	// x 为FtpServerCmd *
	return x;
}

XFtpFactory::XFtpFactory()
{

}
XFtpFactory::~XFtpFactory()
{

}