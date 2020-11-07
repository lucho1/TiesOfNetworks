
ModuleGamesManager::ModuleGamesManager()
{
	m_GameCommands["game next"] = GAME_COMMANDS::NEXT;
	m_GameCommands["game n"] = GAME_COMMANDS::NEXT;

	m_GameCommands["rr bullet"] = GAME_COMMANDS::BULLET_NUM;
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
		m_CurrentUser = App->modNetServer->GetUserFromID(first_user);
		if (m_CurrentUser.second != -1)
		{
			if (((gameType == GAME_TYPE::RUSSIAN_ROULETTE || gameType == GAME_TYPE::UNSCRAMBLE) && App->modNetServer->GetUsersNumber() > 1) || (gameType == GAME_TYPE::SEXKILLMARRY && App->modNetServer->GetUsersNumber() > 3))
			{
				m_CurrentUser = { "NULL", -1 };
				m_CurrentGame = gameType;
				m_GameStatus = GAME_STATUS::START;

				if (gameType == GAME_TYPE::RUSSIAN_ROULETTE)
					for (uint i = -1; i < App->modNetServer->GetUsersNumber() - 1; ++i)
						m_GamesData.alive_players.push_back(App->modNetServer->GetNextUser(i).second);

				if (gameType == GAME_TYPE::UNSCRAMBLE)
					for (uint i = -1; i < App->modNetServer->GetUsersNumber() - 1; ++i)
						m_GamesData.unscramble_ranking.insert({ App->modNetServer->GetNextUser(i).second, 0 });
			}
			else
				SendServerNotification("Couldn't begin game, not enough users connected");
		}
		else
			SendServerNotification("Couldn't begin game, user to begin was invalid!");
	}
	else
		SendServerNotification("Game to begin was invalid!");
}

