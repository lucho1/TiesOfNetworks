
ModuleGamesManager::ModuleGamesManager()
{
	m_GameCommands["next"] = GAME_COMMANDS::NEXT;
	m_GameCommands["n"] = GAME_COMMANDS::NEXT;

	m_GameCommands["rr bullet at"] = GAME_COMMANDS::BULLET_NUM;
	m_GameCommands["rr shoot"] = GAME_COMMANDS::SHOOT;

	m_GameCommands["skm sex"] = GAME_COMMANDS::SEX;
	m_GameCommands["skm s"] = GAME_COMMANDS::SEX;
	m_GameCommands["skm kill"] = GAME_COMMANDS::KILL;
	m_GameCommands["skm k"] = GAME_COMMANDS::KILL;
	m_GameCommands["skm marry"] = GAME_COMMANDS::MARRY;
	m_GameCommands["skm m"] = GAME_COMMANDS::MARRY;

	m_GameCommands["unscramble word"] = GAME_COMMANDS::UNSCRAMBLE_WORD;
}


// --- Class Public Methods ---
void ModuleGamesManager::StartGame(GAME_TYPE gameType, uint first_user)
{
	if (gameType != GAME_TYPE::NONE)
	{
		m_CurrentUser = App->modNetServer->GetNextUser(first_user);
		if (m_CurrentUser.second != -1)
		{
			m_CurrentGame = gameType;
			m_GameStatus = GAME_STATUS::START;
		}
		else
			SendServerNotification("Couldn't begin game, user to begin was invalid!");
	}
	else
		SendServerNotification("Game to begin was invalid!");
}

void ModuleGamesManager::StopGame()
{
	std::string msg = GetStopMessage();
	SendServerNotification(msg);

	m_CurrentUser = { "NULL", -1 };
	m_CurrentGame = GAME_TYPE::NONE;
	m_GameStatus = GAME_STATUS::NONE;
}


// --- Class Private Methods ---
void ModuleGamesManager::SendServerNotification(const std::string& msg)
{
	// std::string message = msg + whatever + "xDD";
	// Server::SendMessageFromGame(message);
	m_GameMessages.push_back(msg);
}

uint ModuleGamesManager::GetNextUserInList()
{
	m_CurrentUser = App->modNetServer->GetNextUser(m_CurrentUser.second);
	if (m_CurrentUser.second == -1)
	{
		StopGame();
		SendServerNotification("Next user was invalid! Stopping Game");
	}

	return 0;
}

void ModuleGamesManager::ProcessAction(const std::string& action)
{
	if (m_CurrentGame == GAME_TYPE::NONE)
		return;

	// Get Command Type
	GAME_COMMANDS gCommand;
	try	{ gCommand = m_GameCommands.at(action); }
	catch (const std::out_of_range & e) { gCommand = GAME_COMMANDS::INVALID_COMMAND; }

	// Find Command Order
	std::size_t start_pos = action.find_first_not_of(' ');
	if (start_pos == std::string::npos)
		APPCONSOLE_WARN_LOG("Invalid Command!");


	bool update_game_status = false;
	switch (gCommand)
	{
		case GAME_COMMANDS::NEXT:
		{
			break;
		}
		case GAME_COMMANDS::BULLET_NUM:
		{
			if (m_CurrentGame != GAME_TYPE::RUSSIAN_ROULETTE)
				APPCONSOLE_WARN_LOG("Invalid Command for this Game!");
			else
			{
				update_game_status = true;
			}

			break;
		}
		case GAME_COMMANDS::SHOOT:
		{
			if (m_CurrentGame != GAME_TYPE::RUSSIAN_ROULETTE)
				APPCONSOLE_WARN_LOG("Invalid Command for this Game!");
			else
			{
				update_game_status = true;
			}

			break;
		}
		case GAME_COMMANDS::SEX:
		{
			if (m_CurrentGame != GAME_TYPE::SEXKILLMARRY)
				APPCONSOLE_WARN_LOG("Invalid Command for this Game!");
			else
			{
				update_game_status = true;
			}

			break;
		}
		case GAME_COMMANDS::KILL:
		{
			if (m_CurrentGame != GAME_TYPE::SEXKILLMARRY)
				APPCONSOLE_WARN_LOG("Invalid Command for this Game!");
			else
			{
				update_game_status = true;
			}

			break;
		}
		case GAME_COMMANDS::MARRY:
		{
			if (m_CurrentGame != GAME_TYPE::SEXKILLMARRY)
				APPCONSOLE_WARN_LOG("Invalid Command for this Game!");
			else
			{
				update_game_status = true;
			}

			break;
		}
		case GAME_COMMANDS::UNSCRAMBLE_WORD:
		{
			if (m_CurrentGame != GAME_TYPE::UNSCRAMBLE)
				APPCONSOLE_WARN_LOG("Invalid Command for this Game!");
			else
			{
				update_game_status = true;
			}

			break;
		}
		case GAME_COMMANDS::INVALID_COMMAND:
		{
			std::string warning = "The game command '" + action + "' does not exist!";
			APPCONSOLE_WARN_LOG(warning.c_str());
			break;
		}
	}

	if (update_game_status)
		m_GameStatus = GAME_STATUS::RUNNING; // TODO: ChooseUser
}


