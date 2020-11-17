#include "Core.h"


void ScreenLoading::Enable()
{
	for (int i = 0; i < BAR_COUNT; ++i)
	{
		float progressRatio = (float)i / (float)BAR_COUNT;
		float radians = 2.0f * PI * progressRatio;

		m_LoadingBars[i] = new GameObject;
		m_LoadingBars[i]->color[0] = 1.0f;
		m_LoadingBars[i]->color[1] = 1.0f;
		m_LoadingBars[i]->color[2] = 1.0f;
		m_LoadingBars[i]->color[3] = 1.0f;
		m_LoadingBars[i]->pivot_x = 0.5f;
		m_LoadingBars[i]->pivot_y = 0.5f;
		m_LoadingBars[i]->width = 4;
		m_LoadingBars[i]->height = 30;
		m_LoadingBars[i]->x = 30.0f * sinf(radians);
		m_LoadingBars[i]->y = 30.0f * cosf(radians);
		m_LoadingBars[i]->angle = - 360.0f * progressRatio;
	}
}

void ScreenLoading::Update()
{
	const float ROUND_TIME = 3.0f;

	for (int i = 0; i < BAR_COUNT; ++i)
	{
		float progressRatio = (float)i / (float)BAR_COUNT;
		auto gameObject = m_LoadingBars[i];
		gameObject->color[3] = 1.0f - GetFractionalPart(((float)Time.time + progressRatio * ROUND_TIME)/ ROUND_TIME);
	}

	if (App->modResources->finishedLoading)
		App->modScreen->SwapScreensWithTransition(this, App->modScreen->screenMainMenu); // NOTE(jesus): This is equivalent to this line (without transition): this->enabled = false; App->modScene->scenePingPong->enabled = true;
}

void ScreenLoading::Disable()
{
	for (int i = 0; i < BAR_COUNT; ++i)
		m_LoadingBars[i]->deleteFlag = true;
}