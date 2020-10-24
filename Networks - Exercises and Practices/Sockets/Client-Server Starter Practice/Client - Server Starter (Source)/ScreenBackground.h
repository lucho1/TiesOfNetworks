#pragma once

class ScreenBackground : public Screen
{
	// Virtual functions of Module
	void enable() override;
	void update() override;
	void disable() override;

	// Variables
	GameObject *background = nullptr;
};