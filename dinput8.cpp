#include "stdafx.h"
#include "dinput8.h"
#include "MinHook/MinHook.h"
#include "d3d9.h"
#include "SaveBackup.h"
#include "Misc.h"
#include "Cheats.h"
#include "InGameClock.h"
#include "Hotkeys.h"
#include "ItemEditor.h"
#include "PlayerStats.h"
#include "Server.h"
#include "Portcrystals.h"
#include "WeaponSets.h"
#include "DamageLog.h"
#include "InGameUI.h"

FILE *logFile = nullptr;
iniConfig config(".\\dinput8.ini");
BYTE **pBase = nullptr, **pWorld = nullptr;

namespace
{

typedef HRESULT(WINAPI *tDirectInput8Create)(HINSTANCE inst_handle, DWORD version, const IID& r_iid, LPVOID* out_wrapper, LPUNKNOWN p_unk);
tDirectInput8Create oDirectInput8Create = nullptr;

BYTE *codeBase, *codeEnd, *dataBase, *dataEnd;

void InitHooks()
{
	BYTE sigBase[] = { 0x8B, 0x15, 0xCC, 0xCC, 0xCC, 0xCC, 0x33, 0xDB, 0x8B, 0xF8 };
	BYTE sigWorld[] = { 0x89, 0x35, 0xCC, 0xCC, 0xCC, 0xCC, 0x89, 0xBE, 0x70, 0x09, 0x00, 0x00 };
	BYTE *pOffset;
	if (Hooks::FindSignature("BasePointer", sigBase, &pOffset))
		pBase = *(BYTE***)(pOffset + 2);

	if (Hooks::FindSignature("WorldPointer", sigWorld, &pOffset))
		pWorld = *(BYTE***)(pOffset + 2);

	Hooks::SaveBackup();
	Hooks::Hotkeys();
	Hooks::Misc();
	Hooks::Cheats();
	Hooks::Server();
	Hooks::WeaponSets();
	if (Hooks::InGameUI())
	{
		Hooks::Portcrystals();
		Hooks::PlayerStats();
		Hooks::ItemEditor();
		Hooks::InGameClock();
		Hooks::DamageLog();
	}
}

void Initialize()
{
	logFile = fopen("dinput8.log", "w");
	config.init();

	DWORD base = (DWORD)GetModuleHandle(nullptr);
	auto idh = (PIMAGE_DOS_HEADER)base;
	auto inh = (PIMAGE_NT_HEADERS)(base + idh->e_lfanew);
	auto ioh = &inh->OptionalHeader;
	codeBase = (BYTE*)(base + ioh->BaseOfCode);
	codeEnd = codeBase + ioh->SizeOfCode;
	dataBase = (BYTE*)(base + ioh->BaseOfData);
	dataEnd = dataBase + ioh->SizeOfInitializedData;

	MH_STATUS status = MH_Initialize();
	LOG("MH_Initialize: %s\n", MH_StatusToString(status));
	InitHooks();

	string loadLibrary = config.getStr("main", "loadLibrary");
	if (!loadLibrary.empty())
	{
		HMODULE hMod = LoadLibrary(loadLibrary.c_str());
		oDirectInput8Create = (tDirectInput8Create)GetProcAddress(hMod, "DirectInput8Create");
	}

	if (!oDirectInput8Create)
	{
		CHAR syspath[MAX_PATH];
		GetSystemDirectory(syspath, MAX_PATH);
		strcat_s(syspath, "\\dinput8.dll");
		HMODULE hMod = LoadLibrary(syspath);
		oDirectInput8Create = (tDirectInput8Create)GetProcAddress(hMod, "DirectInput8Create");
	}
}

void Unitialize()
{
	MH_STATUS status1 = MH_DisableHook(MH_ALL_HOOKS);
	MH_STATUS status2 = MH_Uninitialize();

	if (logFile)
	{
		fprintf(logFile, "MH_DisableHook: %s\n", MH_StatusToString(status1));
		fprintf(logFile, "MH_Uninitialize: %s\n", MH_StatusToString(status2));
		fclose(logFile);
		logFile = nullptr;
	}
}

}

void Hooks::CreateHook(LPCSTR msg, LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal, bool enable)
{
	MH_STATUS status1 = MH_CreateHook(pTarget, pDetour, ppOriginal);

	if (enable)
	{
		MH_STATUS status2 = MH_EnableHook(pTarget);
		LOG("%s hook: %s, %s\n", msg, MH_StatusToString(status1), MH_StatusToString(status2));
	}
	else
	{
		LOG("%s hook: %s, disabled\n", msg, MH_StatusToString(status1));
	}
}

void Hooks::SwitchHook(LPCSTR msg, LPVOID pTarget, bool enable)
{
	if (enable)
	{
		MH_STATUS status = MH_EnableHook(pTarget);
		LOG("%s enable: %s\n", msg, MH_StatusToString(status));
	}
	else
	{
		MH_STATUS status = MH_DisableHook(pTarget);
		LOG("%s disable: %s\n", msg, MH_StatusToString(status));
	}
}

bool Hooks::FindSignature(LPCSTR msg, BYTE* signature, size_t len, BYTE** offset) { return Find(msg, codeBase, codeEnd, signature, len, offset); }
bool Hooks::FindData(LPCSTR msg, BYTE* signature, size_t len, BYTE** offset) { return Find(msg, dataBase, dataEnd, signature, len, offset); }
bool Hooks::Find(LPCSTR msg, BYTE* start, BYTE* end, BYTE *signature, size_t len, BYTE **offset)
{
	for (*offset = start; *offset < end; (*offset)++)
	{
		for (unsigned int i = 0; i < len; i++)
		{
			if (signature[i] != 0xCC && signature[i] != (*offset)[i])
				break;
			if (i == len - 1)
			{
				LOG("%s pointer: %08X\n", msg, (UINT)*offset);
				return true;
			}
		}
	}
	LOG("%s pointer: not found\n", msg);
	return false;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		Initialize();
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
		Unitialize();
	return TRUE;
}

extern "C" __declspec(dllexport) 
HRESULT WINAPI DirectInput8Create(HINSTANCE inst_handle, DWORD version, const IID& r_iid, LPVOID* out_wrapper, LPUNKNOWN p_unk)
{
	return oDirectInput8Create(inst_handle, version, r_iid, out_wrapper, p_unk);
}
