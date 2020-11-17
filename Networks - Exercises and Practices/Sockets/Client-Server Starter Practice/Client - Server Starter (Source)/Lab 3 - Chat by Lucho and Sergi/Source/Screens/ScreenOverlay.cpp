#include "Core.h"


void ScreenOverlay::Enable()
{
	ASSERT((oldScene != nullptr && newScene != nullptr), "Scenes to Overlay don't Exist!");
	oldScene->shouldUpdate = false;
	newScene->shouldUpdate = false;

	m_Overlay = new GameObject();
	//m_Overlay->color[0] = 0.0f;
	//m_Overlay->color[1] = 0.0f;
	//m_Overlay->color[2] = 0.0f;
	//m_Overlay->color[3] = 1.0f;
	m_Overlay->texture = App->modResources->background;
	m_Overlay->order = 9999;
	//m_Overlay->scene = this;

	m_TransitionTimeElapsed = 0.0f;
}

void ScreenOverlay::Update()
{
	ASSERT((oldScene != nullptr && newScene != nullptr), "Scenes to Overlay don't Exist!");

	m_Overlay->width = (float)Window.width;
	m_Overlay->height = (float)Window.height;
	m_TransitionTimeElapsed += Time.deltaTime;

	const float halfTransitionTime = m_TransitionMaxTime * 0.5f;

	if (m_TransitionTimeElapsed < halfTransitionTime)
		m_Overlay->color[3] = m_TransitionTimeElapsed / halfTransitionTime;
	else
	{
		oldScene->enabled = false;
		newScene->enabled = true;
		m_Overlay->color[3] = 1.0f - (m_TransitionTimeElapsed - halfTransitionTime) / halfTransitionTime;

		if (m_Overlay->color[3] < 0.0f)
			m_Overlay->color[3] = 0.0f;
	}

	if (m_TransitionTimeElapsed > m_TransitionMaxTime)
	{
		enabled = false;
		oldScene->shouldUpdate = true;
		newScene->shouldUpdate = true;
		oldScene = nullptr;
		newScene = nullptr;
	}
}