void ModuleGamesManager::StopGame()
{
	// Reset Games variables
	m_GamesData.bullet_slot_number = 0;
	m_GamesData.alive_players.clear();
	m_GamesData.sex = false;
	m_GamesData.kill = false;
	m_GamesData.marry = false;
	m_GamesData.users_answered = "";
	m_GamesData.original_word = "";
	m_GamesData.ordered_word = "";
	m_GamesData.last_word = "";
	m_GamesData.unscramble_ranking.clear();

	// Send Stop Notification
	SendServerNotification(GetStopMessage());

	// Reset user, game type & status
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

void ModuleGamesManager::GetNextUserInList()
{
	m_CurrentUser = App->modNetServer->GetNextUser(m_CurrentUser.second);
	if (m_CurrentUser.second == -1)
	{
		StopGame();
		SendServerNotification("Next user doesn't exists! Stopping Game");
	}
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
		SendServerNotification("Invalid Command!");

	// Process Command Order
	bool update_game_status = false;
	switch (gCommand)
	{
		case GAME_COMMANDS::NEXT:
		{
			std::string response;
			if (m_CurrentGame != GAME_TYPE::RUSSIAN_ROULETTE)
			{
				response = "That's not very brave from you ex-comarade >:(" + GetUserLabel() + "... Do you know what we do in mother russia with the traitors and cowards...? Yes, we shoot them *shoots* ...";
				m_GamesData.alive_players.erase(std::find(m_GamesData.alive_players.begin(), m_GamesData.alive_players.end(), m_CurrentUser.second));
				
				GetNextUserInList();
				response += "\n\nAnyway, the game still runs **reloads**...\nNext user is " + GetUserLabel();
			}
			else
			{
				response = "Oh! User " + GetUserLabel() + " has passed! :(\n\nNext user is ";
				GetNextUserInList();
				response += GetUserLabel();				
			}

			SendServerNotification(response);
			break;
		}
		case GAME_COMMANDS::BULLET_NUM:
		{
			if (m_CurrentGame != GAME_TYPE::RUSSIAN_ROULETTE)
				SendServerNotification("Invalid Command for this Game!");
			else if(m_GamesData.bullet_slot_number == 0)
				SendServerNotification("You can't change the slot of the bullet, you cheaty comarade! Back in the USSR, that would have been a reason to send you to Siberia >:(");
			else
			{
				size_t number_pos = action.find_first_not_of(' ', start_pos);
				std::string number_str = action.substr(number_pos, action.find_first_of(' ', number_pos) - 1);

				m_GamesData.bullet_slot_number = std::stoi(number_str);
				std::string response = "\n\nReceived, Comarade! Bullet will be in the slot " + number_str
										+ ". Anyone shooting will hit a random slot. Luckily for you, it won't be the chosen one. You are the first, comarade " + GetUserLabel();
				
				SendServerNotification(response);
			}

			break;
		}
		case GAME_COMMANDS::SHOOT:
		{
			if (m_CurrentGame != GAME_TYPE::RUSSIAN_ROULETTE)
				SendServerNotification("Invalid Command for this Game!");
			else
			{
				if (m_GamesData.bullet_slot_number == 0)
					SendServerNotification("You must chose a bullet slot first! True comarades play with fire >:(");
				else
				{
					std::string last_user = m_CurrentUser.first;
					std::string response;
					uint slot_hit = rng::GetRandomInt_InRange(1, 6);

					if (slot_hit == m_GamesData.bullet_slot_number)
					{
						m_GamesData.alive_players.erase(std::find(m_GamesData.alive_players.begin(), m_GamesData.alive_players.end(), m_CurrentUser.second));

						GetNextUserInList();						
						while(std::find(m_GamesData.alive_players.begin(), m_GamesData.alive_players.end(), m_CurrentUser.second) != m_GamesData.alive_players.end())
							GetNextUserInList();

						response = "Oh, how sad! " + last_user + " lies in the ground with a bullet in the head, the paths of the old Lenin are mysterious. Well... game goes on **reloads**\nNext user is " + GetUserLabel();
					}
					else
					{
						GetNextUserInList();
						response = "Well played comarade " + last_user + " you don't have your brain in the ground!\nYou hitted slot " + std::to_string(slot_hit);
						+ "\n\n\nWell... game goes on **reloads**\nNext user is " + GetUserLabel();
						
					}

					SendServerNotification(response);
					update_game_status = true;
				}
			}

			break;
		}
		case GAME_COMMANDS::SEX:
		{
			if (m_CurrentGame != GAME_TYPE::SEXKILLMARRY)
				SendServerNotification("Invalid Command for this Game!");
			else
			{
				m_GamesData.sex = true;
				update_game_status = true;

				size_t user_pos = action.find_first_not_of(' ', start_pos);
				std::string user_str = action.substr(user_pos, action.find_first_of(' ', user_pos) - 1);
				m_GamesData.users_answered += " have Sex with: " + user_str;

				// Make response
				std::string response = "Spicy answer " + GetUserLabel() + "!";
				if(!m_GamesData.kill && !m_GamesData.marry)
					response += "\nYou still have to answer with whom you would marry and who you would kill, little assassin";
				else if (!m_GamesData.kill)
					response += "\nYou still have to answer who you would kill, little assassin";
				else if (!m_GamesData.marry)
					response += "\nYou still have to answer with whom you would marry, we don't want you to live alone :(";

				SendServerNotification(response);
			}

			break;
		}
		case GAME_COMMANDS::KILL:
		{
			if (m_CurrentGame != GAME_TYPE::SEXKILLMARRY)
				SendServerNotification("Invalid Command for this Game!");
			else
			{
				m_GamesData.kill = true;
				update_game_status = true;

				size_t user_pos = action.find_first_not_of(' ', start_pos);
				std::string user_str = action.substr(user_pos, action.find_first_of(' ', user_pos) - 1);
				m_GamesData.users_answered += " Kill: " + user_str;

				// Make response
				std::string response = "Revealing answer " + GetUserLabel() + "!";
				if (!m_GamesData.sex && !m_GamesData.marry)
					response += "\nYou still have to answer with whom you would marry and with whom you would have sex, don't be embarrassed!";
				else if (!m_GamesData.sex)
					response += "\nYou still have to answer with whom you would have sex, don't be embarrassed!";
				else if (!m_GamesData.marry)
					response += "\nYou still have to answer with whom you would marry, we don't want you to live alone :(";

				SendServerNotification(response);
			}

			break;
		}
		case GAME_COMMANDS::MARRY:
		{
			if (m_CurrentGame != GAME_TYPE::SEXKILLMARRY)
				SendServerNotification("Invalid Command for this Game!");
			else
			{
				m_GamesData.marry = true;
				update_game_status = true;
				
				size_t user_pos = action.find_first_not_of(' ', start_pos);
				std::string user_str = action.substr(user_pos, action.find_first_of(' ', user_pos) - 1);
				m_GamesData.users_answered += " Marry with: " + user_str;
				
				// Make response
				std::string response = "Beautiful answer " + GetUserLabel() + "!";
				if (!m_GamesData.sex && !m_GamesData.kill)
					response += "\nYou still have to answer whom you would kill and with whom you would have sex, don't be embarrassed, little assassin!";
				else if (!m_GamesData.sex)
					response += "\nYou still have to answer with whom you would have sex, don't be embarrassed!";
				else if (!m_GamesData.kill)
					response += "\nYou still have to answer whom you would kill, little assassin!";

				SendServerNotification(response);
			}

			break;
		}
		case GAME_COMMANDS::UNSCRAMBLE_WORD:
		{
			if (m_CurrentGame != GAME_TYPE::UNSCRAMBLE)
				SendServerNotification("Invalid Command for this Game!");
			else
			{
				std::string response;
				update_game_status = true;

				size_t word_pos = action.find_first_not_of(' ', start_pos);
				std::string word_str = action.substr(word_pos, action.find_first_of(' ', word_pos) - 1);

				if (m_GamesData.original_word == "")
				{
					response = "Nice word " + GetUserLabel() + "!";
					m_GamesData.original_word = m_GamesData.last_word = word_str;
					ArrangeWord();
				}
				else
				{					
					if (CompareWords(word_str))
						response = "Good answer " + GetUserLabel() + "!\n\n\n";
					else
					{
						response = "Incorrect answer " + GetUserLabel() + "! :(\n\nTry Again!\n\n\n";						
						update_game_status = false;
					}
				}

				SendServerNotification(response);
			}

			break;
		}
		case GAME_COMMANDS::INVALID_COMMAND:
		{
			std::string warning = "The game command '" + action + "' does not exist!";
			SendServerNotification(warning.c_str());
			break;
		}
	}

	if (update_game_status)
		m_GameStatus = GAME_STATUS::RUNNING;
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
			SendServerNotification(GetInitialMessage()); // (SHOULD BE SOLVED) Bug: This pass from start to update will make the games to log initmsg, to log updatemsg and make the transition jumping a step
			break;
		}

		case GAME_STATUS::RUNNING:
		{
			if (m_CurrentGame == GAME_TYPE::RUSSIAN_ROULETTE && m_GamesData.alive_players.size() == 1)
			{
				std::string win_statement = "Congratulations Comarade " + GetUserLabel() + "! You have won the game and a promotion comarade-administrator!";
				SendServerNotification(win_statement);
				StopGame();
				break;
			}

			if (m_CurrentGame == GAME_TYPE::SEXKILLMARRY && m_GamesData.sex && m_GamesData.kill && m_GamesData.marry)
			{
				std::string next_statement = "Illuminating answer from user " + GetUserLabel() + ", which would" + m_GamesData.users_answered;
				SendServerNotification(next_statement);
				
				m_GamesData.kill = false;
				m_GamesData.marry = false;
				m_GamesData.sex = false;
				m_GamesData.users_answered = "";

				GetNextUserInList();
			}
			
			if (m_CurrentGame == GAME_TYPE::UNSCRAMBLE)
				GetNextUserInList();

			SendServerNotification(GetRunningMessage());
			m_GameStatus = GAME_STATUS::WAITING;

			break;
		}
	}

	return true;
}


