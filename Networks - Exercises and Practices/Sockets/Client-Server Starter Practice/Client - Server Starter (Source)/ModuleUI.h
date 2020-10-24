#pragma once

class ModuleUI : public Module
{
public:

	// Virtual functions of Module
	bool init() override;
	bool preUpdate() override;
	bool gui() override;
	bool postUpdate() override;
	bool cleanUp() override;

	// Public methods
	LRESULT HandleWindowsEvents(UINT msg, WPARAM wParam, LPARAM lParam);
};