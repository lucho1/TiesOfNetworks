#ifndef _MODULEGAMESMANAGER_H_
#define _MODULEGAMESMANAGER_H_

class ModuleGamesManager : public Module
{
	enum class GAME_TYPE { NONE = 0, RUSSIAN_ROULETTE, UNSCRAMBLE, SEXKILLMARRY };
	enum class GAME_STATUS { NONE = 0, START, RUNNING, WAITING };
	enum class GAME_COMMANDS { INVALID_COMMAND = 0, NEXT, BULLET_NUM, SHOOT, SEX, KILL, MARRY, UNSCRAMBLE_WORD };

public:

	ModuleGamesManager();

	// Class Public Methods
	void StartGame(GAME_TYPE gameType, uint first_user);
	void StopGame();

	// Getters
	bool IsGameRunning()				const { return (m_GameStatus == GAME_STATUS::NONE); }
	uint GetCurrentUserPlaying()		const { return m_CurrentUser.second; }
	GAME_STATUS GetGameStatus()			const { return m_GameStatus; }
	GAME_TYPE GetCurrentGameRunning()	const { return m_CurrentGame; }

private:

	// Module Methods
	virtual bool Update() override;
	virtual bool GUI() override; // For debugging in meantime

	// Class Private Methods
	void SendServerNotification(const std::string& msg);
	uint GetNextUserInList();
	void ProcessAction(const std::string& action);

	// Game Run Methods
	const std::string GetInitialMessage() const;
	const std::string GetRunningMessage() const;
	const std::string GetStopMessage() const;

	//const std::string& SetupNotification();


private:

	GAME_TYPE m_CurrentGame = GAME_TYPE::NONE;
	GAME_STATUS m_GameStatus = GAME_STATUS::NONE;
	std::pair<std::string, uint> m_CurrentUser = { "NULL", -1 };
	std::unordered_map<std::string, GAME_COMMANDS> m_GameCommands;

	// Temporal - for Debugging purposes
	std::vector<std::string> m_GameMessages;
};

#endif