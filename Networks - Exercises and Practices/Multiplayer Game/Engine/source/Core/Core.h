#ifndef _CORE_H_
#define _CORE_H_

#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "D3D11.lib")


////////////////////////////////////////////////////////////////////////
// USEFUL MACROS
////////////////////////////////////////////////////////////////////////

#define Kilobytes(x) (1024L * x)
#define Megabytes(x) (1024L * Kilobytes(x))
#define Gigabytes(x) (1024L * Megabytes(x))
#define Terabytes(x) (1024L * Gigabytes(x))

#define ArrayCount(Array) (sizeof(Array)/sizeof((Array)[0]))


////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////

#define MAX_LOG_ENTRIES                                  256
#define MAX_LOG_ENTRY_LENGTH                    Kilobytes(1)
#define MAX_SCREENS                                       32
#define MAX_ANIMATION_CLIPS                                4
#define MAX_TASKS                                        128
#define MAX_TEXTURES                                     512
#define MAX_GAME_OBJECTS                                4096
#define MAX_COLLIDERS                       MAX_GAME_OBJECTS
#define MAX_CLIENTS                                        8
#define MAX_NETWORK_OBJECTS                              256

#define SCENE_TRANSITION_TIME_SECONDS                   1.0f
#define DISCONNECT_TIMEOUT_SECONDS                      5.0f
#define PACKET_DELIVERY_TIMEOUT_SECONDS                 0.5f
#define DEFAULT_PACKET_SIZE                     Kilobytes(4)
#define PING_INTERVAL_SECONDS                           0.5f
#define REPLICATION_INTERVAL_SECONDS					0.5f


////////////////////////////////////////////////////////////////////////
// BASIC TYPES
////////////////////////////////////////////////////////////////////////

// NOTE(jesus): These sizes are right for most desktop platforms, but we
// should be cautious about this because they could vary somewhere...

typedef char int8;
typedef short int int16;
typedef long int int32;
typedef long long int int64;

typedef unsigned char uint8;
typedef unsigned short int uint16;
typedef unsigned long int uint32;
typedef unsigned long long int uint64;
typedef unsigned int uint;

typedef float real32;
typedef double real64;


////////////////////////////////////////////////////////////////////////
// FRAMEWORK TYPES
////////////////////////////////////////////////////////////////////////

struct GameObject;
struct Sprite;
struct AnimationClip;
struct Animation;
struct Texture;
struct AudioClip;
struct Collider;
struct Behaviour;
class Task;
class Screen;
class ModuleBehaviour;

enum class ColliderType
{
	NONE,
	PLAYER,
	LASER
};


////////////////////////////////////////////////////////////////////////
// WINDOW
////////////////////////////////////////////////////////////////////////

struct WindowStruct
{
	int width = 0;
	int height = 0;
};

// NOTE(jesus): Global object to access the window
extern WindowStruct Window;


////////////////////////////////////////////////////////////////////////
// TIME
////////////////////////////////////////////////////////////////////////

struct TimeStruct
{
	double time = 0.0f;     // NOTE(jesus): Time in seconds since the application started
	float deltaTime = 0.0f; // NOTE(jesus): Fixed update time step (use this for calculations)
	float frameTime = 0.0f; // NOTE(jesus): Time spend during the last frame (don't use this)
};

// NOTE(jesus): Global object to access the time
extern TimeStruct Time;


////////////////////////////////////////////////////////////////////////
// INPUT
////////////////////////////////////////////////////////////////////////

enum ButtonState { IDLE, PRESS, PRESSED, RELEASE };

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
// CONSOLE_INFO_LOG
////////////////////////////////////////////////////////////////////////
enum EntryType { LOG_INFO, LOG_WARN, LOG_ERROR };

void PublicAppLog(int line, const char* msg, EntryType type);
const char* PrivateAppLog(const char file[], int line, const char* format, ...);

#define APP_LOG(format, ...)			PrivateAppLog(__FILE__, __LINE__, format, __VA_ARGS__)
#define CONSOLE_ERROR_LOG(format, ...)	PublicAppLog(__LINE__, PrivateAppLog(__FILE__, __LINE__, format, __VA_ARGS__), LOG_ERROR)
#define CONSOLE_WARN_LOG(format, ...)	PublicAppLog(__LINE__, PrivateAppLog(__FILE__, __LINE__, format, __VA_ARGS__), LOG_WARN)
#define CONSOLE_INFO_LOG(format, ...)	PublicAppLog(__LINE__, PrivateAppLog(__FILE__, __LINE__, format, __VA_ARGS__), LOG_INFO)

// Assertions
#ifdef ASSERT
	#undef ASSERT
#endif
#define ASSERT(x, ...) if ((x == false)) { CONSOLE_ERROR_LOG(__FILE__, __LINE__, "ASSERTION FAILED: ", __VA_ARGS__); __debugbreak(); }


////////////////////////////////////////////////////////////////////////
// PROFILER
////////////////////////////////////////////////////////////////////////

