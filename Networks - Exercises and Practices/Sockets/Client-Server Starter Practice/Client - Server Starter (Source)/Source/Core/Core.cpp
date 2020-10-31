#include "Core.h"

WindowStruct Window = {};
TimeStruct Time = {};
InputController Input = {};
MouseController Mouse = {};


void Log(const char file[], int line, int type, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	const char *basefile = file;
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

	// NOTE: There is a memory leak here, but we will need logs
	// until the application finished, we will go with this...
	char *message = new char[strlen(tmp_string2)+1];
	lstrcpyA(message, tmp_string2);

	LogEntry entry;
	entry.type = type;
	entry.message = message;
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