#include "Application.h"


// NOTE(jesus):
// The following line avoids the black console from appearing.
// It can also be configured in the project linker settings.
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

enum class MainState
{
	Create,
	Init,
	Loop,
	CleanUp,
	Fail,
	Exit
};


Application * App = nullptr;

int main(int argc, char **argv)
{
	int result = EXIT_FAILURE;
	MainState state = MainState::Create;

	while (state != MainState::Exit)
	{
		switch (state)
		{
			// App Creation
			case MainState::Create:
				App = new Application();
				if (App != nullptr)
					state = MainState::Init;
				else
				{
					LOG("Create failed");
					state = MainState::Fail;
				}
			break;

			// App Init
			case MainState::Init:
				if (App->init())
					state = MainState::Loop;
				else
				{
					LOG("Start failed");
					state = MainState::Fail;
				}
			break;

			// Update ok, continue here in the loop, updating...
			case MainState::Loop:
				if (!App->update())
					state = MainState::CleanUp;				
			break;

			// App cleanup
			case MainState::CleanUp:
				if (App->cleanUp())
				{
					delete App;
					App = nullptr;
					state = MainState::Exit;
					result = EXIT_SUCCESS;
				}
				else
				{
					LOG("CleanUp failed");
					state = MainState::Fail;
				}
			break;

			// App Fail/Exit
			case MainState::Fail:
				LOG("Application failed :-(");
				state = MainState::Exit;
			break;

			case MainState::Exit: break;
			default: break;
		}
	}

	return result;
}