enum DebugCycleCounterID
{
	DebugCycleCounter_Frame,
	DebugCycleCounter_PreUpdate,
	DebugCycleCounter_Update,
	DebugCycleCounter_PostUpdate,
	DebugCycleCounter_GUIDraw,
	DebugCycleCounter_GOUpdate,
	DebugCycleCounter_GOPreUpdate,
	DebugCycleCounter_Collisions,
	DebugCycleCounter_Collisions1,
	DebugCycleCounter_Collisions2,
	DebugCycleCounter_CollisionTestProfile,
	DebugCycleCounter_NetSend,
	DebugCycleCounter_NetRecv,
	DebugCycleCounter_Render,
	DebugCycleCounter_Count
};

struct DebugCycleCounter
{
	uint64 cycleCount = 0;
	uint32 hitCount = 0;
	const char *label = "";
};

extern DebugCycleCounter DebugCycleCountersBack[DebugCycleCounter_Count];
extern DebugCycleCounter DebugCycleCountersFront[DebugCycleCounter_Count];
void DebugSwapCycleCounters();

#if _MSC_VER
#define BEGIN_TIMED_BLOCK(ID) uint64 beginCycleCount##ID = __rdtsc();
#define END_TIMED_BLOCK(ID) \
	DebugCycleCountersBack[DebugCycleCounter_##ID].label = #ID; \
	DebugCycleCountersBack[DebugCycleCounter_##ID].cycleCount += __rdtsc() - beginCycleCount##ID; \
	DebugCycleCountersBack[DebugCycleCounter_##ID].hitCount++;
#define END_PROFILING_FRAME() DebugSwapCycleCounters()
#else
#define BEGIN_TIMED_BLOCK(ID)
#define END_TIMED_BLOCK(ID)
#define END_PROFILING_FRAME()
#endif



////////////////////////////////////////////////////////////////////////
// RANDOM NUMBER
////////////////////////////////////////////////////////////////////////

class RandomNumberGenerator
{
public:

	RandomNumberGenerator(uint32 seed = 987654321)
	{
		// NOTE(jesus): VERY IMPORTANT
		// The initial seeds z1, z2, z3, z4  MUST be larger than
		// 1, 7, 15, and 127 respectively.
		z1 = seed, z2 = seed, z3 = seed, z4 = seed;
	}

#if 1

	float next(void)
	{
		uint32 b;
		b = ((z1 << 6) ^ z1) >> 13;
		z1 = ((z1 & 4294967294U) << 18) ^ b;
		b = ((z2 << 2) ^ z2) >> 27;
		z2 = ((z2 & 4294967288U) << 2) ^ b;
		b = ((z3 << 13) ^ z3) >> 21;
		z3 = ((z3 & 4294967280U) << 7) ^ b;
		b = ((z4 << 3) ^ z4) >> 12;
		z4 = ((z4 & 4294967168U) << 13) ^ b;
		float result = (float)((z1 ^ z2 ^ z3 ^ z4) * 2.3283064365386963e-10);
		ASSERT(result >= 0.0f && result <= 1.0f);
		return result;
	}

private:

	uint32 z1, z2, z3, z4;

#else

	float next() // Generates a number from 0 to 1
	{
		return (float)((double)rand() / (double)RAND_MAX);
	}

private:

	uint32 seed;
#endif


};

// NOTE(jesus): Global random generation object
extern RandomNumberGenerator Random;


////////////////////////////////////////////////////////////////////////
// FRAMEWORK HEADERS
////////////////////////////////////////////////////////////////////////
#include "Utilities/Maths.h"
#include "Utilities/Color.h"
#include "Utilities/Memory/MemoryStream.h"
#include "Utilities/Memory/ByteSwap.h"

#include "Module.h"

#include "Collisions/ModuleCollision.h"

#include "Foundations/ModulePlatform.h"
#include "Foundations/ModuleTaskManager.h"
#include "Foundations/ModuleResources.h"

#include "GameObjects/ModuleBehaviour.h"
#include "GameObjects/ModuleGameObject.h"

#include "Networks/DeliveryManager.h"
#include "Networks/Messages.h"
#include "Networks/Linking/ModuleLinkingContext.h"
#include "Networks/Replication/ReplicationCommand.h"
#include "Networks/Replication/ReplicationManagerClient.h"
#include "Networks/Replication/ReplicationManagerServer.h"
#include "Networks/NetworkingModules/ModuleNetworking.h"
#include "Networks/NetworkingModules/ModuleNetworkingCommons.h"
#include "Networks/NetworkingModules/ModuleNetworkingClient.h"
#include "Networks/NetworkingModules/ModuleNetworkingServer.h"

#include "Renderer/ModuleRender.h"
#include "Renderer/ModuleTextures.h"

#include "Screens/ModuleScreen.h"
#include "Screens/Screen.h"
#include "Screens/ScreenLoading.h"
#include "Screens/ScreenBackground.h"
#include "Screens/ScreenOverlay.h"
#include "Screens/ScreenMainMenu.h"
#include "Screens/ScreenGame.h"

#include "Sound/ModuleSound.h"
#include "UI/ModuleUI.h"

#include "Application.h"

#endif //_NETWORKS_H_