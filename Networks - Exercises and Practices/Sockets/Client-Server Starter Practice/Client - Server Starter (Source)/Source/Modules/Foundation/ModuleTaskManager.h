#pragma once

class Task
{
public:
	virtual void Execute() = 0;
	Module *owner = nullptr;
};

class ModuleTaskManager : public Module
{
public:

	// Virtual functions of Module
	virtual bool Init() override;
	virtual bool Update() override;
	virtual bool CleanUp() override;

	// To schedule new tasks
	void ScheduleTask(Task *task, Module *owner);
	void ThreadMain();


private:

	Task* m_ScheduledTasks[MAX_TASKS];
	int m_ScheduledTasksFront = -1;
	int m_ScheduledTasksBack = 0;

	Task* m_FinishedTasks[MAX_TASKS];
	int m_FinishedTasksFront = -1;
	int m_FinishedTasksBack = 0;

	bool exitFlag = false;
};