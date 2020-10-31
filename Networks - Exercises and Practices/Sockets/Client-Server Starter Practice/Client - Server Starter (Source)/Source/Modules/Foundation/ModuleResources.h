#pragma once

#define USE_TASK_MANAGER

struct Texture;

class ModuleResources : public Module
{
public:

	Texture* background = nullptr;
	Texture* banner = nullptr;
	Texture* client = nullptr;
	Texture* server = nullptr;

	bool finishedLoading = false;


private:

	// Virtual functions of Module
	virtual bool Init() override;


private:

#if defined(USE_TASK_MANAGER)
	virtual void onTaskFinished(Task *task) override;
	void LoadTextureAsync(const char *filename, Texture **texturePtrAddress);
#endif


	struct LoadTextureResult
	{
		Texture **texturePtr = nullptr;
		Task *task = nullptr;
	};

	LoadTextureResult m_TaskResults[1024] = {};
	int m_TaskCount = 0;
	int m_FinishedTaskCount = 0;
};