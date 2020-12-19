#include <windows.h>
#include <stdio.h>

#include "logger.h"

// ���O�o��
void Logger::Print(const char* format, ...)
{
	char message[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(message, sizeof(message), format, args);
	va_end(args);

	::OutputDebugStringA(message);
}
