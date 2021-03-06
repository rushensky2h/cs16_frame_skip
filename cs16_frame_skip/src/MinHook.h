#pragma once

#if !(defined _M_IX86) && !(defined _M_X64) && !(defined __i386__ ) && !( defined __x86_64__ )
#error MinHook only supports x64 and x86 systems
#endif

#define MH_ALL_HOOKS NULL

typedef enum MH_STATUS
{
	MH_UNKNOWN = -1,

	// Success
	MH_OK = 0,

	// MinHook is already initialized
	MH_ERROR_ALREADY_INITIALIZED,

	// MinHook is not initialized yet or is not already initialized
	MH_ERROR_NOT_INITIALIZED,

	// The hook for the specified target function has already been created
	MH_ERROR_ALREADY_CREATED,

	// The hook for the specified target function has not yet been created
	MH_ERROR_NOT_CREATED,

	// The hook for the specified target function is already enabled
	MH_ERROR_ENABLED,

	// The hook for the specified target function is not enabled yet or is already disabled
	MH_ERROR_DISABLED,

	// The specified pointer is invalid. Specifies the address of the Unallocated
	// and / or non-executable region
	MH_ERROR_NOT_EXECUTABLE,

	// The specified target function cannot be enabled
	MH_ERROR_UNSUPPORTED_FUNCTION,

	// Failed to allocate memory
	MH_ERROR_MEMORY_ALLOC,

	// Failed to change memory protection
	MH_ERROR_MEMORY_PROTECT,

	// The specified module is not loaded
	MH_ERROR_MODULE_NOT_FOUND,

	// The specified function was not found
	MH_ERROR_FUNCTION_NOT_FOUND
}
MH_STATUS;

// Can be passed as a parameter to MH_EnableHook, MH_DisableHook,
// MH_QueueEnableHook or MH_QueueDisableHook.
#define MH_ALL_HOOKS NULL

#ifdef __cplusplus
extern "C"
{
#endif

	// Initializing the Winsock library. You should only call this function ONCE at the beginning of your program
	MH_STATUS WINAPI MH_Initialize(VOID);

	// Initialize the Winsock library. You must call this function EXACTLY
	// ONCE at the end of your program.
	MH_STATUS WINAPI MH_Uninitialize(VOID);

	// Creates a hook for the specified target function in the disabled state.
	// Parameters:
	// pTarget[in] Pointer to the target function that will be
	// overridden by the crawl function.
	// pDetour [in] A pointer to a bypass function, which will override
	// the target function.
	// ppOriginal [out] Pointer to the trampoline function that will
	// used to call the original target function.
	// This parameter can be NULL.
	MH_STATUS WINAPI MH_CreateHook(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal);

	// Creates a hook for the specified API function in the disabled state.
	// Parameters:
	// pszModule[in] Pointer to the name of the loaded module that contains
	// target function.
	// pszTarget [in] Pointer to the name of the target function that will be
	// overridden by the crawl function.
	// pDetour [in] A pointer to a bypass function, which will override
	// the target function.
	// ppOriginal [out] Pointer to the trampoline function that will
	// used to call the original target function.
	// This parameter can be NULL.
	MH_STATUS WINAPI MH_CreateHookApi(LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, LPVOID* ppOriginal);

	// Creates a hook for the specified API function in the disabled state.
	// Parameters:
	// pszModule[in] Pointer to the name of the loaded module that contains
	// target function.
	// pszTarget [in] Pointer to the name of the target function that will be
	// overridden by the crawl function.
	// pDetour [in] A pointer to a bypass function, which will override
	// the target function.
	// ppOriginal [out] Pointer to the trampoline function that will
	// used to call the original target function.
	// This parameter can be NULL.
	// ppTarget [out] Pointer to the target function to be used
	// with other functions.
	// This parameter can be NULL.
	MH_STATUS WINAPI MH_CreateHookApiEx(LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, LPVOID* ppOriginal, LPVOID* ppTarget);

	// Deletes an already created hook.
	// Parameters:
	// pTarget[in] Pointer to the target function.
	MH_STATUS WINAPI MH_RemoveHook(LPVOID pTarget);

	// Enables an already created hook.
	// Parameters:
	// pTarget[in] Pointer to the target function.
	// If this parameter is MH_ALL_HOOKS, all created hooks
	// enabled at a time.
	MH_STATUS WINAPI MH_EnableHook(LPVOID pTarget);

	// Disables an already created hook.
	// Parameters:
	// pTarget[in] Pointer to the target function.
	// If this parameter is MH_ALL_HOOKS, all created hooks
	// disabled at a time.
	MH_STATUS WINAPI MH_DisableHook(LPVOID pTarget);

	// Queues for enabling an already created hook.
	// Parameters:
	// pTarget[in] Pointer to the target function.
	// If this parameter is MH_ALL_HOOKS, all created hooks
	// queued for inclusion
	MH_STATUS WINAPI MH_QueueEnableHook(LPVOID pTarget);

	// Queues for disabling an already created hook.
	// Parameters:
	// pTarget[in] Pointer to the target function.
	// If this parameter is MH_ALL_HOOKS, all created hooks
	// in the queue for disconnection.
	MH_STATUS WINAPI MH_QueueDisableHook(LPVOID pTarget);

	// Applies all changes to the queue at once.
	MH_STATUS WINAPI MH_ApplyQueued(VOID);

	// Translates MH_STATUS to its name as a string.
	const char* WINAPI MH_StatusToString(MH_STATUS status);

#ifdef __cplusplus
}
#endif