// --- Game Run Methods ---
const std::string ModuleGamesManager::GetInitialMessage()
{
	switch (m_CurrentGame)
	{
		case GAME_TYPE::RUSSIAN_ROULETTE:
		{
			m_GameStatus = GAME_STATUS::WAITING;

			std::string line1 = "\n\n\nIn the Russian Roulette all comarades in the chat will have to shoot themselves to test their luck for the motherland!";
			std::string line2 = "\n\nThe first user chooses in which gunslot to put the bullet, from slot 1 to 6, decide it by typing '/rr bullet [number from 1-6]'";
			std::string line3 = "\n\nThe other users will have to wait its turn and type '/rr shoot' when they are ready to fire. The last one standing wins!";
			
			return (line1 + line2 + line3 + "\n\n\n** GOOD LUCK! **\n\n" + "Game begins when user " + GetUserLabel() + " decides the bullet slot\n\n");
		}
		case GAME_TYPE::SEXKILLMARRY:
		{
			m_GameStatus = GAME_STATUS::WAITING;

			std::string line1 = "\n\n\nIn SEX/KILL/MARRY a user will have to choose, among 3 proposed users, which one it would kill, with which one it would have sex and to which one it would marry";
			return (line1 + "\n\n\n** Do so by typing '/skm ' followed by the action and the user (as: '/skm s [user]' or '/skm kill [user]') **" + std::string("\n\nAre you ready? User ") + GetUserLabel() + " begins!\n\n\n");
		}
		case GAME_TYPE::UNSCRAMBLE:
		{
			m_GameStatus = GAME_STATUS::WAITING;

			std::string line1 = "\n\n\nIn Unscramble a user will write a word and the other users will have to write a word with the same letters";
			return (line1 + "\n\n\n** Do so by typing '/unscramble word [word]' **" + std::string("\n\nAre you ready? User ") + GetUserLabel() + " begins! Choose a word!\n\n\n");
		}
	}

	return "NULL";
}

