#include "Core/Core.h"


void ScreenLoading::Enable()
{
	for (int i = 0; i < BAR_COUNT; ++i)
	{
		float progressRatio = (float)i / (float)BAR_COUNT;
		float radians = 2.0f * PI * progressRatio;

		m_LoadingBars[i] = Instantiate();
		m_LoadingBars[i]->position = 30.0f * vec2{ sinf(radians), cosf(radians) };
		m_LoadingBars[i]->angle = -360.0f * progressRatio;
		m_LoadingBars[i]->size = vec2{ 4, 30 };
		m_LoadingBars[i]->sprite = App->modRender->AddSprite(m_LoadingBars[i]);
		m_LoadingBars[i]->sprite->color = vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
		m_LoadingBars[i]->sprite->pivot = vec2{ 0.5f, 0.5f };
	}
}

void ScreenLoading::Update()
{
	const float ROUND_TIME = 3.0f;
	for (int i = 0; i < BAR_COUNT; ++i)
	{
		float progressRatio = (float)i / (float)BAR_COUNT;
		GameObject *gameObject = m_LoadingBars[i];
		gameObject->sprite->color.a = 1.0f - FractionalPart(((float)Time.time + progressRatio * ROUND_TIME)/ ROUND_TIME);
	}

	if (App->modResources->finishedLoading)
	{
		App->modScreen->SwapScreensWithTransition(this, App->modScreen->screenMainMenu);

		// NOTE(jesus): The following is equivalent to the previous line but without transition.
		//this->enabled = false;
		//App->modScene->scenePingPong->enabled = true;
	}
}

void ScreenLoading::Disable()
{
	for (int i = 0; i < BAR_COUNT; ++i)
		Destroy(m_LoadingBars[i]);
}
