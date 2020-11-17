#ifndef _SCREEN_BACKGROUND_H_
#define _SCREEN_BACKGROUND_H_

class ScreenBackground : public Screen
{
private:

	virtual void Enable() override;
	virtual void Update() override;
	virtual void Disable() override;

	GameObject *m_Background = nullptr;
};


#endif //_SCREEN_BACKGROUND_H_