

// --- Class Public Methods ---
void ModuleGamesManager::StartGame(GAME_TYPE gameType, uint first_user)
{
	if (gameType != GAME_TYPE::NONE && first_user != 0) // TODO: Would be great to check if user exists!
	{
		m_CurrentUserID = first_user;
		m_CurrentGame = gameType;
		m_GameStatus = GAME_STATUS::START;
	}
	// TODO: Would be also great to communicate we couldn't start a game!
}

void ModuleGamesManager::StopGame()
{
	m_GameStatus = GAME_STATUS::STOP;
}


// --- Class Private Methods ---
void ModuleGamesManager::SendServerNotification(const std::string& msg) const
{
	// std::string message = msg + whatever + "xDD";
	// Server::SendMessageFromGame(message);
}

uint ModuleGamesManager::GetNextUserInList() const
{
	// Server::GetNextUserInList(m_CurrentUserID);
	return 0;
}

void ModuleGamesManager::ProcessAction(const std::string& action) const
{
	if (m_CurrentGame == GAME_TYPE::NONE)
		return;

	std::string command = action.substr(0, action.find_first_of(" "));
	if (command == "/next")
	{

	}
	else
	{
		switch (m_CurrentGame)
		{
		case GAME_TYPE::RUSSIAN_ROULETTE:

			if (command == "/bullet_num")
			{

			}
			else if (command == "/shoot")
			{

			}

			break;

		case GAME_TYPE::SEXKILLMARRY:

			if (command == "/sex")
			{

			}
			else if (command == "/kill")
			{

			}
			else if (command == "/marry")
			{

			}

			break;

		case GAME_TYPE::UNSCRAMBLE:

			if (command == "/unscramble")
			{

			}

			break;
		}
	}
}


// --- Module Methods ---
bool ModuleGamesManager::Update()
{
	switch (m_GameStatus)
	{
		case GAME_STATUS::START:
		{
			SendServerNotification(GetInitialMessage()); // Bug: This pass from start to update will make the games to log initmsg to log updatemsg and make the transition jumping a step
			m_GameStatus = GAME_STATUS::RUNNING;
			break;
		}

		case GAME_STATUS::RUNNING:
		{
			SendServerNotification(GetRunningMessage());

			// Now ProcessAction() is in charge of getting the action of the user and responding after validation... Then, he also should change user and set back to UPDATE
			m_GameStatus = GAME_STATUS::WAITING;
			break;
		}

		case GAME_STATUS::STOP:
		{			
			SendServerNotification(GetStopMessage());

			m_CurrentUserID = 0;
			m_CurrentGame = GAME_TYPE::NONE;
			m_GameStatus = GAME_STATUS::NONE;
			break;
		}
	}

	return true;
}


// --- Game Run Methods ---
const std::string& ModuleGamesManager::GetInitialMessage() const
{
	switch (m_CurrentGame)
	{
		case GAME_TYPE::RUSSIAN_ROULETTE:
		{
			std::string line1 = "In the Russian Roulette all comarades in the chat will have to shoot and test its luck for the motherland!";
			std::string line2 = line1 + "\nThe first user chooses in which gunslot to put the bullet, from slot 1 to 6, decide it by typing '/russianR number_of_bullet_slot'";
			std::string line3 = line2 + "\nThe other users will have to wait its turn and type '/shoot' when they are ready to fire. The last one standing wins!";
			
			return (line1 + line2 + line3 + "\n-- GOOD LUCK! --");
		}
		case GAME_TYPE::SEXKILLMARRY:
		{
			std::string line1 = "In SEX/KILL/MARRY a user will have to choose, among 3 proposed users, which one it would kill, with which one it would have sex and to which one it would marry";
			return (line1 + "\n-- Do so by typing '/skm 'user_sex' 'user_kill' 'user_marry' --" + std::string("\n Are you ready? User ") + std::to_string(m_CurrentUserID) + " Begins!");
		}
		case GAME_TYPE::UNSCRAMBLE:
		{
			std::string line1 = "In Unscramble a user will write a word and the others will have to write a word with the same letters";
			return (line1 + "\n-- Do so by typing '/unscramble word' --" + std::string("\n Are you ready? User ") + std::to_string(m_CurrentUserID) + " Begins!");
		}
	}

	return "";
}

const std::string& ModuleGamesManager::GetRunningMessage() const
{
	switch (m_CurrentGame)
	{
		case GAME_TYPE::RUSSIAN_ROULETTE:
			return ("User " + std::to_string(m_CurrentUserID) + " your turn! Shoot when you are ready, comarade!"); // TODO: + username

		case GAME_TYPE::SEXKILLMARRY:
			return ("SPICY! User " + std::to_string(m_CurrentUserID) + " your turn to kill, fuck and get married! What happens in this server stays in this server ;)"); // TODO: + username

		case GAME_TYPE::UNSCRAMBLE:
			return ("User " + std::to_string(m_CurrentUserID) + " your turn! Write a word with the same letters than "); //TODO: + Previous Word + username
	}

	return "";
}

const std::string& ModuleGamesManager::GetStopMessage() const
{
	switch (m_CurrentGame)
	{
		case GAME_TYPE::RUSSIAN_ROULETTE:
			return ("Oh! User " + std::to_string(m_CurrentUserID) + " has stopped the game! :(\nThe comarades standing are: ! Bye!"); // TODO: + username

		case GAME_TYPE::SEXKILLMARRY:
			return ("Oh! User " + std::to_string(m_CurrentUserID) + " has stopped the game! :O\nNo more fun :( ... Bye!"); // TODO: + username

		case GAME_TYPE::UNSCRAMBLE:
			return ("Oh! User " + std::to_string(m_CurrentUserID) + " has stopped the game! :(\nThe winner is: with X points Congratulations! Bye!"); // TODO: + Winner + Points + username
	}

	return "";
}