const std::string ModuleGamesManager::GetRunningMessage()
{
	switch (m_CurrentGame)
	{
		case GAME_TYPE::RUSSIAN_ROULETTE:
			return ("\n\n\nUser " + GetUserLabel() + " your turn! Shoot when you are ready, comarade! Remember Order 227: Not a step back!\n\n");

		case GAME_TYPE::SEXKILLMARRY:
			return ("\n\n\nSPICY! User " + GetUserLabel() + " your turn to kill, fuck and get married! What happens in this server stays in this server ;)\n\n");

		case GAME_TYPE::UNSCRAMBLE:
		{
			std::string msg = "\n\n\nUser " + GetUserLabel() + " your turn! Write a word with the same letters than '" + m_GamesData.original_word + "'\n\n";
			m_GamesData.original_word = m_GamesData.original_word;
			return (msg);
		}
	}

	return "NULL";
}

const std::string ModuleGamesManager::GetStopMessage() const
{
	switch (m_CurrentGame)
	{
		case GAME_TYPE::RUSSIAN_ROULETTE:
		{
			std::string winners_str = "";
			for (uint user : m_GamesData.alive_players)
				winners_str += App->modNetServer->GetUserFromID(user).first + " ";

			return ("\n\n\nOh! User " + GetUserLabel() + " has stopped the game! :(\n\n\nThe comarades standing are: " + winners_str + "! Bye!\n\n\n");
		}

		case GAME_TYPE::SEXKILLMARRY:
			return ("\n\n\nOh! User " + GetUserLabel() + " has stopped the game! :O\n\n\nNo more fun :( ... Bye!\n\n\n");

		case GAME_TYPE::UNSCRAMBLE:
		{
			uint winnerID = 0, winnerPoints = 0;
			for (auto ranker : m_GamesData.unscramble_ranking)
				if (ranker.second > winnerPoints)
					winnerID = ranker.first;

			std::string winners = "";
			for(auto ranker : m_GamesData.unscramble_ranking)
				if (ranker.second == winnerPoints)
					winners += App->modNetServer->GetUserFromID(ranker.first).first + " ";

			std::string ret = "\n\n\nOh! User " + GetUserLabel() + " has stopped the game! :(\n\n\nThe winner is:"
								+ App->modNetServer->GetUserFromID(winnerID).first +" with " + std::to_string(winnerID) + " points."
								+ " users " + winners + "have the same punctuation.\n\nCongratulations! Bye!\n\n\n";

			return (ret);
		}
	}

	return "NULL";
}

const std::string ModuleGamesManager::GetUserLabel() const
{
	return m_CurrentUser.first + " (#" + std::to_string(m_CurrentUser.second) + ")";
}

bool ModuleGamesManager::CompareWords(const std::string& compared_word)
{
	// Compare Words
	uint words_found = 0;
	std::string ordered_word_backup = m_GamesData.ordered_word;

	for (uint i = 0; i < compared_word.size() && m_GamesData.ordered_word.size() > 0; ++i)
	{
		std::string::iterator it = std::find(m_GamesData.ordered_word.begin(), m_GamesData.ordered_word.end(), compared_word[i]);
		if (it != m_GamesData.ordered_word.end())
		{
			++words_found;
			m_GamesData.ordered_word.erase(it);
		}
	}

	// Return comparison result
	if (words_found >= ordered_word_backup.size() && m_GamesData.ordered_word.empty())
	{
		m_GamesData.unscramble_ranking[m_CurrentUser.second]++;
		m_GamesData.last_word = m_GamesData.original_word;
		m_GamesData.original_word = compared_word; // Word Passes

		ArrangeWord();
		return true;
	}
	else
		m_GamesData.ordered_word = ordered_word_backup; // Word doesn't passes

	return false;
}

void ModuleGamesManager::ArrangeWord()
{
	// Order Word
	for (uint i = 0; i < m_GamesData.original_word.size(); ++i)
	{
		if (std::find(m_GamesData.ordered_word.begin(), m_GamesData.ordered_word.end(), m_GamesData.original_word[i]) == m_GamesData.ordered_word.end())
			m_GamesData.ordered_word.push_back(m_GamesData.original_word[i]);
	}
}