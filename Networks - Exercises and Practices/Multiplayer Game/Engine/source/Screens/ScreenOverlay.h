#ifndef _SCREEN_OVERLAY_H_
#define _SCREEN_OVERLAY_H_

class ScreenOverlay : public Screen
{
public:

	Screen *oldScene = nullptr;
	Screen *newScene = nullptr;

private:

	virtual void Enable() override;
	virtual void Update() override;
	virtual void Disable() override;

private:

	GameObject* m_Overlay = nullptr;
	float m_TransitionTimeElapsed = 0.0f;
	float m_TransitionTimeMax = SCENE_TRANSITION_TIME_SECONDS;
};


#endif //_SCREEN_OVERLAY_H_