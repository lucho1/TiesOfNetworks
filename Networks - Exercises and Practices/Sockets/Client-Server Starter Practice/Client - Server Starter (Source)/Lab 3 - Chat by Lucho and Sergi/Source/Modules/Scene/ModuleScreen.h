#pragma once

class ScreenOverlay;
class ScreenLoading;
class ScreenBackground;
class ScreenMainMenu;
class ScreenGame;

class ModuleScreen : public Module
{
public:

	// Virtual functions of Module
	virtual bool Init() override;
	virtual bool Update() override;
	virtual bool GUI() override;
	virtual bool CleanUp() override;

	// Module Screen Methods
	void SwapScreensWithTransition(Screen* oldScene, Screen* newScene);

public:

	// Screens
	ScreenOverlay*		screenOverlay = nullptr;
	ScreenLoading*		screenLoading = nullptr;
	ScreenBackground*	screenBackground = nullptr;
	ScreenMainMenu*		screenMainMenu = nullptr;
	ScreenGame*			screenGame = nullptr;

	Screen *m_Screens[MAX_SCREENS];
	int m_ScreenCount = 0;
};