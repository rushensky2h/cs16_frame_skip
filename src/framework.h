#pragma once

#define WIN32_LEAN_AND_MEAN  // Exclude rarely used components from Windows headers
#include <windows.h>

#include "detours.h"
#pragma once(lib, "detours.lib");

struct ModuleInfo
{
	HMODULE		handle;
	uintptr_t	base;
	uintptr_t	size;
	uintptr_t	end;
};

typedef int(*SCR_UpdateScreen_t)();
extern SCR_UpdateScreen_t g_pSCR_UpdateScreen;

bool FindModuleByName(const char* name, ModuleInfo* module);
DWORD FindPattern(PCSTR pattern, PCSTR mask, DWORD start, DWORD end, DWORD offset = 0);
uintptr_t FindDownPattern(PCHAR pattern, uintptr_t start, uintptr_t end, uintptr_t offset = 0);
uintptr_t FindPush(PCHAR message, uintptr_t start, uintptr_t end, uintptr_t offset = 0);
