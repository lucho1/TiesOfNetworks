#pragma once

class ScreenOverlay : public Screen
{
public:

	Screen *oldScene = nullptr;
	Screen *newScene = nullptr;


private:

	// Virtual functions of Screen
	virtual void Enable() override;
	virtual void Update() override;
	virtual void Disable() override { m_Overlay->deleteFlag = true; }


private:
	
	GameObject* m_Overlay = nullptr;

	float m_TransitionTimeElapsed = 0.0f;
	float m_TransitionMaxTime = SCENE_TRANSITION_TIME;
};