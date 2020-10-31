#pragma once

class ScreenLoading : public Screen
{
	// Virtual functions of Screen
	virtual void Enable() override;
	virtual void Update() override;
	virtual void Disable() override;

	// Variables
	static const int BAR_COUNT = 12;
	GameObject* m_LoadingBars[BAR_COUNT];
};