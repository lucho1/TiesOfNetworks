#pragma once

class Screen
{
public:

	// Enable / disable screens
	bool enabled = false;
	bool shouldUpdate = true;


private:

	// Virtual functions of Module
	virtual void Enable() {}  // Called each time the screen is enabled
	virtual void Update() {}  // Called at each frame (if enabled)
	virtual void GUI() {}     // Called at each frame (if enabled)
	virtual void Disable() {} // Called each time the screen is disabled


private:

	bool m_WasEnabled = false;
	friend class ModuleScreen;
};