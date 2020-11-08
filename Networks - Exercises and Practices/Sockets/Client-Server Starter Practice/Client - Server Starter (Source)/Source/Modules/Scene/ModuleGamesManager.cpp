#include "ModuleGamesManager.h"

ModuleGamesManager::ModuleGamesManager()
{
	m_GameCommands["next"] = GAME_COMMANDS::NEXT;
	m_GameCommands["n"] = GAME_COMMANDS::NEXT;
	m_GameCommands["start"] = GAME_COMMANDS::START;
	m_GameCommands["help"] = GAME_COMMANDS::HELP;
	m_GameCommands["h"] = GAME_COMMANDS::HELP;
	m_GameCommands["stop"] = GAME_COMMANDS::STOP;

	// Russian Roulette
	m_GameCommands["bullet"] = GAME_COMMANDS::BULLET_NUM;
	m_GameCommands["shoot"] = GAME_COMMANDS::SHOOT;

	// Sex Kill Marry
	m_GameCommands["sex"] = GAME_COMMANDS::SEX;
	m_GameCommands["s"] = GAME_COMMANDS::SEX;
	m_GameCommands["kill"] = GAME_COMMANDS::KILL;
	m_GameCommands["k"] = GAME_COMMANDS::KILL;
	m_GameCommands["marry"] = GAME_COMMANDS::MARRY;
	m_GameCommands["m"] = GAME_COMMANDS::MARRY;

	// Unscramble and Chain
	m_GameCommands["word"] = GAME_COMMANDS::WORD;
}


// --- Class Public Methods ---
void ModuleGamesManager::StartGame(GAME_TYPE gameType, uint first_user)
{
	if (gameType != GAME_TYPE::NONE)
	{
		m_CurrentUser = App->modNetServer->GetUserFromID(first_user);
		if (m_CurrentUser.second != -1)
		{
			if (((gameType == GAME_TYPE::RUSSIAN_ROULETTE || gameType == GAME_TYPE::UNSCRAMBLE || gameType == GAME_TYPE::CHAINED_WORDS) && App->modNetServer->GetUsersNumber() > 1)
				|| (gameType == GAME_TYPE::SEXKILLMARRY && App->modNetServer->GetUsersNumber() > 3))
			{
				m_CurrentUser = { "NULL", -1 };
				m_CurrentGame = gameType;
				m_GameStatus = GAME_STATUS::START;

				if (gameType == GAME_TYPE::RUSSIAN_ROULETTE)
					for (uint i = -1; i < App->modNetServer->GetUsersNumber() - 1; ++i)
						m_GamesData.alive_players.push_back(App->modNetServer->GetNextUser(i).second);

				if (gameType == GAME_TYPE::UNSCRAMBLE || gameType == GAME_TYPE::CHAINED_WORDS)
					for (uint i = -1; i < App->modNetServer->GetUsersNumber() - 1; ++i)
						m_GamesData.unscramble_ranking.insert({ App->modNetServer->GetNextUser(i).second, 0 });
			}
			else
				App->modNetServer->SendServerNotification("Couldn't begin game, not enough users connected", EntryType::APP_WARN_LOG, first_user);
		}
		else
			App->modNetServer->SendServerNotification("Couldn't begin game, user to begin was invalid!", EntryType::APP_WARN_LOG, first_user);
	}
	else
		App->modNetServer->SendServerNotification("Game to begin was invalid!", EntryType::APP_WARN_LOG, first_user);
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
	m_GamesData.unscramble_ranking.clear();

	// Send Stop Notification
	App->modNetServer->SendServerNotification(GetStopMessage(), APP_INFO_LOG);

	// Reset user, game type & status
	m_CurrentUser = { "NULL", -1 };
	m_CurrentGame = GAME_TYPE::NONE;
	m_GameStatus = GAME_STATUS::NONE;
}

void ModuleGamesManager::GetNextUserInList()
{
	m_CurrentUser = App->modNetServer->GetNextUser(m_CurrentUser.second);
	if (m_CurrentUser.second == -1)
	{
		StopGame();
		App->modNetServer->SendServerNotification("Next user doesn't exists! Stopping Game", EntryType::APP_WARN_LOG);
	}
}

