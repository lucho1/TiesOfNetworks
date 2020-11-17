#ifndef _MODULE_RESOURCES_H_
#define _MODULE_RESOURCES_H_

#define USE_TASK_MANAGER

struct Texture;

class ModuleResources : public Module
{
public:

	Texture *background = nullptr;
	Texture *space = nullptr;
	Texture *asteroid1 = nullptr;
	Texture *asteroid2 = nullptr;
	Texture *spacecraft1 = nullptr;
	Texture *spacecraft2 = nullptr;
	Texture *spacecraft3 = nullptr;
	Texture *laser = nullptr;
	Texture *explosion1 = nullptr;

	AnimationClip *explosionClip = nullptr;

	AudioClip *audioClipLaser = nullptr;
	AudioClip *audioClipExplosion = nullptr;

	bool finishedLoading = false;

private:

	virtual bool Init() override;

#if defined(USE_TASK_MANAGER)
	
	class TaskLoadTexture : public Task
	{
	public:

		const char *filename = nullptr;
		Texture **texture = nullptr;

		virtual void Execute() override;
	};

	static const int MAX_RESOURCES = 16;
	TaskLoadTexture m_Tasks[MAX_RESOURCES] = {};
	uint32 m_TaskCount = 0;
	uint32 m_FinishedTaskCount = 0;

private:

	virtual void OnTaskFinished(Task *task) override;
	void LoadTextureAsync(const char *filename, Texture **texturePtrAddress);

#endif
};

#endif //_MODULE_RESOURCES_H_