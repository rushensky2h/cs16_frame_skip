#pragma once

// Exclude rarely used components from Windows headers
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

// https://github.com/TsudaKageyu/minhook
#include "MinHook.h"
#pragma comment(lib, "MinHook.lib")

struct ModuleInfo
{
	HMODULE		handle;
	uintptr_t	base;
	uintptr_t	size;
	uintptr_t	end;
};

typedef void(*SCR_UpdateScreen_t)();

bool FindModuleByName(const char *name, ModuleInfo *module);
DWORD FindPattern(PCSTR pattern, PCSTR mask, DWORD start, DWORD end, DWORD offset = 0);
