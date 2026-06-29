#pragma once

#include <string>

#ifdef CLIPBRIDGE_NATIVE_EXPORTS
#define CLIPBRIDGE_API __declspec(dllexport)
#else
#define CLIPBRIDGE_API __declspec(dllimport)
#endif

extern "C" CLIPBRIDGE_API bool StartServer(int port);
extern "C" CLIPBRIDGE_API bool SendText(const char* ipAddress, int port, const char* text);
extern "C" CLIPBRIDGE_API void GetLastMessage(char* buffer, int bufferSize);
extern "C" CLIPBRIDGE_API std::string ReadClipboard();
extern "C" CLIPBRIDGE_API void WriteClipboard(const char* text);
