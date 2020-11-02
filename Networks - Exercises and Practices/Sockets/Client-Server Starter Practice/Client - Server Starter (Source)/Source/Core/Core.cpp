#include "Core.h"
#include <strsafe.h>

WindowStruct Window = {};
TimeStruct Time = {};
InputController Input = {};
MouseController Mouse = {};

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


void PublicAppLog(const char* msg, EntryType type)
{
	// NOTE: There is a memory leak here, but we will need logs
	// until the application finished, we will go with this...
	char *message = new char[strlen(msg)+1];
	//lstrcpyA(message, msg);
	StringCchCopyA(message, strlen(msg) + 1, msg); // A safer function than lstrcpyA() --> lstrcpy has undefined behavior if src & dest buffers overlap

	Color color = Color();
	switch (type)
	{
		case APP_ERROR_LOG:	color.SetColor(Colors::ConsoleRed);		// Red
		case APP_WARN_LOG:	color.SetColor(Colors::ConsoleYellow);	// Yellow
		case APP_INFO_LOG:	color.SetColor(Colors::ConsoleBlue);	// Blue
	}

	LogEntry entry = LogEntry(message, color);
	logLines.push_back(entry);
}

inline const uint32 GetLogEntryCount()
{
	return (uint32)logLines.size();
}

inline const LogEntry GetLogEntry(uint32 logLineIndex)
{
	ASSERT((logLineIndex < logLines.size()), "LogLineIndex passed was Invalid!");
	return logLines[logLineIndex];
}

inline void PushLogEntry(const LogEntry& entry)
{
	logLines.push_back(entry);
}