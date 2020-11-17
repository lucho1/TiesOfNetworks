#ifndef _SCREEN_H_
#define _SCREEN_H_

class Screen
{
	friend class ModuleScreen;
public:

	// Enable / disable screens
	bool enabled = false;
	bool shouldUpdate = true;


private:

	// Virtual methods
	virtual void Enable()	{}	// Called each time the screen is enabled
	virtual void Update()	{}	// Called at each frame (if enabled)
	virtual void DrawGUI()	{}	// Called at each frame (if enabled)
	virtual void Disable()	{}	// Called each time the screen is disabled

private:

	bool m_WasEnabled = false;
};


#endif //_SCREEN_H_