void ModuleGamesManager::ProcessAction(GAME_TYPE game_action, uint user_id, const std::string& action)
{
	if (m_CurrentGame != GAME_TYPE::NONE && m_CurrentGame != game_action) {
		App->modNetServer->SendServerNotification("We are not playing that game!", EntryType::APP_WARN_LOG, user_id);
		return;
	}

	//Find command
	std::string command, args;
	std::size_t pos = action.find_first_of(' ');
	command = action.substr(0, pos);
	if (pos != std::string::npos)
		args = action.substr(pos);
	else
		args = "";

	// Get Command Type
	GAME_COMMANDS gCommand;
	try	{ gCommand = m_GameCommands.at(command); }
	catch (const std::out_of_range & e) { gCommand = GAME_COMMANDS::INVALID_COMMAND; }

	// Process Command Order
	switch (m_CurrentGame)
	{
	case GAME_TYPE::NONE:
		if (gCommand == GAME_COMMANDS::START)
			StartGame(game_action, user_id);
		else {
			App->modNetServer->SendServerNotification("We are not playing yet, might want to try starting first!", EntryType::APP_WARN_LOG, user_id);
		}
		break;

	case GAME_TYPE::RUSSIAN_ROULETTE:
		ProcessRussianRoulette(gCommand, args, user_id);
		break;
	
	case GAME_TYPE::SEXKILLMARRY:
		ProcessSexKillMarry(gCommand, args, user_id);
		break;

	case GAME_TYPE::UNSCRAMBLE:
		ProcessUnscramble(gCommand, args, user_id);
		break;

	case GAME_TYPE::CHAINED_WORDS:
		ProcessChainedWords(gCommand, args, user_id);
		break;
	}
}


// --- Module Methods ---
bool ModuleGamesManager::Update()
{
	switch (m_GameStatus)
	{
		case GAME_STATUS::START:
		{
			App->modNetServer->SendServerNotification(GetInitialMessage(), EntryType::APP_INFO_LOG); // (SHOULD BE SOLVED) Bug: This pass from start to update will make the games to log initmsg, to log updatemsg and make the transition jumping a step
			break;
		}

		case GAME_STATUS::RUNNING:
		{
			if (m_CurrentGame == GAME_TYPE::RUSSIAN_ROULETTE && m_GamesData.alive_players.size() == 1)
			{
				std::string win_statement = "Congratulations Comarade " + GetUserLabel() + "! You have won the game and a promotion comrade-administrator!";
				App->modNetServer->SendServerNotification(win_statement, EntryType::APP_INFO_LOG);
				StopGame();
				break;
			}

			if (m_CurrentGame == GAME_TYPE::SEXKILLMARRY && m_GamesData.sex && m_GamesData.kill && m_GamesData.marry)
			{
				std::string next_statement = "Illuminating answer from user " + GetUserLabel() + ", which would" + m_GamesData.users_answered;
				App->modNetServer->SendServerNotification(next_statement, EntryType::APP_INFO_LOG);
				
				m_GamesData.kill = false;
				m_GamesData.marry = false;
				m_GamesData.sex = false;
				m_GamesData.users_answered = "";

				GetNextUserInList();
			}
			
			if (m_CurrentGame == GAME_TYPE::UNSCRAMBLE || m_CurrentGame == GAME_TYPE::CHAINED_WORDS)
				GetNextUserInList();

			App->modNetServer->SendServerNotification(GetRunningMessage(), EntryType::APP_INFO_LOG);
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

			std::string line1 = "In the Russian Roulette all comrades in the chat will have to shoot themselves to test their luck for the motherland!";
			std::string line2 = "\nThe first user chooses in which gunslot to put the bullet, from slot 1 to 6, decide it by typing '/rr bullet [number from 1-6]'";
			std::string line3 = "\nThe other users will have to wait its turn and type '/rr shoot' when they are ready to fire. The last one standing wins!";
			
			return (line1 + line2 + line3 + "\n** GOOD LUCK! **" + "\nGame begins when user " + GetUserLabel() + " decides the bullet slot");
		}
		case GAME_TYPE::SEXKILLMARRY:
		{
			m_GameStatus = GAME_STATUS::WAITING;
			GenerateKSMNames();

			std::string line1 = "In KILL/SEX/MARRY a user will have to choose, among 3 proposed users, which one it would kill, with which one it would have sex and to which one it would marry";
			std::string line2 = "\n** Do so by typing '/ksm ' followed by the action and the user (as: '/ksm sex [user]' or '/ksm kill [user]') **" + std::string("\nAre you ready? User ") + GetUserLabel() + " begins!";
			std::string line3 = "\nYour names are: \"" + m_GamesData.ksm_names[0] + "\", \"" + m_GamesData.ksm_names[1] + "\", \"" + m_GamesData.ksm_names[2] + "\"";
			return (line1 + line2 + line3);
		}
		case GAME_TYPE::UNSCRAMBLE:
		{
			m_GameStatus = GAME_STATUS::WAITING;

			std::string line1 = "In Unscramble, a user will write a word and the other users will have to write a word with the same letters";
			return (line1 + "\n** Do so by typing '/unscramble word [word]' **" + std::string("\nAre you ready? User ") + GetUserLabel() + " begins! Choose a word!");
		}
		case GAME_TYPE::CHAINED_WORDS:
		{
			m_GameStatus = GAME_STATUS::WAITING;

			std::string line1 = "In Chained Words, a user will write a word and the other users will have to write a word with the same  first letter";
			return (line1 + "\n** Do so by typing '/chained word [word]' **" + std::string("\nAre you ready? User ") + GetUserLabel() + " begins! Choose a word!");
		}
	}

	return "NULL";
}

