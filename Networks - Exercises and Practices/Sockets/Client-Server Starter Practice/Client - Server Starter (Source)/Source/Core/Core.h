#pragma once

// Libraries
#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "D3D11.lib")


////////////////////////////////////////////////////////////////////////
// BASIC TYPES
////////////////////////////////////////////////////////////////////////

// NOTE(jesus): These sizes are right for most platforms, but we should
// should be cautious about this because they could vary somewhere...

typedef char int8;
typedef short int int16;
typedef long int int32;
typedef long long int int64;

typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned long int uint32;
typedef unsigned long long int uint64;

typedef float real32;
typedef double real64;


////////////////////////////////////////////////////////////////////////
// FRAMEWORK TYPES
////////////////////////////////////////////////////////////////////////

struct GameObject;
struct Texture;

class Task;


////////////////////////////////////////////////////////////////////////
// WINDOW
////////////////////////////////////////////////////////////////////////

struct WindowStruct
{
	int width = 0;
	int height = 0;
};

// NOTE(jesus): Global objet to access window dimensions
extern WindowStruct Window;


////////////////////////////////////////////////////////////////////////
// TIME
////////////////////////////////////////////////////////////////////////

struct TimeStruct
{
	double time = 0.0f;      // NOTE(jesus): Time in seconds since the application started
	float deltaTime = 0.0f; // NOTE(jesus): Fixed update time step (use this for calculations)
	float frameTime = 0.0f; // NOTE(jesus): Time spend during the last frame
};

// NOTE(jesus): Global object to access the time
extern TimeStruct Time;


////////////////////////////////////////////////////////////////////////
// INPUT
////////////////////////////////////////////////////////////////////////

enum ButtonState { Idle, Press, Pressed, Release };

struct InputController
{
	bool isConnected = false;
	float verticalAxis = 0.0f;
	float horizontalAxis = 0.0f;

	union
	{
		ButtonState buttons[8] = {};
		struct
		{
			ButtonState actionUp;
			ButtonState actionDown;
			ButtonState actionLeft;
			ButtonState actionRight;
			ButtonState leftShoulder;
			ButtonState rightShoulder;
			ButtonState back;
			ButtonState start;
		};
	};
};

struct MouseController
{
	int16 x = 0;
	int16 y = 0;
	ButtonState buttons[5] = {};
};

// NOTE(jesus): Global object to access the input controller
extern InputController Input;

// NOTE(jesus): Global object to access the mouse
extern MouseController Mouse;


////////////////////////////////////////////////////////////////////////
// MACROS
////////////////////////////////////////////////////////////////////////

#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT(x, ...) if ((!x)) { PrivateAppLog(__FILE__, __LINE__, "ASSERTION FAILED: ", __VA_ARGS__); __debugbreak(); }

#ifdef PI
#undef PI
#endif
#define PI 3.14159265359f

#define Kilobytes(x) (1024L * x)
#define Megabytes(x) (1024L * Kilobytes(x))
#define Gigabytes(x) (1024L * Megabytes(x))
#define Terabytes(x) (1024L * Gigabytes(x))


////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////

#define MAX_SCREENS           32
#define MAX_TASKS            128
#define MAX_TEXTURES         512
#define MAX_GAME_OBJECTS    2048
#define MAX_CLIENTS            4

#define SCENE_TRANSITION_TIME      1.0f
#define PACKET_SIZE        Kilobytes(4)


////////////////////////////////////////////////////////////////////////
// MATH
////////////////////////////////////////////////////////////////////////

inline float DegreesToRadians(float degrees) { return float(PI * degrees / 180.0f); }
inline float GetFractionalPart(float number) { return float(number - (int)number); }
inline float ClampValue(float val) { return float(std::max(0.0f, std::min(val, 1.0f))); }


////////////////////////////////////////////////////////////////////////
// LOG
////////////////////////////////////////////////////////////////////////
// NOTE(jesus):
// Use log just like standard printf function.
// Example: LOG("New user connected %s\n", usernameString);
enum EntryType { APP_ERROR_LOG = 0, APP_WARN_LOG, APP_INFO_LOG };

void PublicAppLog(const char* msg, EntryType type);
const char* PrivateAppLog(const char file[], int line, const char* format, ...);

#define APP_LOG(format, ...)				PrivateAppLog(__FILE__, __LINE__, format, __VA_ARGS__)
#define APPCONSOLE_ERROR_LOG(format, ...)	PublicAppLog(PrivateAppLog(__FILE__, __LINE__, format, __VA_ARGS__), APP_ERROR_LOG)
#define APPCONSOLE_WARN_LOG(format, ...)	PublicAppLog(PrivateAppLog(__FILE__, __LINE__, format, __VA_ARGS__), APP_WARN_LOG)
#define APPCONSOLE_INFO_LOG(format, ...)	PublicAppLog(PrivateAppLog(__FILE__, __LINE__, format, __VA_ARGS__), APP_INFO_LOG)


// --- Utilities Includes ---
#include "Utilities/Color.h"

struct LogEntry
{
	LogEntry(const char* msg, const Color& col) : message(msg), text_color(col) {}
	const Color text_color;
	const char *message;
};

std::vector<LogEntry> logLines;
inline const uint32 GetLogEntryCount();
inline const LogEntry GetLogEntry(uint32 entryIndex);
inline void PushLogEntry(const LogEntry& entry);


////////////////////////////////////////////////////////////////////////
// FRAMEWORK HEADERS
////////////////////////////////////////////////////////////////////////

// --- Networking Includes ---
#include "NetStreams/Messages.h"
#include "NetStreams/MemoryStream.h"
#include "NetStreams/MemoryStream.cpp"

// --- Modules Include ---
#include "Modules/Module.h"

// Foundation
#include "Modules/Foundation/ModulePlatform.h"
#include "Modules/Foundation/ModuleResources.h"
#include "Modules/Foundation/ModuleTaskManager.h"

// Network
#include "Modules/Network/ModuleNetworking.h"
#include "Modules/Network/ModuleNetworkingClient.h"
#include "Modules/Network/ModuleNetworkingServer.h"

// Rendering
#include "Modules/Rendering/ModuleRender.h"
#include "Modules/Rendering/ModuleTextures.h"

// Scene
#include "Modules/Scene/ModuleGameObject.h"
#include "Modules/Scene/ModuleScreen.h"
#include "Modules/Scene/ModuleUI.h"

// Screens Include
#include "Screens/Screen.h"
#include "Screens/ScreenBackground.h"
#include "Screens/ScreenGame.h"
#include "Screens/ScreenLoading.h"
#include "Screens/ScreenMainMenu.h"
#include "Screens/ScreenOverlay.h"

// --- Application Include ---
#include "Application.h"