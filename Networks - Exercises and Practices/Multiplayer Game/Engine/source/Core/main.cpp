#include "Application.h"


// NOTE(jesus):
// The following line avoids the black console from appearing.
// It can also be configured in the project linker settings.
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

Application * App = nullptr;

enum class MainState
{
	CREATE,
	INIT,
	LOOP,
	CLEANUP,
	FAIL,
	EXIT
};

int main(int argc, char **argv)
{
	int result = EXIT_FAILURE;
	MainState state = MainState::CREATE;

	while (state != MainState::EXIT)
	{
		switch (state)
		{
			case MainState::CREATE:

				App = new Application();
				if (App != nullptr)
					state = MainState::INIT;
				else
				{
					CONSOLE_ERROR_LOG("Create failed");
					state = MainState::FAIL;
				}
				break;

			case MainState::INIT:

				if (App->AppInit())
					state = MainState::LOOP;
				else
				{
					CONSOLE_ERROR_LOG("Start failed");
					state = MainState::FAIL;
				}
				break;

			case MainState::LOOP:

				if (!App->AppUpdate())
					state = MainState::CLEANUP; // Update ok, continue here...					
				break;

			case MainState::CLEANUP:

				if (App->AppCleanUp())
				{
					delete App;
					App = nullptr;

					state = MainState::EXIT;
					result = EXIT_SUCCESS;
				}
				else
				{
					CONSOLE_ERROR_LOG("CleanUp failed");
					state = MainState::FAIL;
				}
				break;

			case MainState::FAIL:

				CONSOLE_ERROR_LOG("Application failed :-(");
				state = MainState::EXIT;
				break;

			case MainState::EXIT:	break;
			default:				break;
		}
	}

	return result;
}