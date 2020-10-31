#pragma once

class ModuleUI : public Module
{
public:

	// Virtual functions of Module
	virtual bool Init() override;
	virtual bool PreUpdate() override;
	virtual bool GUI() override;
	virtual bool PostUpdate() override;
	virtual bool CleanUp() override;

	// Public methods
	LRESULT HandleWindowsEvents(UINT msg, WPARAM wParam, LPARAM lParam);
};