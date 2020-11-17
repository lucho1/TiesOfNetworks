#ifndef _SCREEN_GAME_H_
#define _SCREEN_GAME_H_

class ScreenGame : public Screen
{
public:

	bool isServer = true;
	int serverPort;
	const char* serverAddress = "127.0.0.1";
	const char* playerName = "player";
	uint8 spaceshipType = 0;

private:

	virtual void Enable() override;
	virtual void Update() override;
	virtual void DrawGUI() override;
	virtual void Disable() override;
};


#endif //_SCREEN_GAME_H_