#include "Core/Core.h"

WindowStruct Window = {};
TimeStruct Time = {};
InputController Input = {};
MouseController Mouse = {};

RandomNumberGenerator Random;


// --- Logging ---
const char* PrivateAppLog(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	const char* basefile = file;
	const size_t filelen = strlen(file);

	for (size_t i = 0; i < filelen; ++i)
		if (file[i] == '\\' || file[i] == '/')
			basefile = file + i + 1;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "%s(%d) : %s\n", basefile, line, tmp_string);

	// Windows debug output
	OutputDebugString(tmp_string2);
	return tmp_string2;
}


void PublicAppLog(int line, const char* msg, EntryType type)
{
	std::string cutted_message = msg;

	size_t size = cutted_message.find_first_of(":  ", 0) + 3;
	if (size != std::string::npos)
		cutted_message = cutted_message.substr(size, cutted_message.size());

	Color color = Color();
	switch (type)
	{
		case LOG_ERROR:	color.SetColor(Colors::ConsoleRed);		break; // Red
		case LOG_WARN:	color.SetColor(Colors::ConsoleYellow);	break; // Yellow
		case LOG_INFO:	color.SetColor(Colors::ServerGreen);	break; // Blue
	}

	if (App && App->modUI)
		App->modUI->PrintMessageInConsole(cutted_message, color);
}

// --- Profiler ---
DebugCycleCounter DebugCycleCountersBack[DebugCycleCounter_Count] = {};
DebugCycleCounter DebugCycleCountersFront[DebugCycleCounter_Count] = {};

void DebugSwapCycleCounters()
{
	for (int i = 0; i < DebugCycleCounter_Count; ++i)
	{
		DebugCycleCountersFront[i] = DebugCycleCountersBack[i];
		DebugCycleCountersBack[i].hitCount = 0;
		DebugCycleCountersBack[i].cycleCount = 0;
	}
}
