#include "stdafx.h"
#include "iniConfig.h"
#include "dinput8.h"

void iniConfig::init()
{
	SetLastError(ERROR_SUCCESS);
	GetPrivateProfileSectionNamesA(buffer, sizeof buffer, fileName);
	if (GetLastError() == ERROR_FILE_NOT_FOUND)
		LOG("Config: file not found!\n");
}

bool iniConfig::get(LPCSTR section, LPCSTR key, bool allowEmpty)
{
	SetLastError(ERROR_SUCCESS);
	DWORD result = GetPrivateProfileStringA(section, key, nullptr, buffer, sizeof buffer, fileName);
	return GetLastError() != ERROR_FILE_NOT_FOUND && (allowEmpty || result > 0);
}

void iniConfig::removeKey(LPCSTR section, LPCSTR key) const { WritePrivateProfileStringA(section, key, nullptr, fileName); }

vector<int> iniConfig::getSectionInts(LPCSTR section)
{
	vector<int> keys;
	if (get(section, nullptr, false))
	{
		LPSTR nextKey = buffer;
		while (*nextKey != '\0')
		{
			LPSTR end;
			int val = strtol(nextKey, &end, 10);
			if (end != nextKey)
				keys.push_back(val);
			nextKey = nextKey + strlen(nextKey) + 1;
		}
	}
	return keys;
}

LPCSTR iniConfig::getStr(LPCSTR section, LPCSTR key, LPCSTR defValue)
{
	if (get(section, key, *defValue == 0))
		return buffer;
	LOG("Config: %s->%s has invalid value, using default (%s)\n", section, key, defValue);
	return defValue;
}

int iniConfig::getInt(LPCSTR section, LPCSTR key, int defValue)
{
	if (get(section, key))
	{
		LPSTR end;
		int val = strtol(buffer, &end, 0);
		if (end != buffer)
			return val;
	}
	LOG("Config: %s->%s has invalid value, using default (%d)\n", section, key, defValue);
	return defValue;
}

unsigned int iniConfig::getUInt(LPCSTR section, LPCSTR key, unsigned defValue)
{
	if (get(section, key))
	{
		LPSTR end;
		unsigned val = strtoul(buffer, &end, 0);
		if (end != buffer)
			return val;
	}
	LOG("Config: %s->%s has invalid value, using default (%u)\n", section, key, defValue);
	return defValue;
}

float iniConfig::getFloat(LPCSTR section, LPCSTR key, float defValue)
{
	if (get(section, key))
	{
		LPSTR end;
		float val = strtof(buffer, &end);
		if (end != buffer)
			return val;
	}
	LOG("Config: %s->%s has invalid value, using default (%.1f)\n", section, key, defValue);
	return defValue;
}

bool iniConfig::getBool(LPCSTR section, LPCSTR key, bool defValue)
{
	if (get(section, key))
	{
		if (_stricmp("true", buffer) == 0 || _stricmp("on", buffer) == 0)
			return true;
		if (_stricmp("false", buffer) == 0 || _stricmp("off", buffer) == 0)
			return false;
	}
	LOG("Config: %s->%s has invalid value, using default (%s)\n", section, key, defValue ? "on": "off");
	return defValue;
}

int iniConfig::getEnum(LPCSTR section, LPCSTR key, int defValue, pair<int, LPCSTR> map[], int size)
{
	if (get(section, key))
	{
		for (int i = 0; i < size; i++)
			if (_stricmp(map[i].second, buffer) == 0)
				return map[i].first;
	}
	LOG("Config: %s->%s has invalid value, using default (%d)\n", section, key, defValue);
	return defValue;
}

vector<int> iniConfig::getInts(LPCSTR section, LPCSTR key)
{
	if (get(section, key, true))
	{
		vector<int> list;
		char *context, *token = strtok_s(buffer, ";", &context);
		while (true)
		{
			if (token == nullptr)
				return list;
			LPSTR end;
			int val = strtol(token, &end, 0);
			if (end == token) break;
			list.push_back(val);
			token = strtok_s(nullptr, ";", &context);
		}
	}
	LOG("Config: %s->%s has invalid value, using default ()\n", section, key);
	return vector<int>();
}

vector<float> iniConfig::getFloats(LPCSTR section, LPCSTR key)
{
	if (get(section, key, true))
	{
		vector<float> list;
		char *context, *token = strtok_s(buffer, ";", &context);
		while (true)
		{
			if (token == nullptr)
				return list;
			LPSTR end;
			float val = strtof(token, &end);
			if (end == token) break;
			list.push_back(val);
			token = strtok_s(nullptr, ";", &context);
		}
	}
	LOG("Config: %s->%s has invalid value, using default ()\n", section, key);
	return vector<float>();
}

void iniConfig::setStr(LPCSTR section, LPCSTR key, LPCSTR value)
{
	snprintf(buffer, sizeof buffer, " %s", value);
	WritePrivateProfileStringA(section, key, buffer, fileName);
}

void iniConfig::setInt(LPCSTR section, LPCSTR key, int value)
{
	char buf[16];
	snprintf(buf, sizeof buf, "%d", value);
	setStr(section, key, buf);
}

void iniConfig::setUInt(LPCSTR section, LPCSTR key, unsigned value, bool hex)
{
	char buf[16];
	if (hex)
		snprintf(buf, sizeof buf, "0x%08X", value);
	else
		snprintf(buf, sizeof buf, "%u", value);
	setStr(section, key, buf);
}

void iniConfig::setFloat(LPCSTR section, LPCSTR key, float value)
{
	char buf[64];
	snprintf(buf, sizeof buf, "%f", value);
	setStr(section, key, buf);
}

void iniConfig::setBool(LPCSTR section, LPCSTR key, bool value) { setStr(section, key, value ? "on": "off"); };

void iniConfig::setEnum(LPCSTR section, LPCSTR key, int value, pair<int, LPCSTR> map[], int size)
{
	for (int i = 0; i < size; i++)
		if (map[i].first == value)
		{
			setStr(section, key, map[i].second);
			return;
		}
	setInt(section, key, value);
}

void iniConfig::setInts(LPCSTR section, LPCSTR key, vector<int> list)
{
	char str[1024];
	char *p = str;
	size_t n = 1024;
	*p = 0;
	for (size_t i = 0; i < list.size(); i++)
	{
		if (i)
		{
			if (n < 2) break;
			*(p++) = ';';
			n--;
		}
		size_t s = snprintf(p, n, "%d", list[i]);
		if (s >= n) break;
		p += s;
		n -= s;
	}
	setStr(section, key, str);
}

void iniConfig::setFloats(LPCSTR section, LPCSTR key, vector<float> list)
{
	char str[1024];
	char *p = str;
	size_t n = 1024;
	*p = 0;
	for (size_t i = 0; i < list.size(); i++)
	{
		if (i)
		{
			if (n < 2) break;
			*(p++) = ';';
			n--;
		}
		size_t s = snprintf(p, n, "%f", list[i]);
		if (s >= n) break;
		p += s;
		n -= s;
	}
	setStr(section, key, str);
}