// --- Module Methods ---
bool ModuleGamesManager::GUI()
{
	ImGui::Begin("GAMES DEBUGGER");
	ImGui::SetWindowSize({ 400.0f, 400.0f });

	if (ImGui::Button("Begin Unscramble"))
		StartGame(GAME_TYPE::UNSCRAMBLE, 0);

	if (ImGui::Button("Begin Russian Roulette"))
		StartGame(GAME_TYPE::RUSSIAN_ROULETTE, 0);

	if (ImGui::Button("Begin SexKillMarry"))
		StartGame(GAME_TYPE::SEXKILLMARRY, 0);


	if (ImGui::Button("End Game & Clear Text"))
	{
		StopGame();
		m_GameMessages.clear();
	}

	for (auto& msg_pair : m_GameMessages)
		ImGui::TextWrapped("%s", msg_pair.c_str());


	ImGui::End();
	return true;
}

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
	}

	return true;
}


// --- Game Run Methods ---
const std::string ModuleGamesManager::GetInitialMessage() const
{
	switch (m_CurrentGame)
	{
		case GAME_TYPE::RUSSIAN_ROULETTE:
		{
			std::string line1 = "In the Russian Roulette all comarades in the chat will have to shoot and test its luck for the motherland!";
			std::string line2 = "\n\nThe first user chooses in which gunslot to put the bullet, from slot 1 to 6, decide it by typing '/russianR number_of_bullet_slot'";
			std::string line3 = "\n\nThe other users will have to wait its turn and type '/shoot' when they are ready to fire. The last one standing wins!";
			
			return (line1 + line2 + line3 + "\n\n\n** GOOD LUCK! **\n\n\n");
		}
		case GAME_TYPE::SEXKILLMARRY:
		{
			std::string line1 = "In SEX/KILL/MARRY a user will have to choose, among 3 proposed users, which one it would kill, with which one it would have sex and to which one it would marry";
			return (line1 + "\n\n\n** Do so by typing '/skm 'user_sex' 'user_kill' 'user_marry' **" + std::string("\n\nAre you ready? User ") + m_CurrentUser.first
					+ "(#" + std::to_string(m_CurrentUser.second) + ") Begins!\n\n\n");
		}
		case GAME_TYPE::UNSCRAMBLE:
		{
			std::string line1 = "In Unscramble a user will write a word and the others will have to write a word with the same letters";
			return (line1 + "\n\n\n** Do so by typing '/unscramble word' **" + std::string("\n\nAre you ready? User ") + m_CurrentUser.first + "(#" + std::to_string(m_CurrentUser.second) + ") Begins!\n\n\n");
		}
	}

	return "NULL";
}

const std::string ModuleGamesManager::GetRunningMessage() const
{
	switch (m_CurrentGame)
	{
		case GAME_TYPE::RUSSIAN_ROULETTE:
			return ("User " + m_CurrentUser.first + "(#" + std::to_string(m_CurrentUser.second) + ") your turn! Shoot when you are ready, comarade!\n\n");

		case GAME_TYPE::SEXKILLMARRY:
			return ("SPICY! User " + m_CurrentUser.first + "(#" + std::to_string(m_CurrentUser.second) + ") your turn to kill, fuck and get married! What happens in this server stays in this server ;)\n\n");

		case GAME_TYPE::UNSCRAMBLE:
			return ("User " + m_CurrentUser.first + "(#" + std::to_string(m_CurrentUser.second) + ") your turn! Write a word with the same letters than '' \n\n"); //TODO: + Previous Word
	}

	return "NULL";
}

const std::string ModuleGamesManager::GetStopMessage() const
{
	switch (m_CurrentGame)
	{
		case GAME_TYPE::RUSSIAN_ROULETTE:
			return ("Oh! User " + m_CurrentUser.first + " (#" + std::to_string(m_CurrentUser.second) + ") has stopped the game! :(\n\n\nThe comarades standing are: ! Bye!\n\n\n"); // TODO: winners

		case GAME_TYPE::SEXKILLMARRY:
			return ("Oh! User " + m_CurrentUser.first + " (#" + std::to_string(m_CurrentUser.second) + ") has stopped the game! :O\n\n\nNo more fun :( ... Bye!\n\n\n");

		case GAME_TYPE::UNSCRAMBLE:
			return ("Oh! User " + m_CurrentUser.first + " (#" + std::to_string(m_CurrentUser.second) + ") has stopped the game! :(\n\n\nThe winner is: with X points Congratulations! Bye!\n\n\n"); // TODO: + Winner + Points
	}

	return "NULL";
}