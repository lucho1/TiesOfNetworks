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

	inline void PrintMessageInConsole(const char* msg, const Color& col = Color(0.2f, 1.0f, 0.2f, 1.0f)) const; // Text Color Green by default

	// Public methods
	LRESULT HandleWindowsEvents(UINT msg, WPARAM wParam, LPARAM lParam);
};