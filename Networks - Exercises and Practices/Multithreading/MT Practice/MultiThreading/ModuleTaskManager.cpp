#include "ModuleTaskManager.h"

// -------------------------------------------------------------------------------------------------
// ----------------------------- EXERCICE BY LUCHO SUAYA & SERGI PARRA -----------------------------
// -------------------------------------------------------------------------------------------------

typedef unsigned int uint;

void ModuleTaskManager::threadMain()
{
	// Here, we enter into an infinite loop that will sequentially perform the tasks of task 3
	// We lock the shared access to scheduledTasks until the function finishes
	Task* task = nullptr;
	std::unique_lock<std::mutex> lock(mtx);
	while (true)
	{
		if (exitFlag)
			break;
		
		// TODO 3: Body for each worker thread
		// Wait for new tasks to arrive
		if (scheduledTasks.size() > 0)
		{
			// Retrieve a scheduled task
			task = scheduledTasks.front();
			scheduledTasks.pop();

			// Unlock while execution (so we don't freeze other threats), execute and then lock back
			lock.unlock();
			task->execute();
			lock.lock();
			
			// Push it into finishedTasks
			finishedTasks.push(task);
		}
		else
			event.wait(lock);
	}
}

bool ModuleTaskManager::init()
{
	// TODO 1: Create threads (they have to execute threadMain())
	for (uint i = 0; i < MAX_THREADS; ++i)
		threads[i] = std::thread(&ModuleTaskManager::threadMain, this);

	return true;
}

bool ModuleTaskManager::update()
{
	// TODO 4: Dispatch all finished tasks to their owner module (use Module::onTaskFinished() callback)
	// -- --
	Task* task = nullptr;
	std::unique_lock<std::mutex> lock(mtx);

	while (!finishedTasks.empty())
	{
		task = finishedTasks.front();
		finishedTasks.pop();

		// Callback
		lock.unlock();
		task->owner->onTaskFinished(task); // Lock/Unlock to while callback so we don't keep threads frozen
		lock.lock();
	}

	// -- --
	return true;
}

bool ModuleTaskManager::cleanUp()
{
	// TODO 5: Notify (wake up) all threads to finish and join them
	// -- --
	std::unique_lock<std::mutex> lock(mtx);
	exitFlag = true;
	lock.unlock();

	event.notify_all();
	for (uint i = 0; i < MAX_THREADS; ++i)
		threads[i].join();

	// -- --
	return true;
}

void ModuleTaskManager::scheduleTask(Task *task, Module *owner)
{
	task->owner = owner;

	// TODO 2: Insert the task into scheduledTasks so it is executed by some thread
	// -- --
	std::unique_lock<std::mutex> lock(mtx);	// Lock the shared access to scheduledTasks
	scheduledTasks.push(task);				// Push task into queue
	lock.unlock();							// Unlock shared access to scheduledTasks
	
	event.notify_one();						// We notify ONLY ONE thread to wake & process a task (not all of them!)
	// -- --
}
