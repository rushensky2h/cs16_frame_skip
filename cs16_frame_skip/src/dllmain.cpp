// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

/*
SCR_UpdateScreen:
{
	build 4554: 
		ida ptr: A1 ? ? ? ? 83 EC 10 56 33 F6 3B C6 0F 85
		code ptr: \xA1\xFF\xFF\xFF\xFF\x83\xEC\x10\x56\x33\xF6\x3B\xC6\x0F\x85
		code mask: x????xxxxxxxxxx

	last steam build 8684:
		ida ptr: 55 8B EC 83 EC 10 A1 ? ? ? ? 56 33 F6 3B C6 0F 85 ? ? ? ?
		code ptr: \x55\x8B\xEC\x83\xEC\x10\xA1\xFF\xFF\xFF\xFF\x56\x33\xF6\x3B\xC6\x0F\x85\xFF\xFF\xFF\xFF
		code mask: xxxxxxx????xxxxxxx?????
}
*/

ModuleInfo HW;
SCR_UpdateScreen_t g_pSCR_UpdateScreen;

void hkSCR_UpdateScreen()
{
	static int count = 0;

	// We skip frames as much as possible to increase the indicator
	// The higher the value, the fewer frames
	const int skip_frames = 1;

	if (count <= skip_frames)
		count++;

	if (count > skip_frames)
	{
		count  = 0;
		return;
	}

	g_pSCR_UpdateScreen();
}

void ProcessAttach(HMODULE hModule)
{
	if (GetLastError() != ERROR_ALREADY_EXISTS)
	{
		if (!FindModuleByName("hw.dll", &HW))
			return;

		// Build: 8684
		g_pSCR_UpdateScreen = (SCR_UpdateScreen_t)FindPattern("\xA1\xFF\xFF\xFF\xFF\x83\xEC\x10\x56\x33\xF6\x3B\xC6\x0F\x85", "x????xxxxxxxxxx", HW.base, HW.end);

		if (MH_Initialize() != MH_OK)
			return;

		if (!g_pSCR_UpdateScreen)
			return;
		
		MH_CreateHook((LPVOID)g_pSCR_UpdateScreen, hkSCR_UpdateScreen, (LPVOID*)&g_pSCR_UpdateScreen);
		MH_EnableHook((LPVOID)g_pSCR_UpdateScreen);
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			ProcessAttach(hModule);
			break;
	}
	return TRUE;
}

bool FindModuleByName(const char* name, ModuleInfo* module)
{
	if (!strlen(name))
		return false;

	HMODULE hModuleDll = GetModuleHandleA(name);

	if (!hModuleDll)
		return false;

	if (hModuleDll == INVALID_HANDLE_VALUE)
		return false;

	MEMORY_BASIC_INFORMATION mem;

	if (!VirtualQuery(hModuleDll, &mem, sizeof(mem)))
		return false;

	if (mem.State != MEM_COMMIT)
		return false;

	if (!mem.AllocationBase)
		return false;

	IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)mem.AllocationBase;
	IMAGE_NT_HEADERS* pe = (IMAGE_NT_HEADERS*)((uintptr_t)dos + (uintptr_t)dos->e_lfanew);

	if (pe->Signature != IMAGE_NT_SIGNATURE)
		return false;

	module->handle = hModuleDll;
	module->base = (uintptr_t)(mem.AllocationBase);
	module->size = (uintptr_t)(pe->OptionalHeader.SizeOfImage);
	module->end = module->base + module->size - sizeof(uintptr_t);

	return true;
}

DWORD FindPattern(PCSTR pattern, PCSTR mask, DWORD start, DWORD end, DWORD offset)
{
	if (start > end)
	{
		auto reverse = end;

		end = start;
		start = reverse;
	}

	size_t patternLength = lstrlenA(pattern);
	bool found = false;

	for (DWORD i = start; i < end - patternLength; ++i)
	{
		found = true;

		for (size_t idx = 0; idx < patternLength; ++idx)
		{
			if (mask[idx] == 'x' && pattern[idx] != *(PCHAR)(i + idx))
			{
				found = false;
				break;
			}
		}

		if (found)
			return i + offset;
	}

	return 0;
}
