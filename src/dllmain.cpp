// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

ModuleInfo HW;

SCR_UpdateScreen_t g_pSCR_UpdateScreen;

void SCR_UpdateScreen()
{
	static int m_iCount = 0;

	// We skip frames as much as possible to increase the indicator
	// The higher the value, the fewer frames
	const int m_iSkipFrames = 1;

	if (m_iCount <= m_iSkipFrames)
		m_iCount++;

	if (m_iCount > m_iSkipFrames)
	{
		m_iCount = 0;
		return;
	}

	auto oSCR_UpdateScreen = (SCR_UpdateScreen_t)g_pSCR_UpdateScreen;

	oSCR_UpdateScreen();
}

PVOID FindSCRUpdateScreen( )
{
	uintptr_t ptr = FindPush((PCHAR)"Connection to server lost during level change.", HW.base, HW.end);
	ptr = FindPattern((PCHAR) "90 90 90", ptr, HW.base, 0x90);

	while (*(PBYTE)ptr == 0x90)
		ptr++;

	return (PVOID) ptr;
}

void ProcessAttach(HMODULE hModule)
{
    if (GetLastError() != ERROR_ALREADY_EXISTS)
    {
		if (!FindModuleByName("hw.dll", &HW))
			return;

		// 55 8B EC 83 EC 10 A1 ? ? ? ? 56 33 F6 3B C6 0F 85 ? ? ? ? 
		g_pSCR_UpdateScreen = ((SCR_UpdateScreen_t)FindSCRUpdateScreen());

		if (g_pSCR_UpdateScreen != nullptr)
		{
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DetourAttach(&(PVOID&)g_pSCR_UpdateScreen, SCR_UpdateScreen);
			DetourTransactionCommit();
		}
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
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
	if (!name || !strlen(name))
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

uintptr_t FindPush(PCHAR message, uintptr_t start, uintptr_t end, uintptr_t offset)
{
	char bPushAddrPattern[] = { 0x68, 0x00, 0x00, 0x00, 0x00, 0x00 };
	auto ptr = FindDownPattern(message, start, end);
	*(PDWORD)&bPushAddrPattern[1] = ptr;
	ptr = FindDownPattern((PCHAR)bPushAddrPattern, start, end);
	return ptr + offset;
}

uintptr_t FindDownPattern(PCHAR pattern, uintptr_t start, uintptr_t end, uintptr_t offset)
{
	if (!start || !end)
		return 0;

	size_t patternLength = strlen(pattern);

	for (auto i = start; i < end - patternLength; ++i)
	{
		bool found = true;

		for (size_t idx = 0; idx < patternLength; ++idx)
		{
			if (pattern[idx] != *(PCHAR)(i + idx))
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


