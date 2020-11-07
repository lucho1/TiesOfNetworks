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
	void GetNextUserInList();
	void ProcessAction(const std::string& action);

	// Game Run Methods
	const std::string GetInitialMessage();
	const std::string GetRunningMessage();
	const std::string GetStopMessage() const;
	const std::string GetUserLabel() const;
	bool CompareWords(const std::string& compared_word);
	void ArrangeWord();


private:

	GAME_TYPE m_CurrentGame = GAME_TYPE::NONE;
	GAME_STATUS m_GameStatus = GAME_STATUS::NONE;

	std::unordered_map<std::string, GAME_COMMANDS> m_GameCommands;
	std::pair<std::string, uint> m_CurrentUser = { "NULL", -1 };

	// Temporal - for Debugging purposes
	std::vector<std::string> m_GameMessages;

	struct GameData
	{
		// Russian Roulette variables
		uint bullet_slot_number = 0;
		std::vector<uint> alive_players;

		//SexKillMarry variables
		bool sex = false, kill = false, marry = false;
		std::string users_answered = "";

		// Unscramble variables
		std::string original_word = "";
		std::string ordered_word = "";
		std::string last_word = "";
		std::unordered_map<uint, uint> unscramble_ranking;
	};

	GameData m_GamesData;
};

#endif