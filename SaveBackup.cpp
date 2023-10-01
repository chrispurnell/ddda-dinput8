#include "stdafx.h"
#include "SaveBackup.h"
#include "dinput8.h"
#include "steam_api.h"

string saveDir, savePath;
int saveLimit;
void printError(LPCSTR msg, DWORD error)
{
	if (error == ERROR_FILE_NOT_FOUND)
		LOG("%s: file not found\n", msg);
	else if (error == ERROR_PATH_NOT_FOUND)
		LOG("%s: path not found\n", msg);
	else if (error == ERROR_INVALID_NAME)
		LOG("%s: invalid path\n", msg);
	else
		LOG("%s: %08X\n", msg, (UINT)error);
}

void clearBackups()
{
	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile((saveDir + "ddda_*.sav").c_str(), &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	std::vector<std::pair<UINT64, string>> files;
	do
	{
		SYSTEMTIME t;
		if (sscanf_s(ffd.cFileName, "ddda_%04hd-%02hd-%02hd_%02hd-%02hd-%02hd.sav",
			&t.wYear, &t.wMonth, &t.wDay, &t.wHour, &t.wMinute, &t.wSecond) == 6)
		{
			UINT64 time;
			SystemTimeToFileTime(&t, (LPFILETIME)&time);
			files.push_back({ time, ffd.cFileName });
		}
	} while (FindNextFile(hFind, &ffd));
	FindClose(hFind);

	sort(files.begin(), files.end());
	for (int i = 0; i < (int)files.size() - saveLimit; i++)
	{
		string file = saveDir + files[i].second;
		DeleteFile(file.c_str());
		LOG("Backup deleted: %s\n", file.c_str());
	}
}

void __stdcall handleSave()
{
	WIN32_FILE_ATTRIBUTE_DATA fileData;
	if (GetFileAttributesEx(savePath.c_str(), GetFileExInfoStandard, &fileData))
	{
		SYSTEMTIME systemTime, t;
		FileTimeToSystemTime(&fileData.ftLastWriteTime, &systemTime);
		SystemTimeToTzSpecificLocalTime(nullptr, &systemTime, &t);

		CHAR str[64];
		sprintf_s(str, "ddda_%04hd-%02hd-%02hd_%02hd-%02hd-%02hd.sav", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
		string backupPath = saveDir + str;

		if (CopyFile(savePath.c_str(), backupPath.c_str(), FALSE))
		{
			LOG("Backup created: %s\n", backupPath.c_str());
			if (saveLimit > 0)
				clearBackups();
		}
		else
			printError("Creating backup", GetLastError());
	}
	else
		printError("Finding save", GetLastError());
}

LPVOID oSaveGame = nullptr;
void __declspec(naked) HSaveGame()
{
	asm("pusha");
	asm("call	%0" : : "m"(handleSave));
	asm("popa");
	asm("jmp	*%0" : : "m"(oSaveGame));
}

bool findSavePath()
{
	string configDir = config.getStr("main", "savePath");
	if (configDir.empty())
	{
		HMODULE hModule = GetModuleHandle("steam_api.dll");
		if (hModule)
		{
			char buf[1024];
			tSteamUser pSteamUser = (tSteamUser)GetProcAddress(hModule, "SteamUser");
			if (pSteamUser && pSteamUser() && pSteamUser()->GetUserDataFolder(buf, 1024))
			{
				saveDir = string(buf);
				size_t index = saveDir.rfind("local");
				if (index != string::npos)
				{
					saveDir.erase(index);
					saveDir += "remote";
				}
			}
		}
	}
	else
		saveDir = configDir;
	saveDir.erase(saveDir.find_last_not_of('\\') + 1);

	DWORD attributes = GetFileAttributes(saveDir.c_str());
	if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
	{
		if (configDir.empty())
			LOG("SaveBackup path: NOT FOUND, SET MANUALLY IN DINPUT8.INI\n");
		else
			LOG("SaveBackup path: INVALID PATH - %s\n", saveDir.c_str());
		return false;
	}

	saveDir.push_back('\\');
	savePath = saveDir + "ddda.sav";
	LOG("SaveBackup path: %s\n", savePath.c_str());
	return true;
}

void Hooks::SaveBackup()
{
	if (!config.getBool("main", "backupSaves", false) || !findSavePath())
	{
		LOG("SaveBackup: disabled\n");
		return;
	}

	saveLimit = config.getInt("main", "saveLimit", -1);

	BYTE *pSaveName;
	BYTE saveName[] = "DDDA.sav";
	if (FindData("SaveBackup", saveName, &pSaveName))
	{
		BYTE sig[] = { 0x8D, 0x93, 0xE6, 0x09, 0x00, 0x00,
						0x68, 0x00, 0x00, 0x00, 0x00 };
		*(LPDWORD)(sig + 7) = (DWORD)pSaveName;
		BYTE *pOffset;
		if (FindSignature("SaveBackup", sig, &pOffset))
			CreateHook("SaveBackup", pOffset, (LPVOID)HSaveGame, &oSaveGame);
	}
}
