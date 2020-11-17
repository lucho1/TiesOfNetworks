#include "Core.h"

void ScreenBackground::Enable()
{
	background = new GameObject;
	background->texture = App->modResources->background;
	background->width = 1920;
	background->height = 1080;
	background->order = -1;
	//background->scene = this;
}

void ScreenBackground::Update()
{
	background->width = (float)Window.width;
	background->height = (float)Window.height;
}

void ScreenBackground::Disable()
{
	background->deleteFlag = true;
}