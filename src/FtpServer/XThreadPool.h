#pragma once
#include <vector>
#include "XTask.h"
//.h少引用头文件
class XThread;

class XThreadPool
{
public:

	static XThreadPool * Get()
	{
		static XThreadPool p;
		return &p;
	}
	//~XThreadPool();

	//初始化线程并启动线程
	void Init(int threadCount);


	//分发线程
	void Dispatch(XTask * task);


private:
	//线程数量
	int threadCount = 0;
	int lastThread = -1;
	//线程池线程
	std::vector<XThread *> threads;
	XThreadPool() {};
};

