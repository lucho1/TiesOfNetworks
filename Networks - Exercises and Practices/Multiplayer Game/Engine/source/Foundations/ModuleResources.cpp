#include "Core/Core.h"


#if defined(USE_TASK_MANAGER)

void ModuleResources::TaskLoadTexture::Execute()
{
	*texture = App->modTextures->LoadTexture(filename);
}

#endif


bool ModuleResources::Init()
{
	background = App->modTextures->LoadTexture("background.jpg");

#if !defined(USE_TASK_MANAGER)
	space = App->modTextures->loadTexture("space_background.jpg");
	asteroid1 = App->modTextures->loadTexture("asteroid1.png");
	asteroid2 = App->modTextures->loadTexture("asteroid2.png");
	spacecraft1 = App->modTextures->loadTexture("spacecraft1.png");
	spacecraft2 = App->modTextures->loadTexture("spacecraft2.png");
	spacecraft3 = App->modTextures->loadTexture("spacecraft3.png");
	loadingFinished = true;
	completionRatio = 1.0f;
#else
	LoadTextureAsync("space_background.jpg", &space);
	LoadTextureAsync("asteroid1.png",        &asteroid1);
	LoadTextureAsync("asteroid2.png",        &asteroid2);
	LoadTextureAsync("spacecraft1.png",      &spacecraft1);
	LoadTextureAsync("spacecraft2.png",      &spacecraft2);
	LoadTextureAsync("spacecraft3.png",      &spacecraft3);
	LoadTextureAsync("laser.png",            &laser);
	LoadTextureAsync("explosion1.png",       &explosion1);
#endif

	audioClipLaser = App->modSound->LoadAudioClip("laser.wav");
	audioClipExplosion = App->modSound->LoadAudioClip("explosion.wav");
	//App->modSound->playAudioClip(audioClipExplosion);

	return true;
}

#if defined(USE_TASK_MANAGER)

void ModuleResources::LoadTextureAsync(const char * filename, Texture **texturePtrAddress)
{
	ASSERT(m_TaskCount < MAX_RESOURCES);
	
	TaskLoadTexture *task = &m_Tasks[m_TaskCount++];
	task->owner = this;
	task->filename = filename;
	task->texture = texturePtrAddress;

	App->modTaskManager->ScheduleTask(task, this);
}

void ModuleResources::OnTaskFinished(Task * task)
{
	ASSERT(task != nullptr, "Task was NULL");

	TaskLoadTexture *taskLoadTexture = dynamic_cast<TaskLoadTexture*>(task);
	for (uint32 i = 0; i < m_TaskCount; ++i)
	{
		if (task == &m_Tasks[i])
		{
			m_FinishedTaskCount++;
			task = nullptr;
			break;
		}
	}

	ASSERT(task == nullptr, "Task was NULL");

	if (m_FinishedTaskCount == m_TaskCount)
	{
		finishedLoading = true;

		// Create the explosion animation clip
		explosionClip = App->modRender->AddAnimationClip();
		explosionClip->frameTime = 0.1f;
		explosionClip->loop = false;

		for (int i = 0; i < 16; ++i)
		{
			float x = (i % 4) / 4.0f;
			float y = (i / 4) / 4.0f;
			float w = 1.0f / 4.0f;
			float h = 1.0f / 4.0f;
			explosionClip->addFrameRect(vec4{ x, y, w, h });
		}
	}
}

#endif
