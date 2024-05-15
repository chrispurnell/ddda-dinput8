#include "stdafx.h"
#include "SaveBackup.h"
#include "dinput8.h"
#include "steam_api.h"

namespace
{

struct fileInfo
{
	UINT64 time;
	string name;
	fileInfo(UINT64 t, const char *n) : time(t), name(n) { }
};
inline bool operator<(const fileInfo &a, const fileInfo &b) { return a.time < b.time; }

CHAR saveDir[PATH_MAX];
CHAR savePath[PATH_MAX];
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
	CHAR path[PATH_MAX];
	sprintf_s(path, PATH_MAX, "%s\\ddda_*.sav", saveDir);
	HANDLE hFind = FindFirstFile(path, &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	vector<fileInfo> files;
	do
	{
		SYSTEMTIME t;
		if (sscanf_s(ffd.cFileName, "ddda_%04hu-%02hu-%02hu_%02hu-%02hu-%02hu.sav",
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
		CHAR file[PATH_MAX];
		if (sprintf_s(file, "%s\\%s", saveDir, files[i].name.c_str()) > 0)
		{
			DeleteFile(file);
			LOG("Backup deleted: %s\n", file);
		}
	}
}

void __stdcall handleSave()
{
	WIN32_FILE_ATTRIBUTE_DATA fileData;
	if (GetFileAttributesEx(savePath, GetFileExInfoStandard, &fileData))
	{
		SYSTEMTIME systemTime, t;
		FileTimeToSystemTime(&fileData.ftLastWriteTime, &systemTime);
		SystemTimeToTzSpecificLocalTime(nullptr, &systemTime, &t);

		CHAR backupPath[PATH_MAX];
		sprintf_s(backupPath, "%s\\ddda_%04hu-%02hu-%02hu_%02hu-%02hu-%02hu.sav", saveDir, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

		if (CopyFile(savePath, backupPath, FALSE))
		{
			LOG("Backup created: %s\n", backupPath);
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

char *strrstr(char *x, const char *y)
{
	char *p = nullptr;
	char *n;
	if (*y == 0)
		return x + strlen(x);
	while ((n = strstr(x, y)) != nullptr)
	{
		p = n;
		x = n + 1;
	}
	return p;
}

bool findSavePath()
{
	LPCSTR configDir = config.getStr("main", "savePath");
	if (!*configDir)
	{
		HMODULE hModule = GetModuleHandle("steam_api.dll");
		if (hModule)
		{
			tSteamUser pSteamUser = (tSteamUser)GetProcAddress(hModule, "SteamUser");
			if (pSteamUser)
			{
				char buf[1024];
				ISteamUser *steamUser = pSteamUser();
				if (steamUser && steamUser->GetUserDataFolder(buf, 1024))
				{
					char *s = strrstr(buf, "local");
					if (s)
					{
						*s = 0;
						sprintf_s(saveDir, "%sremote", buf);
					}
					else
						strcpy_s(saveDir, buf);
				}
			}
		}
	}
	else
		strcpy_s(saveDir, configDir);

	char *p = saveDir;
	for (int i = 0; i < PATH_MAX; i++)
	{
		char c = saveDir[i];
		if (!c) break;
		if (c != '\\') p = saveDir + 1 + i;
	}
	*p = 0;

	if (!saveDir[0])
	{
		LOG("SaveBackup path: NOT FOUND, SET MANUALLY IN DINPUT8.INI\n");
		return false;
	}

	DWORD attributes = GetFileAttributes(saveDir);
	if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
	{
		LOG("SaveBackup path: INVALID PATH - %s\n", saveDir);
		return false;
	}

	sprintf_s(savePath, PATH_MAX, "%s\\ddda.sav", saveDir);
	LOG("SaveBackup path: %s\n", savePath);
	return true;
}

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
