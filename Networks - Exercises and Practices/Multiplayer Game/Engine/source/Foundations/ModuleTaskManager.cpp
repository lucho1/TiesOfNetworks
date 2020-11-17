#include "Core/Core.h"


inline void Enqueue(Task **queue, int *front, int *back, int max_elems, Task * task)
{
	ASSERT(*back != *front);

	queue[*back] = task;
	if (*front == -1)
		*front = *back;
	
	*back = (*back + 1) % max_elems;
}

inline Task* Dequeue(Task **queue, int *front, int *back, int max_elems)
{
	ASSERT(*front != -1);

	Task *task = queue[*front];
	queue[*front] = nullptr;
	*front = (*front + 1) % max_elems;
	
	if (*front == *back)
	{
		*front = -1;
		*back = 0;
	}
	
	return task;
}


static const int MAX_THREADS = 4;
static std::thread threads[MAX_THREADS];

static std::mutex scheduledMutex;
static std::mutex finishedMutex;
static std::condition_variable event;

void ModuleTaskManager::ThreadMain()
{
	Task *task = nullptr;
	
	while (true)
	{
		{
			std::unique_lock<std::mutex> lock(scheduledMutex);
			while (m_ScheduledTasksFront == -1 && !m_ExitFlag)
				event.wait(lock);

			if (m_ExitFlag)
				break;
			else
				task = Dequeue(m_ScheduledTasks, &m_ScheduledTasksFront, &m_ScheduledTasksBack, MAX_TASKS);
		}

		task->Execute();

		{
			std::unique_lock<std::mutex> lock(finishedMutex);
			Enqueue(m_FinishedTasks, &m_FinishedTasksFront, &m_FinishedTasksBack, MAX_TASKS, task);
		}
	}
}

bool ModuleTaskManager::Init()
{
	for (auto &thread : threads)
		thread = std::thread(&ModuleTaskManager::ThreadMain, this);

	return true;
}

bool ModuleTaskManager::Update()
{
	if (m_FinishedTasksFront != -1)
	{
		std::unique_lock<std::mutex> lock(finishedMutex);

		while (m_FinishedTasksFront != -1)
		{
			Task *task = Dequeue(m_FinishedTasks, &m_FinishedTasksFront, &m_FinishedTasksBack, MAX_TASKS);
			task->owner->OnTaskFinished(task);
		}
	}

	return true;
}

bool ModuleTaskManager::CleanUp()
{
	{
		std::unique_lock<std::mutex> lock(scheduledMutex);
		m_ExitFlag = true;
		event.notify_all();
	}

	for (auto &thread : threads)
		thread.join();

	return true;
}

void ModuleTaskManager::ScheduleTask(Task *task, Module *owner)
{
	task->owner = owner;
	std::unique_lock<std::mutex> lock(scheduledMutex);

	Enqueue(m_ScheduledTasks, &m_ScheduledTasksFront, &m_ScheduledTasksBack, MAX_TASKS, task);
	event.notify_one();
}
