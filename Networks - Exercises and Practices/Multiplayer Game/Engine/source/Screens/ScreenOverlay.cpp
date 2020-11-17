#include "Core/Core.h"


void ScreenOverlay::Enable()
{
	ASSERT(oldScene != nullptr && newScene != nullptr);
	oldScene->shouldUpdate = false;
	newScene->shouldUpdate = false;

	m_Overlay = Instantiate();
	//overlay->color[0] = 0.0f;
	//overlay->color[1] = 0.0f;
	//overlay->color[2] = 0.0f;
	//overlay->color[3] = 1.0f;
	m_Overlay->sprite = App->modRender->AddSprite(m_Overlay);
	m_Overlay->sprite->texture = App->modResources->background;
	m_Overlay->sprite->order = 9999;
	//overlay->scene = this;

	m_TransitionTimeElapsed = 0.0f;
	App->modUI->SetInputsEnabled(false);
}

void ScreenOverlay::Update()
{
	ASSERT(oldScene != nullptr && newScene != nullptr);
	m_Overlay->size = { (float)Window.width, (float)Window.height };
	const float halfTransitionTime = m_TransitionTimeMax * 0.5f;

	if (m_TransitionTimeElapsed < halfTransitionTime)
		m_Overlay->sprite->color.a = m_TransitionTimeElapsed / halfTransitionTime;
	else
	{
		oldScene->enabled = false;
		newScene->enabled = true;
		m_Overlay->sprite->color.a = 1.0f - (m_TransitionTimeElapsed - halfTransitionTime) / halfTransitionTime;

		if (m_Overlay->sprite->color.a < 0.0f)
			m_Overlay->sprite->color.a = 0.0f;
	}

	if (m_TransitionTimeElapsed > m_TransitionTimeMax)
	{
		enabled = false;
		oldScene->shouldUpdate = true;
		newScene->shouldUpdate = true;
		oldScene = nullptr;
		newScene = nullptr;
	}

	m_TransitionTimeElapsed += Time.deltaTime;
}

void ScreenOverlay::Disable()
{
	App->modUI->SetInputsEnabled(true);
	Destroy(m_Overlay);
}
