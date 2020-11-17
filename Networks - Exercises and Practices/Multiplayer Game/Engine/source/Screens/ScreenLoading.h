#ifndef _SCREEN_LOADING_H_
#define _SCREEN_LOADING_H_

class ScreenLoading : public Screen
{
private:

	virtual void Enable() override;
	virtual void Update() override;
	virtual void Disable() override;

private:

	static const int BAR_COUNT = 12;
	GameObject *m_LoadingBars[BAR_COUNT];
};


#endif //_SCREEN_LOADING_H_