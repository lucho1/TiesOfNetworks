#pragma once

class ScreenLoading : public Screen
{
	// Virtual functions of Screen
	void enable() override;
	void update() override;
	void disable() override;

	// Variables
	static const int BAR_COUNT = 12;
	GameObject *loadingBars[BAR_COUNT];
};