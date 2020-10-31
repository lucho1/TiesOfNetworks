#include "Core.h"


#if defined(USE_TASK_MANAGER)
class TaskLoadTexture : public Task
{
public:

	const char *filename = nullptr;
	Texture *texture = nullptr;
	virtual void Execute() override { texture = App->modTextures->LoadTexture(filename); }
};
#endif


bool ModuleResources::Init()
{
	background = App->modTextures->LoadTexture("Assets/Textures/background.jpg");

#if !defined(USE_TASK_MANAGER)
	banner = App->modTextures->loadTexture("Assets/Textures/banner.jpg");
	client = App->modTextures->loadTexture("Assets/Textures/client.jpg");
	server = App->modTextures->loadTexture("Assets/Textures/server.png");
	loadingFinished = true;
	completionRatio = 1.0f;
#else
	LoadTextureAsync("Assets/Textures/banner.jpg", &banner);
	LoadTextureAsync("Assets/Textures/client.jpg", &client);
	LoadTextureAsync("Assets/Textures/server.jpg", &server);
#endif

	return true;
}

#if defined(USE_TASK_MANAGER)

void ModuleResources::LoadTextureAsync(const char * filename, Texture **texturePtrAddress)
{
	TaskLoadTexture *task = new TaskLoadTexture;
	task->owner = this;
	task->filename = filename;
	App->modTaskManager->ScheduleTask(task, this);

	m_TaskResults[m_TaskCount].texturePtr = texturePtrAddress;
	m_TaskResults[m_TaskCount].task = task;
	m_TaskCount++;
}

void ModuleResources::onTaskFinished(Task * task)
{
	ASSERT((task != nullptr), "Task passed was NULL");
	TaskLoadTexture *taskLoadTexture = dynamic_cast<TaskLoadTexture*>(task);

	for (int i = 0; i < m_TaskCount; ++i)
	{
		if (task == m_TaskResults[i].task)
		{
			*(m_TaskResults[i].texturePtr) = taskLoadTexture->texture;
			m_FinishedTaskCount++;
			delete task;
			task = nullptr;
			break;
		}
	}

	ASSERT((task == nullptr), "Task is not NULL, still Exists");
	if (m_FinishedTaskCount == m_TaskCount)
		finishedLoading = true;
}

#endif