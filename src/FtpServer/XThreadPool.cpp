#include "XThreadPool.h"
#include "XThread.h"
#include <thread>
#include <iostream>
using namespace std;

void XThreadPool::Init(int threadCount)
{
	this->threadCount = threadCount;
	this->lastThread = -1;
	for (int i = 0; i < threadCount; i++)
	{
		XThread *t = new XThread();
		t->id = i + 1;
		cout << "Creat thread: " << i+1 << endl;
		t->Start();
		//分发和init都在同一个线程，不需要考虑互斥
		threads.push_back(t);
		this_thread::sleep_for(10ms);
	}
}


//分发线程
void XThreadPool::Dispatch(XTask * task)
{
	if (task == NULL) return;
	int tid = (lastThread + 1) % threadCount;
	lastThread = tid;


	XThread *t = threads[tid];
	//添加 task类型为 XFtpServerCmd *
	t->AddTask(task);  //线程互斥
	//激活线程
	t->Activate();


}