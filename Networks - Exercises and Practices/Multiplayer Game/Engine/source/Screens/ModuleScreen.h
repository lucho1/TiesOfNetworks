#ifndef _MODULE_SCREEN_H_
#define _MODULE_SCREEN_H_

class ScreenOverlay;
class ScreenLoading;
class ScreenBackground;
class ScreenMainMenu;
class ScreenGame;

class ModuleScreen : public Module
{
public:

	virtual bool Init() override;
	virtual bool Update() override;
	virtual bool DrawGUI() override;
	virtual bool CleanUp() override;

	void SwapScreensWithTransition(Screen *oldScene, Screen *newScene);

public:

	// Screens
	ScreenOverlay		*screenOverlay = nullptr;
	ScreenLoading		*screenLoading = nullptr;
	ScreenBackground	*screenBackground = nullptr;
	ScreenMainMenu		*screenMainMenu = nullptr;
	ScreenGame			*screenGame = nullptr;

private:

	Screen *m_Screens[MAX_SCREENS];
	int m_ScreenCount = 0;

};

#endif //_MODULE_SCREEN_H_