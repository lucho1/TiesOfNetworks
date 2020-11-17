#pragma once

class ScreenBackground : public Screen
{
	// Virtual functions of Module
	virtual void Enable() override;
	virtual void Update() override;
	virtual void Disable() override;

	// Variables
	GameObject* background = nullptr;
};