const std::string ModuleGamesManager::GetRunningMessage()
{
	switch (m_CurrentGame)
	{
		case GAME_TYPE::RUSSIAN_ROULETTE:
			return ("User " + GetUserLabel() + " your turn! Shoot when you are ready, comrade! Remember Order 227: Not a step back!");
		case GAME_TYPE::SEXKILLMARRY: 
		{
			GenerateKSMNames();

			std::string line1 = "SPICY! User " + GetUserLabel() + " your turn to kill, fuck and get married! What happens in this server stays in this server ;)";
			std::string line2 = "\nYour names are: \"" + m_GamesData.ksm_names[0] + "\", \"" + m_GamesData.ksm_names[1] + "\", \"" + m_GamesData.ksm_names[2] + "\"";

			return (line1 + line2);
		}
		case GAME_TYPE::UNSCRAMBLE:
			return("User " + GetUserLabel() + " your turn! Write a word with the same letters than '" + m_GamesData.original_word + "'");
		case GAME_TYPE::CHAINED_WORDS:
			return ("User " + GetUserLabel() + " your turn! Write a word with the same 1st letter than '" + m_GamesData.original_word + "'");
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

			return ("\n\n\nOh! User " + GetUserLabel() + " has stopped the game! :(\n\n\nThe comrades standing are: " + winners_str + "! Bye!\n\n\n");
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
		case GAME_TYPE::CHAINED_WORDS:
		{
			uint winnerID = 0, winnerPoints = 0;
			for (auto ranker : m_GamesData.unscramble_ranking)
				if (ranker.second > winnerPoints)
					winnerID = ranker.first;

			std::string winners = "";
			for (auto ranker : m_GamesData.unscramble_ranking)
				if (ranker.second == winnerPoints)
					winners += App->modNetServer->GetUserFromID(ranker.first).first + " ";

			std::string ret = "\n\n\nOh! User " + GetUserLabel() + " has stopped the game! :(\n\n\nThe winner is:"
				+ App->modNetServer->GetUserFromID(winnerID).first + " with " + std::to_string(winnerID) + " points."
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

inline void ModuleGamesManager::GenerateKSMNames() {
	uint nusers = App->modNetServer->GetUsersNumber();
	const std::unordered_map<std::string, uint>& users = App->modNetServer->GetUserNicknames();

	uint name1, name2, name3;
	std::string& ksm_name1 = m_GamesData.ksm_names[0];
	std::string& ksm_name2 = m_GamesData.ksm_names[2];
	std::string& ksm_name3 = m_GamesData.ksm_names[3];

	do {
		name1 = rng::GetRandomInt_InRange(0, nusers - 1);

		auto name = users.begin();
		for (int i = 0; i < name1; ++i)
			name++;

		ksm_name1 = name->first;
	} while (ksm_name1 == m_CurrentUser.first);

	do {
		name2 = rng::GetRandomInt_InRange(0, nusers - 1);

		auto name = users.begin();
		for (int i = 0; i < name2; ++i)
			name++;

		ksm_name2 = name->first;
	} while (ksm_name2 == m_CurrentUser.first || name2 == name1);

	do {
		name3 = rng::GetRandomInt_InRange(0, nusers - 1);

		auto name = users.begin();
		for (int i = 0; i < name3; ++i)
			name++;

		ksm_name3 = name->first;
	} while (ksm_name3 == m_CurrentUser.first || name3 == name1 || name3 == name2);
}

void ModuleGamesManager::ProcessRussianRoulette(GAME_COMMANDS command, const std::string& args, uint user_id) {
	if (command != GAME_COMMANDS::STOP && user_id != GetCurrentUserPlaying()) {
		App->modNetServer->SendServerNotification("Hey! Not your turn yet comrade!", EntryType::APP_WARN_LOG, user_id);
		return;
	}
	switch (command) {
	default:
		App->modNetServer->SendServerNotification("Invalid command! Try /rr help", EntryType::APP_WARN_LOG, user_id);
		break;
	case GAME_COMMANDS::NEXT:
	{
		std::string response = "That's not very brave from you ex-comrade >:( " + GetUserLabel() + "... Do you know what we do in mother russia with the traitors and cowards...? Yes, we shoot them *shoots* ...";
		m_GamesData.alive_players.erase(std::find(m_GamesData.alive_players.begin(), m_GamesData.alive_players.end(), m_CurrentUser.second));

		GetNextUserInList();
		response += "\n\nAnyway, the game still runs **reloads**...\nNext user is " + GetUserLabel();
		App->modNetServer->SendServerNotification(response, EntryType::APP_WARN_LOG);
		break;
	} //NEXT
	case GAME_COMMANDS::SHOOT:
	{
		if (m_GamesData.bullet_slot_number == 0)
			App->modNetServer->SendServerNotification("You must chose a bullet slot first! True comrades play with fire >:(", EntryType::APP_WARN_LOG, user_id);
		else {
			std::string last_user = m_CurrentUser.first;
			std::string response;
			uint slot_hit = rng::GetRandomInt_InRange(1, 6);

			if (slot_hit == m_GamesData.bullet_slot_number) {
				m_GamesData.alive_players.erase(std::find(m_GamesData.alive_players.begin(), m_GamesData.alive_players.end(), m_CurrentUser.second));

				GetNextUserInList();
				while (std::find(m_GamesData.alive_players.begin(), m_GamesData.alive_players.end(), m_CurrentUser.second) != m_GamesData.alive_players.end())
					GetNextUserInList();

				response = "Oh, how sad! " + last_user + " lies in the ground with a bullet in the head, the paths of the old Lenin are mysterious. Well... game goes on **reloads**\nNext user is " + GetUserLabel();
			}
			else {
				GetNextUserInList();
				response = "Well played comrade " + last_user + " you don't have your brain in the ground!\nYou hitted slot " + std::to_string(slot_hit);
				+"\n\n\nWell... game goes on **reloads**\nNext user is " + GetUserLabel();

			}

			App->modNetServer->SendServerNotification(response, EntryType::APP_INFO_LOG);
			m_GameStatus = GAME_STATUS::RUNNING;
		}
		break;
	} //SHOOT
	case GAME_COMMANDS::BULLET_NUM:
	{
		if (m_GamesData.bullet_slot_number == 0)
			App->modNetServer->SendServerNotification("You can't change the slot of the bullet, you cheaty comrade! Back in the USSR, that would have been a reason to send you to Siberia >:(", EntryType::APP_WARN_LOG, user_id);
		else {

			size_t pos = args.find_first_of(' ');
			std::string number_str = args.substr(0, pos);

			m_GamesData.bullet_slot_number = std::stoi(number_str);
			std::string response = "Received, Comrade! Bullet will be in the slot " + number_str
				+ ". Anyone shooting will hit a random slot. If you are lucky, it won't be the chosen one. You are the first, comrade " + GetUserLabel();

			App->modNetServer->SendServerNotification(response, EntryType::APP_INFO_LOG, user_id);

			std::string announcement = "Comrade " + GetUserLabel() + " has choosen a slot, let the fun begin!";
			App->modNetServer->SendServerNotification(announcement, EntryType::APP_INFO_LOG);
		}
		break;
	} //BULLET_NUM

	case GAME_COMMANDS::HELP:
	{
		std::string line1 = "In the Russian Roulette all comrades in the chat will have to shoot themselves to test their luck for the motherland!";
		std::string line2 = "\nThe first user chooses in which gunslot to put the bullet, from slot 1 to 6, decide it by typing '/rr bullet [number from 1-6]'";
		std::string line3 = "\nThe other users will have to wait its turn and type '/rr shoot' when they are ready to fire. The last one standing wins!";

		App->modNetServer->SendServerNotification(line1 + line2 + line3, EntryType::APP_INFO_LOG, user_id);
		break;
	} // HELP

	}// switch (command)
}

void ModuleGamesManager::ProcessSexKillMarry(GAME_COMMANDS command, const std::string& args, uint user_id) {
	if (command != GAME_COMMANDS::STOP && user_id != GetCurrentUserPlaying()) {
		App->modNetServer->SendServerNotification("Hey! It's not your turn!", EntryType::APP_WARN_LOG, user_id);
		return;
	}

	switch (command) {
	default:
		App->modNetServer->SendServerNotification("Invalid command! Try /ksm help", EntryType::APP_WARN_LOG, user_id);
		break;
	case GAME_COMMANDS::SEX:
	{
		size_t user_pos = args.find_first_of(' ');
		std::string user_str = args.substr(0, user_pos);

		bool is_name = false;
		for (int i = 0; i < 3 && !is_name; ++i)
			is_name = user_str == m_GamesData.ksm_names[i];

		if (!is_name) {
			App->modNetServer->SendServerNotification("That is not one of the names!", EntryType::APP_WARN_LOG, user_id);
			break;
		}

		m_GamesData.sex = true;
		m_GameStatus = GAME_STATUS::RUNNING;

		m_GamesData.users_answered += " have Sex with: " + user_str;

		// Make response
		std::string response = "Spicy answer " + GetUserLabel() + "!";
		if (!m_GamesData.kill && !m_GamesData.marry)
			response += "\nYou still have to answer with whom you would marry and who you would kill, little assassin";
		else if (!m_GamesData.kill)
			response += "\nYou still have to answer who you would kill, little assassin";
		else if (!m_GamesData.marry)
			response += "\nYou still have to answer with whom you would marry, we don't want you to live alone :(";

		App->modNetServer->SendServerNotification(response, EntryType::APP_INFO_LOG, user_id);

		break;
	} //SEX
	case GAME_COMMANDS::KILL:
	{
		size_t user_pos = args.find_first_of(' ');
		std::string user_str = args.substr(0, user_pos);

		bool is_name = false;
		for (int i = 0; i < 3 && !is_name; ++i)
			is_name = user_str == m_GamesData.ksm_names[i];

		if (!is_name) {
			App->modNetServer->SendServerNotification("That is not one of the names!", EntryType::APP_WARN_LOG, user_id);
			break;
		}

		m_GamesData.kill = true;
		m_GameStatus = GAME_STATUS::RUNNING;

		m_GamesData.users_answered += " Kill: " + user_str;

		// Make response
		std::string response = "Revealing answer " + GetUserLabel() + "!";
		if (!m_GamesData.sex && !m_GamesData.marry)
			response += "\nYou still have to answer with whom you would marry and with whom you would have sex, don't be embarrassed!";
		else if (!m_GamesData.sex)
			response += "\nYou still have to answer with whom you would have sex, don't be embarrassed!";
		else if (!m_GamesData.marry)
			response += "\nYou still have to answer with whom you would marry, we don't want you to live alone :(";

		App->modNetServer->SendServerNotification(response, EntryType::APP_INFO_LOG, user_id);

		break;
	} //KILL
	case GAME_COMMANDS::MARRY:
	{
		size_t user_pos = args.find_first_of(' ');
		std::string user_str = args.substr(0, user_pos);

		bool is_name = false;
		for (int i = 0; i < 3 && !is_name; ++i)
			is_name = user_str == m_GamesData.ksm_names[i];

		if (!is_name) {
			App->modNetServer->SendServerNotification("That is not one of the names!", EntryType::APP_WARN_LOG, user_id);
			break;
		}

		m_GamesData.marry = true;
		m_GameStatus = GAME_STATUS::RUNNING;

		m_GamesData.users_answered += " Marry with: " + user_str;

		// Make response
		std::string response = "Beautiful answer " + GetUserLabel() + "!";
		if (!m_GamesData.sex && !m_GamesData.kill)
			response += "\nYou still have to answer whom you would kill and with whom you would have sex, don't be embarrassed, little assassin!";
		else if (!m_GamesData.sex)
			response += "\nYou still have to answer with whom you would have sex, don't be embarrassed!";
		else if (!m_GamesData.kill)
			response += "\nYou still have to answer whom you would kill, little assassin!";

		App->modNetServer->SendServerNotification(response, EntryType::APP_INFO_LOG, user_id);

		break;
	} //MARRY
	case GAME_COMMANDS::HELP:
	{
		std::string line1 = "In KILL/SEX/MARRY a user will have to choose, among 3 proposed users, which one it would kill, with which one it would have sex and to which one it would marry";
		std::string line2 = "\n** Do so by typing '/ksm ' followed by the action and the user (as: '/ksm sex [user]' or '/ksm kill [user]') **";

		App->modNetServer->SendServerNotification(line1 + line2, EntryType::APP_INFO_LOG, user_id);
		break;
	} // HELP
	}// switch (command)
}

void ModuleGamesManager::ProcessUnscramble(GAME_COMMANDS command, const std::string& args, uint user_id) {
	if (command != GAME_COMMANDS::STOP && user_id != GetCurrentUserPlaying()) {
		App->modNetServer->SendServerNotification("Hey! It's not your turn!", EntryType::APP_WARN_LOG, user_id);
		return;
	}

	switch (command) {
	default:
		App->modNetServer->SendServerNotification("Invalid command! Try /unscramble help", EntryType::APP_WARN_LOG, user_id);
		break;
	case GAME_COMMANDS::WORD:
	{
		std::string response;
		bool update_game_status = true;

		size_t word_pos = args.find_first_of(' ');
		std::string word_str = args.substr(0, word_pos);

		response = GetUserLabel() + " said the word \"" + word_str + "\"\n";

		if (m_GamesData.original_word == "") {
			response = "Nice word " + GetUserLabel() + "!";
			m_GamesData.original_word = word_str;
			ArrangeWord();
		}
		else {
			if (CompareWords(word_str))
				response = "Good answer " + GetUserLabel() + "!";
			else {
				response = "Incorrect answer " + GetUserLabel() + "! :(\nTry Again!";
				update_game_status = false;
			}
		}

		if (update_game_status)
			m_GameStatus = GAME_STATUS::RUNNING;


		App->modNetServer->SendServerNotification(response, EntryType::APP_INFO_LOG);
		break;
	} //WORD

	case GAME_COMMANDS::HELP:
	{
		std::string line1 = "In Unscramble, a user will write a word and the other users will have to write a word with the same letters";
		std::string line2 = "\n** Do so by typing '/unscramble word [word]' **";

		App->modNetServer->SendServerNotification(line1 + line2, EntryType::APP_INFO_LOG, user_id);
		break;
	} // HELP
	} //switch (command)
}

void ModuleGamesManager::ProcessChainedWords(GAME_COMMANDS command, const std::string& args, uint user_id) {
	if (command != GAME_COMMANDS::STOP && user_id != GetCurrentUserPlaying()) {
		App->modNetServer->SendServerNotification("Hey! It's not your turn!", EntryType::APP_WARN_LOG, user_id);
		return;
	}

	switch (command) {
	default:
		App->modNetServer->SendServerNotification("Invalid command! Try /unscramble help", EntryType::APP_WARN_LOG, user_id);
		break;
	case GAME_COMMANDS::WORD:
	{
		std::string response;
		bool update_game_status = true;

		size_t word_pos = args.find_first_of(' ');
		std::string word_str = args.substr(0, word_pos);

		response = GetUserLabel() + " said the word \"" + word_str + "\"\n";

		if (m_GamesData.original_word == "") {
			response = "Nice word " + GetUserLabel() + "!";
			m_GamesData.original_word = word_str;
		}
		else {
			if (word_str[0] == m_GamesData.original_word[0]) {
				m_GamesData.unscramble_ranking[m_CurrentUser.second]++;
				m_GamesData.original_word = word_str;
				response = "Good answer " + GetUserLabel() + "!\n\n\n";
			}
			else {
				response = "Incorrect answer " + GetUserLabel() + "! :(\n\nTry Again!\n\n\n";
				update_game_status = false;
			}
		}

		if (update_game_status)
			m_GameStatus = GAME_STATUS::RUNNING;


		App->modNetServer->SendServerNotification(response, EntryType::APP_INFO_LOG);
		break;
	} //WORD
	case GAME_COMMANDS::HELP:
	{
		std::string line1 = "In Chained Words, a user will write a word and the other users will have to write a word with the same  first letter";
		std::string line2 = "\n** Do so by typing '/chained word [word]' **";

		App->modNetServer->SendServerNotification(line1+line2, EntryType::APP_INFO_LOG, user_id);
		break;
	}
	} //switch (command)
}
