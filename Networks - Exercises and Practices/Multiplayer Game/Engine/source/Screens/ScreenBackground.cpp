#include "Core/Core.h"


void ScreenBackground::Enable()
{
	m_Background = Instantiate();
	m_Background->size = { 1920.f, 1080.f };
	m_Background->sprite = App->modRender->AddSprite(m_Background);
	m_Background->sprite->texture = App->modResources->background;
	m_Background->sprite->order = -2;
}

void ScreenBackground::Update()
{
	m_Background->size = { (float)Window.width, (float)Window.height };
}

void ScreenBackground::Disable()
{
	Destroy(m_Background);
}
