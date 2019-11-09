#include <thread>
#include "TaskQueue.h"
#include "TasksExecutor.h"

TasksExecutor::TasksExecutor(TaskQueue queue)
{
	_queue = queue;
}

//the handler of this queue, which extracts tasks from it 
//and gives to the given number of threads (извл.отд)
void threadFunction(TaskQueue queue, int count)
{
	std::vector<std::thread> arr;
	while (count) {
		TTask task = queue.popTask();
		if (task != NULL) {
			std::thread thr(task);
			arr.push_back(move(thr));
			count--;
		}
	}

	for (int i = 0; i < arr.size(); i++) {
		arr[i].join();
	}
}

void TasksExecutor::startExecution(int maxThreadsCount) {
	std::thread thr(threadFunction, _queue, maxThreadsCount);
	thr.join();
}




