#include "Application.h"


// NOTE(jesus):
// The following line avoids the black console from appearing.
// It can also be configured in the project linker settings.
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

enum class MainState
{
	CREATE,
	INIT,
	LOOP,
	CLEANUP,
	FAIL,
	EXIT
};


Application * App = nullptr;

int main(int argc, char **argv)
{
	int result = EXIT_FAILURE;
	MainState state = MainState::CREATE;

	while (state != MainState::EXIT)
	{
		switch (state)
		{
			// App Creation
			case MainState::CREATE:
				App = new Application();
				if (App != nullptr)
					state = MainState::INIT;
				else
				{
					LOG("Create failed");
					state = MainState::FAIL;
				}
			break;

			// App Init
			case MainState::INIT:
				if (App->AppInit())
					state = MainState::LOOP;
				else
				{
					LOG("Start failed");
					state = MainState::FAIL;
				}
			break;

			// Update ok, continue here in the loop, updating...
			case MainState::LOOP:
				if (!App->AppUpdate())
					state = MainState::CLEANUP;				
			break;

			// App cleanup
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
					LOG("CleanUp failed");
					state = MainState::FAIL;
				}
			break;

			// App Fail/Exit
			case MainState::FAIL:
				LOG("Application failed :-(");
				state = MainState::EXIT;
			break;

			case MainState::EXIT: break;
			default: break;
		}
	}

	return result;
}